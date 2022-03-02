#ifndef SLAVE_HPP_SENTRY
#define SLAVE_HPP_SENTRY

#include "vehicle.hpp"

class Master;

class Slave : public Vehicle {
        Master *target;
public:
        Slave(const Vector2d coord, int colour, Master *p)
                : Vehicle(coord, colour), target(p) {}
        void Bind(Master *p) { target = p; }
        virtual ~Slave() {}
        virtual void ChangeTargets(const TargetSet& set) {}
        virtual Vector2d GetTarget() const { return target->GetXY(); }
};

#endif /* SLAVE_HPP_SENTRY */

