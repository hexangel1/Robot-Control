#ifndef MANAGER_HPP_SENTRY
#define MANAGER_HPP_SENTRY

#include "graphobject.hpp"
#include "vehicle.hpp"

class Manager {
        LocalMap map;
        GraphObjectItem *objects;
        Vehicle **robots;
        int allocated;
        int amount;
public:
        Manager(int width, int height);
        ~Manager();
        void Init();
        void Update(bool paused, bool info);
private:
        void MapInit();
        void AddObject(GraphObject *ptr);
        void AddRobot(Vehicle *ptr);
        void Display() const;
};

#endif /* MANAGER_HPP_SENTRY */
