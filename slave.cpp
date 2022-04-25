#include "slave.hpp"

void Slave::ChangeTargets(const Array<Circle>& targets)
{
        Vector2d s = target->GetXY() - coord;
        if (s.Module() > 150.0)
                lead_loss_time++;
}
 
