#ifndef SPAWNSET_HPP_SENRTY
#define SPAWNSET_HPP_SENRTY

#include "graphobject.hpp"
#include "array.hpp"

class SpawnSet {
        Array<Circle> spawns;
        bool *spawn_used;
        int colour;
public:
        SpawnSet(int col) : spawn_used(0), colour(col) {}
        ~SpawnSet() { delete []spawn_used; }
        Vector2d GetSpawn() const;
        void AddSpawn(const Vector2d& spawn) { spawns.Add(new Circle(spawn, colour, 15)); }
        void SpawnCompile();
        void Reset();
};

#endif
