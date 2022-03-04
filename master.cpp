#include <cstdlib>
#include "master.hpp"

void Master::ChangeTargets(const TargetSet& set)
{
        Vector2d s = target - coord;
        if (s.Module() < vehicle_radius + 15.0) {
                int k = rand() % set.GetAmount();
                target = set[k];
                colour = set.GetCol(k);
        }
}

