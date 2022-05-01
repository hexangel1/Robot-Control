#include "slave.hpp"
#include <cstdio>

void Slave::ChangeTargets(const Array<Circle>& targets)
{
        Vector2d s = target->GetXY() - coord;
        if (s.Module() > Vehicle::max_distance)
                lead_loss_time++;

}
 
