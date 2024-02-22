#include "agents.hpp"

void Evader::Runaway(Environment& map, Array<Pursuer>& pursuers)
{
        Vector2d k_targ(-1.0 + 2.0 * (double)rand() / RAND_MAX, -1.0 + 2.0 * (double)rand() / RAND_MAX);
        Update(map, k_targ);
}