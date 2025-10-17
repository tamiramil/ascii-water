#pragma once

#ifndef ASCII_WATER_WAVE2D_HPP
#define ASCII_WATER_WAVE2D_HPP

#include <cstddef>
#include <vector>


class Wave2D {
public:
    Wave2D(size_t size, double speed, double base_damping);

    void update_wave(double delta);

    void add_random_disturbance(double min_v, double max_v);

    void reset();

    const std::vector<std::vector<double>> &get_surface() const;

    void set_damping(double val);

    const double &get_damping() const;

    void set_speed(double val);

    const double &get_speed() const;

private:
    const double MAX_DAMPING = 0.2;
    const double SPONGE_FRACTION = 0.125;

    size_t _size;
    double _speed;
    double _base_damping;

    int _sponge_thickness;

    std::vector<std::vector<double>> _hp;
    std::vector<std::vector<double>> _hc;
    std::vector<std::vector<double>> _hn;
    std::vector<std::vector<double>> _damping_mask;

    void calculate_damping_mask();
};


#endif //ASCII_WATER_WAVE2D_HPP