#ifndef MANAGER_HPP_SENTRY
#define MANAGER_HPP_SENTRY

#include "vehicle.hpp"
#include "master.hpp"
#include "slave.hpp"
#include "graphobject.hpp"
#include "targetset.hpp"

class Manager {
        Environment map;
        TargetSet set;
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
        void Show(bool info, bool drop);
private:
        void SetTargets();
        void AddObject(GraphObject *ptr);
        void AddMaster(Master *p);
        void AddVehicle(Vehicle *ptr);
        void Display() const;
};

#endif /* MANAGER_HPP_SENTRY */

