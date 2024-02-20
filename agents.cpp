#include "agents.hpp"

void Evader::Runaway(Environment& map, Array<Pursuer>& pursuers)
{
        Vector2d k_targ(1, -1);
        Update(map, k_targ);
}