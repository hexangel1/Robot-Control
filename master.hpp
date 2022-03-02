#ifndef MASTER_HPP_SENTRY
#define MASTER_HPP_SENTRY

#include "vehicle.hpp"

class Master : public Vehicle {
        Vector2d target;
public:
        Master(const Vector2d& coord, int colour)
                : Vehicle(coord, colour), target(coord) {}
        virtual ~Master() {}
        virtual void ChangeTargets(const TargetSet& set);
        virtual Vector2d GetTarget() const { return target; }
};

#endif /* MASTER_HPP_SENTRY */

