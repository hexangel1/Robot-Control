#ifndef AGENTS_HPP_SENTRY
#define AGENTS_HPP_SENTRY

#include "vehicle.hpp"

class Pursuer : public Vehicle {
public:
        Pursuer(const Vector2d& coord, int color = red)
                : Vehicle(coord, Vehicle::master_max_speed, color) {}
};

class Evader : public Vehicle {
public:
        Evader(const Vector2d& coord)
                : Vehicle(coord, Vehicle::slave_max_speed, green) {}
        void Runaway(Environment& map, Array<Pursuer>& pursuers);
};

#endif

