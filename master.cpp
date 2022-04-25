#include <cstdlib>
#include "master.hpp"

void Master::ChangeTargets(const Array<Circle>& targets)
{
        Vector2d s = target - coord;
        if (s.Module() < Radius() + 15.0) {
                target_visited++;
                int k = rand() % targets.Size();
                target = targets[k]->GetXY();
                colour = targets[k]->Colour();
        }
}

