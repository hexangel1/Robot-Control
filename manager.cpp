#include "manager.hpp"
#include <GLFW/glfw3.h>

Manager::Manager(int width, int height)
        : map(width / Environment::cell_size, height / Environment::cell_size)
{
        objects = 0;
        robots = new Vehicle*[4];
        allocated = 4;
        amount = 0;
        box = glGenLists(1);
}

Manager::~Manager()
{
        GraphObjectItem *tmp;
        while (objects) {
                tmp = objects;
                objects = objects->next;
                delete tmp->elem;
                delete tmp;
        }
        delete[] robots;
}

void Manager::Init()
{
#ifndef TEST_VERSION
        for (int i = 200; i < 1700; i += 400)
                AddObject(new Rectangle(Vector2d(i, 620), red, 100, 200));
        for (int i = 400; i < 1700; i += 400)
                AddObject(new Ellipse(Vector2d(i, 820), red, 100, 50));
        for (int i = 200; i < 1700; i += 400)
                AddObject(new Rectangle(Vector2d(i, 200), red, 90, 200));
        for (int i = 400; i < 1700; i += 400)
                AddObject(new Ellipse(Vector2d(i, 300), red, 50, 100));
        for (int i = 400; i < 1700; i += 400)
                AddObject(new Triangle(Vector2d(i, 550), red, 70));
#else
        double x = 1000, y = 700;
        AddObject(new Triangle(Vector2d(x-200, y),
                               Vector2d(x, y),
                               Vector2d(x-200, y-100), 
                               red));
        AddObject(new Triangle(Vector2d(x+200, y),
                               Vector2d(x, y),
                               Vector2d(x+200, y-100), 
                               red));
#endif
        glNewList(box, GL_COMPILE);
        Display();
#ifndef TEST_VERSION
        AddMaster(new Master(Vector2d(1100, 900), yellow));
        AddMaster(new Master(Vector2d(1800, 780), magenta));
        AddMaster(new Master(Vector2d(300, 200), blue));
        AddMaster(new Master(Vector2d(1800, 150), cyan));
        AddMaster(new Master(Vector2d(500, 250), orange));
        AddMaster(new Master(Vector2d(300, 900), white));
        AddMaster(new Master(Vector2d(20, 650), rose));
        AddMaster(new Master(Vector2d(200, 450), khaki));
        AddMaster(new Master(Vector2d(330, 100), indigo));
        AddMaster(new Master(Vector2d(1200, 700), dgreen));
#else
        Slave *tmp = new Slave(Vector2d(995.5, 100), yellow, 0);
        Master *tmp2 = new Master(Vector2d(800, 101), green);
        tmp->Bind(tmp2);
        AddRobot(tmp);
        AddRobot(tmp2);
#endif
        SetTargets();
        glEndList();
        MapInit();
}

void Manager::SetTargets()
{
        set.Add(Vector2d(900, 100), yellow);
        set.Add(Vector2d(100, 300), magenta);
        set.Add(Vector2d(600, 920), blue);
        set.Add(Vector2d(100, 850), cyan);
        set.Add(Vector2d(900, 900), orange);
        set.Add(Vector2d(1800, 300), white);
        set.Add(Vector2d(1800, 920), rose);
        set.Add(Vector2d(1800, 500), khaki);
        set.Add(Vector2d(1400, 900), indigo);
        set.Add(Vector2d(400, 100), dgreen);
        set.Show();
}

void Manager::Update()
{
        for (int i = 0; i < amount; i++)
                robots[i]->Update(map, set);
}

void Manager::Show(bool info, bool drop)
{
        if (drop) {
                for (int i = 0; i < amount; i++) {
                        if (robots[i]->Colour() != yellow)
                                continue;
                        robots[i]->WriteHistogram();
//                        robots[i]->WriteState();
                }
        }
        glCallList(box);
        for (int i = 0; i < amount; i++)
                robots[i]->Show();
        if (info) {
                for (int i = 0; i < amount; i++)
                        robots[i]->ShowInfo();
        }
}

void Manager::MapInit()
{
        GraphObjectItem *tmp;
        for (tmp = objects; tmp; tmp = tmp->next)
                tmp->elem->Mapping(map, true);
}

void Manager::AddObject(GraphObject *ptr)
{
        GraphObjectItem *tmp = new GraphObjectItem;
        tmp->elem = ptr;
        tmp->next = objects;
        objects = tmp;
}

void Manager::AddMaster(Master *p)
{
//        Slave *q = new Slave(p->GetXY() + Vector2d(0.0, -30.0), p->Colour(), 0);
  //      q->Bind(p);
        AddVehicle(p);
    //    AddVehicle(q);
}

void Manager::AddVehicle(Vehicle *ptr)
{
        AddObject(ptr);
        if (amount == allocated) {
                allocated <<= 1;
                Vehicle **tmp = new Vehicle*[allocated];
                for (int i = 0; i < amount; i++)
                        tmp[i] = robots[i];
                delete[] robots;
                robots = tmp;
        }
        robots[amount] = ptr;
        amount++;
}

void Manager::Display() const
{
        GraphObjectItem *tmp;
        for (tmp = objects; tmp; tmp = tmp->next)
                tmp->elem->Show();
}

