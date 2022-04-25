#ifndef MASTER_HPP_SENTRY
#define MASTER_HPP_SENTRY

#include "vehicle.hpp"

class Master : public Vehicle {
        unsigned long target_visited;
        Vector2d target;
public:
        Master(const Vector2d& coord)
                : Vehicle(coord, Vehicle::master_max_speed, white), 
                target_visited(-1), target(coord) {}
        virtual ~Master() {}
        virtual void ChangeTargets(const Array<Circle>& targets);
        virtual Vector2d GetTarget() const { return target; }
        unsigned long TargetVisited() const { return target_visited; }
};

#endif /* MASTER_HPP_SENTRY */

