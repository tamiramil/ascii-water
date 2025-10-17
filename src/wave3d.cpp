#include "wave2d.hpp"

#include <algorithm>
#include <random>

Wave2D::Wave2D(const size_t size,
               const double speed,
               const double base_damping) {
    _size = size;
    _speed = speed;
    _base_damping =
            (base_damping > 0. && base_damping < 1.) ? base_damping : .5;

    _sponge_thickness =
            std::max(static_cast<int>(size * .5 * SPONGE_FRACTION), 1);

    _hp = std::vector(size, std::vector<double>(size, 0.));
    _hc = std::vector(size, std::vector<double>(size, 0.));
    _hn = std::vector(size, std::vector<double>(size, 0.));

    calculate_damping_mask();
}

void Wave2D::calculate_damping_mask() {
    _damping_mask = std::vector(_size, std::vector<double>(_size));

    for (size_t i = 0; i < _size; ++i) {
        for (size_t j = 0; j < _size; ++j) {
            int distance = std::min({i, j, _size - i - 1, _size - j - 1});
            double s = 1. - static_cast<double>(distance) / _sponge_thickness;
            s = std::clamp(s, 0., 1.);
            _damping_mask[i][j] = _base_damping + (1. - _base_damping) * s * s;
        }
    }
}

void Wave2D::update_wave(const double delta) {
    double alpha = _speed * delta;
    alpha *= alpha;

    if (alpha >= 1.) return;

    for (size_t i = 0; i < _size; ++i) {
        for (size_t j = 0; j < _size; ++j) {
            if (i == 0 || j == 0 || i == _size - 1 || j == _size - 1) {
                _hn[i][j] = 0.;
                continue;
            }

            _hn[i][j] = 2. * _hc[i][j] - _hp[i][j] +
                        alpha * (_hc[i + 1][j] + _hc[i - 1][j] +
                                 _hc[i][j + 1] + _hc[i][j - 1] -
                                 4. * _hc[i][j]);
            _hn[i][j] *= 1. - _damping_mask[i][j];
        }
    }

    std::swap(_hp, _hc);
    std::swap(_hc, _hn);
}

void Wave2D::add_random_disturbance(double min_v, double max_v) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<double> distrib_double(min_v, max_v);
    std::uniform_int_distribution<>
            distrib_int(_sponge_thickness, _size - _sponge_thickness - 1);

    size_t x = distrib_int(gen);
    size_t y = distrib_int(gen);

    _hc[x][y] += distrib_double(gen);
}

void Wave2D::reset() {
    _hp = std::vector(_size, std::vector<double>(_size, 0.));
    _hc = std::vector(_size, std::vector<double>(_size, 0.));
}

const std::vector<std::vector<double>> &Wave2D::get_surface() const {
    return _hc;
}

void Wave2D::set_damping(const double val) {
    if (val <= 0. || val >= MAX_DAMPING) return;
    _base_damping = val;
    calculate_damping_mask();
}

const double &Wave2D::get_damping() const {
    return _base_damping;
}

void Wave2D::set_speed(const double val) {
    if (val <= 0.) return;
    _speed = val;
}

const double &Wave2D::get_speed() const {
    return _speed;
}