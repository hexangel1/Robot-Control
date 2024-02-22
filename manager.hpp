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

        double *pursuer_distance;
        double *pursuer_distance_old;
        bool *vehicle_collisions;
        bool *obstacle_collisions;
        unsigned int vehicle_crashes;
        unsigned int obstacle_crashes;
        unsigned int box;
        int ddpg_conn;

        unsigned int episode_iter;
        unsigned int simulation_time;
        const unsigned int episode_iter_max;
        const unsigned int simulation_time_max;
        bool graphics_mode;
public:
        Manager(int width, int height, unsigned int ep_iter, unsigned int total_iter,
                bool graphics);
        ~Manager();
        void Init();
        void Sample();
        void Show(bool info);
        bool Finished() const { return simulation_time >= simulation_time_max; }
private:
        void BuildMap();
        void InitCollisionArrays();
        void NewEpisode();
        void ShowRobots() const;
        void ShowObstacles() const;
        void EvalDistances() const;
        double EvaluateReward(int& done) const;
        void CheckCollision();
        void MakeResponse(ddpg_response *resp) const;
};

#endif /* MANAGER_HPP_SENTRY */

