#ifndef MANAGER_HPP_SENTRY
#define MANAGER_HPP_SENTRY

#include "vehicle.hpp"
#include "graphobject.hpp"

class Manager {
        Environment map;
        GraphObjectItem *objects;
        Vehicle **robots;
        int allocated;
        int amount;
        unsigned int box;
public:
        Manager(int width, int height);
        ~Manager();
        void Init();
        void Update();
        void Show(bool info);
private:
        void MapInit();
        void AddObject(GraphObject *ptr);
        void AddRobot(Vehicle *ptr);
        void Display() const;
};

#endif /* MANAGER_HPP_SENTRY */

