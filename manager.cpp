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
        glNewList(box, GL_COMPILE);
        Display();
        SetTargets();
        glEndList();
        Vehicle::ReadConfig("config.txt");
        AddMaster(new Master(Vector2d(1100, 900), yellow));
        AddMaster(new Master(Vector2d(1800, 780), magenta));
        AddMaster(new Master(Vector2d(300, 200), blue));
        AddMaster(new Master(Vector2d(1800, 150), cyan));
        AddMaster(new Master(Vector2d(500, 250), orange));
/*
AddMaster(new Master(Vector2d(300, 900), white));
        AddMaster(new Master(Vector2d(20, 650), rose));
        AddMaster(new Master(Vector2d(200, 450), khaki));
        AddMaster(new Master(Vector2d(330, 100), indigo));
        AddMaster(new Master(Vector2d(1200, 700), dgreen)); */
        map.Init(objects);
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
        CheckCollision();
}

void Manager::Show(bool info, bool drop)
{
        if (drop) {
                for (int i = 0; i < amount; i++) {
                        if (robots[i]->Colour() != yellow)
                                continue;
                        robots[i]->WriteLog();
                }
        }
        glCallList(box);
        for (int i = 0; i < amount; i++)
                robots[i]->Show();
        if (info) {
                for (int i = 0; i < amount; i++) {
                        robots[i]->ShowActiveWindow();
                        robots[i]->ShowFreeValleys();
                }
        }
        for (int i = 0; i < amount; i++)
                robots[i]->ShowDirection();
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
        Slave *q = new Slave(p->GetXY() + Vector2d(0.0, -30.0), p->Colour(), 0);
        q->Bind(p);
        AddVehicle(p);
        AddVehicle(q);
}

void Manager::AddVehicle(Vehicle *ptr)
{
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

void Manager::CheckCollision() const
{
        for (int i = 0; i < amount - 1; i++) {
                for (int j = i + 1; j < amount; j++) {
                        Vector2d v = robots[i]->GetXY() - robots[j]->GetXY();
                        double s = robots[i]->Radius() + robots[j]->Radius();
                        double distance = v.Module() - s;
                        if (distance <= 0.01) {
                                robots[i]->Stop();
                                robots[j]->Stop();
                        }
                }
        }
#ifdef CHECK_OBSTACLES
        for (int i = 0; i < amount; i++) {
                Vector2d coord = robots[i]->GetXY();
                for (GraphObjectItem *tmp = objects; tmp; tmp = tmp->next) {
                        Vector2d dir(robots[i]->Radius(), 0.0);
                        for (int k = 0; k < 8; k++) {
                                if (tmp->elem->IsInside(coord + dir)) {
                                        robots[i]->Stop();
                                        goto next_robot;
                                }
                                dir.Rotate(DEG2RAD(45));
                        }
                }
next_robot: ;
        }
#endif
}

