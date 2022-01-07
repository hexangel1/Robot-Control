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
        AddRobot(new Vehicle(Vector2d(1100, 900), yellow, Vector2d(900, 100)));
        AddRobot(new Vehicle(Vector2d(1800, 780), magenta, Vector2d(100, 300)));
        AddRobot(new Vehicle(Vector2d(300, 200), blue, Vector2d(600, 920)));
        AddRobot(new Vehicle(Vector2d(1800, 150), cyan, Vector2d(100, 850)));
        AddRobot(new Vehicle(Vector2d(500, 250), orange, Vector2d(900, 900)));
        AddRobot(new Vehicle(Vector2d(300, 900), white, Vector2d(1800, 300)));
        AddRobot(new Vehicle(Vector2d(20, 650), rose, Vector2d(1800, 920)));
        AddRobot(new Vehicle(Vector2d(200, 450), khaki, Vector2d(1800, 500)));
        AddRobot(new Vehicle(Vector2d(330, 100), indigo, Vector2d(1400, 900)));
        AddRobot(new Vehicle(Vector2d(1200, 700), dgreen, Vector2d(400, 100)));
        glEndList();
        MapInit();
}

void Manager::Update(bool paused, bool info)
{
        if (!paused) {
                for (int i = 0; i < amount; i++)
                        robots[i]->Update(robots, map);
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

void Manager::AddRobot(Vehicle *ptr)
{
        GraphObject *q;
        q = new Circle(ptr->GetTarget(), ptr->Colour(), 15.0);
        q->Show();
        AddObject(ptr);
        AddObject(q);
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

