#include "spawnset.hpp"

Vector2d SpawnSet::GetSpawn() const
{
        int spawn_no;
        do {
            spawn_no = (int)((double)rand() / (RAND_MAX + 1.0) * spawns.Size());
            if (spawn_used[spawn_no])
                    spawn_no = -1;
            else
                    spawn_used[spawn_no] = true;
        } while (spawn_no == -1);
        return spawns[spawn_no]->GetXY();
}

void SpawnSet::SpawnCompile()
{
        for (size_t i = 0; i < spawns.Size(); i++)
                spawns[i]->Show();
}

void SpawnSet::Reset()
{
        if (!spawn_used)
                spawn_used = new bool[spawns.Size()];
        for (size_t i = 0; i < spawns.Size(); i++)
                spawn_used[i] = false;
}