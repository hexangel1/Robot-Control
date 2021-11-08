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
                AddObject(new Rectangle(Vector2d(i, 380), red, 100, 200));
        for (int i = 400; i < 1700; i += 400)
                AddObject(new Ellipse(Vector2d(i, 180), red, 100, 50));
        for (int i = 200; i < 1700; i += 400)
                AddObject(new Rectangle(Vector2d(i, 800), red, 90, 200));
        for (int i = 400; i < 1700; i += 400)
                AddObject(new Ellipse(Vector2d(i, 700), red, 50, 100));
        for (int i = 400; i < 1700; i += 400)
                AddObject(new Triangle(Vector2d(i, 450), red, 70));
        glNewList(box, GL_COMPILE);
        Display();
        AddRobot(new Vehicle(Vector2d(1100, 100), yellow, Vector2d(900, 900)));
        AddRobot(new Vehicle(Vector2d(1800, 220), magenta, Vector2d(100, 700)));
        AddRobot(new Vehicle(Vector2d(300, 800), blue, Vector2d(600, 80)));
        AddRobot(new Vehicle(Vector2d(1800, 850), cyan, Vector2d(100, 150)));
        AddRobot(new Vehicle(Vector2d(500, 750), orange, Vector2d(900, 100)));
        AddRobot(new Vehicle(Vector2d(300, 100), white, Vector2d(1800, 700)));
        AddRobot(new Vehicle(Vector2d(20, 350), rose, Vector2d(1800, 80)));
        AddRobot(new Vehicle(Vector2d(200, 550), khaki, Vector2d(1800, 500)));
        AddRobot(new Vehicle(Vector2d(330, 900), indigo, Vector2d(1400, 100)));
        AddRobot(new Vehicle(Vector2d(1200, 300), dgreen, Vector2d(400, 900)));
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

