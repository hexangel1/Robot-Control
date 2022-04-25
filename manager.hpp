#ifndef MANAGER_HPP_SENTRY
#define MANAGER_HPP_SENTRY

#include "vehicle.hpp"
#include "master.hpp"
#include "slave.hpp"
#include "graphobject.hpp"
#include "array.hpp"

class Manager {
        Environment map;
        Array<Vehicle> robots;
        Array<Circle> targets;
        Array<GraphObject> obstacles;
        unsigned long simulation_time;
        unsigned long target_visited_sum;
        unsigned long target_visited_min;
        unsigned long lead_loss_time_sum;
        unsigned long lead_loss_time_max; 
        unsigned long vehicle_crashes;
        unsigned long obstacle_crashes;
        bool *vehicle_collisions;
        bool *obstacle_collisions;
        unsigned int box;
public:
        Manager(int width, int height);
        ~Manager();
        void Init();
        void Sample();
        void Show(bool info);
        void PrintStatistics();
        void CalculateStat();
private:
        void BuildMap();
        void InitCollisionArrays();
        void SetSlaves();
        void SetTarget(double x, double y);
        void ShowRobots() const;
        void ShowTargets() const;
        void ShowObstacles() const;
        void CheckCollision();
};

#endif /* MANAGER_HPP_SENTRY */

