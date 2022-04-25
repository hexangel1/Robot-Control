#ifndef SLAVE_HPP_SENTRY
#define SLAVE_HPP_SENTRY

#include "vehicle.hpp"
#include "master.hpp"

class Slave : public Vehicle {
        unsigned long lead_loss_time;
        Master *target;
public:
        Slave(const Vector2d coord, Master *p)
                : Vehicle(coord, Vehicle::slave_max_speed, white),
                 lead_loss_time(0), target(p) {}
        void Bind(Master *p) { target = p; }
        virtual ~Slave() {}
        virtual void ChangeTargets(const Array<Circle>& targets);
        virtual Vector2d GetTarget() const { return target->GetXY(); }
        unsigned long LeadLossTime() const { return lead_loss_time; }
};

#endif /* SLAVE_HPP_SENTRY */

