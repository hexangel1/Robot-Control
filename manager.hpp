#ifndef MANAGER_HPP_SENTRY
#define MANAGER_HPP_SENTRY

#include "vehicle.hpp"
#include "agents.hpp"
#include "graphobject.hpp"
#include "array.hpp"
#include "spawnset.hpp"
#include "ddpg.hpp"

class Manager {
        Environment map;

        Array<Pursuer> pursuers;
        Array<Evader> evaders;
        Array<Vehicle> agents;
        Array<GraphObject> obstacles;
        SpawnSet evader_spawns;
        SpawnSet pursuer_spawns;
        unsigned long simulation_time;
        unsigned long vehicle_crashes;
        unsigned long obstacle_crashes;
        bool *vehicle_collisions;
        bool *obstacle_collisions;
        unsigned int box;
        int ddpg_conn;
        int episode_iter;
        int episode_length;
public:
        Manager(int width, int height, int episode);
        ~Manager();
        void Init();
        void Sample();
        void Show(bool info);
        unsigned long Time() const { return simulation_time; }
private:
        void BuildMap();
        void InitCollisionArrays();
        void NewEpisode();
        void ShowRobots() const;
        void ShowObstacles() const;
        double EvaluateReward(bool& done) const;
        void CheckCollision();
        void MakeResponse(ddpg_response *resp) const;
};

#endif /* MANAGER_HPP_SENTRY */

