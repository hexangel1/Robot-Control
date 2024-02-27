#include "agents.hpp"

void Evader::Runaway(Environment& map, Array<Pursuer>& pursuers)
{
        Array<Vector2d> danger;
        for (size_t i = 0; i < pursuers.Size(); i++) {
                if ((pursuers[i]->GetXY() - GetXY()).Module() < 350.0)
                        danger.Add(new Vector2d(pursuers[i]->GetXY() - GetXY()));
        }
        if (danger.Size() == 0)
                return;
        Vector2d k_targ;
        if (danger.Size() == 1) {
                k_targ = -(*danger[0]);
        } else {
               Vector2d dir1 = Vector2d::Bis(*danger[0], *danger[1]);
               Vector2d dir2 = Vector2d::Bis(*danger[1], *danger[0]);
               k_targ = dir1 * (*danger[0]) < dir2 * (*danger[0]) ? dir1 : dir2;
        }

        Update(map, k_targ);
}