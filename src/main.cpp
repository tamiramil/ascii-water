#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include <chrono>
#include <thread>
#include <cmath>

#include "wave2d.hpp"

std::vector<std::vector<sf::Vector2f>>
get_isometric_plane(size_t size, float angle_rad, float scale) {
    auto plane = std::vector(size, std::vector<sf::Vector2f>(size));

    float tan = std::tanf(angle_rad);
    float cot = 1.f / tan;

    float alpha = cot * .5f * scale;
    float beta = tan * .5f * scale;

    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            plane[i][j] = sf::Vector2f(static_cast<float>(i + j) * alpha,
                                       static_cast<float>(i - j + size - 1) *
                                       beta);
        }
    }

    return plane;
}

int main() {
    const size_t grid_size = 40;
    double speed = 4.;
    double damping = .02;

    const double speed_step = .1;
    const double damping_step = .005;

    const float grid_scale = 12.f;
    const float height_scale = 10.f;

    const double physics_dt = 0.04;
    double time_accum = 0.;
    double fps = 0.;

    auto base_plane = get_isometric_plane(grid_size, M_PI / 6., grid_scale);

    Wave2D wave(grid_size, speed, damping);

    sf::RenderWindow window(sf::VideoMode({900, 700}), "ascii_water");
    window.setFramerateLimit(60);

    sf::Clock clock;
    sf::Clock fps_clock;

    sf::Font font;
    if (!font.openFromFile("fonts/nimbusmono_bold.otf")) {
        return 1;
    }

    sf::Text hud(font);
    hud.setCharacterSize(12);
    hud.setFillColor(sf::Color::White);
    hud.setPosition({10.f, 10.f});

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                switch (keyPressed->scancode) {
                    case sf::Keyboard::Scancode::Escape:
                        window.close();
                        break;
                    case sf::Keyboard::Scancode::Space:
                        wave.add_random_disturbance(-10., 10.);
                        break;
                    case sf::Keyboard::Scancode::I:
                        wave.set_speed(speed - speed_step);
                        speed = wave.get_speed();
                        break;
                    case sf::Keyboard::Scancode::O:
                        wave.set_speed(speed + speed_step);
                        speed = wave.get_speed();
                        break;
                    case sf::Keyboard::Scancode::K:
                        wave.set_damping(damping - damping_step);
                        damping = wave.get_damping();
                        break;
                    case sf::Keyboard::Scancode::L:
                        wave.set_damping(damping + damping_step);
                        damping = wave.get_damping();
                        break;
                    case sf::Keyboard::Scancode::R:
                        wave.reset();
                        break;
                    default:
                        break;
                }
            }
        }

        float elapsed = fps_clock.getElapsedTime().asSeconds();
        fps_clock.restart();
        fps = 1. / std::max(elapsed, 0.0001f);

        time_accum += clock.getElapsedTime().asSeconds();
        clock.restart();

        while (time_accum >= physics_dt) {
            wave.update_wave(physics_dt);
            time_accum -= physics_dt;
        }

        auto surface = wave.get_surface();
        window.clear(sf::Color(20, 20, 30));

        sf::Vector2 win_size = window.getSize();
        auto offset = sf::Vector2f(
                (static_cast<float>(win_size.x) -
                 base_plane.back().back().x) * .5f,
                (static_cast<float>(win_size.y) * .5f -
                 base_plane.back().back().y)
        );

        sf::VertexArray grid(sf::PrimitiveType::Points);

        for (size_t i = 0; i < grid_size; ++i) {
            for (size_t j = 0; j < grid_size; ++j) {
                auto z = static_cast<float>(surface[i][j]);

                auto pos = sf::Vector2f(base_plane[i][j].x,
                                        base_plane[i][j].y + z * height_scale
                );

                auto intensity = static_cast<float>(
                        std::clamp((z + 1.) / 2., 0., 1.)
                );

                sf::Color color(
                        static_cast<std::uint8_t>(100 + 155 * intensity),
                        static_cast<std::uint8_t>(100 + 100 * intensity),
                        static_cast<std::uint8_t>(200 - 150 * intensity)
                );

                grid.append(sf::Vertex(pos + offset, color));
            }
        }

        sf::VertexArray h_line(sf::PrimitiveType::LineStrip);
        sf::VertexArray v_line(sf::PrimitiveType::LineStrip);

        for (size_t i = 0; i < grid_size; ++i) {
            for (size_t j = 0; j < grid_size; ++j) {
                size_t h_idx = i * grid_size + j;
                h_line.append(grid[h_idx]);
                size_t v_idx = j * grid_size + i;
                v_line.append(grid[v_idx]);
            }
            window.draw(h_line);
            window.draw(v_line);
            h_line.resize(0);
            v_line.resize(0);
        }

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "fps: " << std::setw(6) << std::setprecision(1) << fps << "\n"
           << "propagation speed: " << std::setprecision(2) << speed << "\n"
           << "damping: " << std::setprecision(3) << damping << "\n\n"
           << "I / O  - wave speed -/+ 0.1\n"
           << "K / L  - damping -/+ 0.005\n"
           << "R      - reset\n"
           << "Space  - disturbance";
        hud.setString(ss.str());

        window.draw(hud);
        window.display();
    }

    return 0;
}