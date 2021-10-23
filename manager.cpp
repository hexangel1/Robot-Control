#include "manager.hpp"

Manager::Manager(int width, int height) : map(width, height)
{
        objects = 0;
        robots = new Vehicle*[4];
        allocated = 4;
        amount = 0;
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
        AddRobot(new Vehicle(Vector2D(1100, 100), yellow, Vector2D(900, 900)));
        AddRobot(new Vehicle(Vector2D(1800, 220), magenta, Vector2D(100, 700)));
        AddRobot(new Vehicle(Vector2D(300, 800), blue, Vector2D(600, 80)));
        AddRobot(new Vehicle(Vector2D(1800, 850), cyan, Vector2D(100, 150)));
        AddRobot(new Vehicle(Vector2D(500, 750), orange, Vector2D(900, 100)));
        AddRobot(new Vehicle(Vector2D(300, 100), white, Vector2D(1800, 700)));
        AddRobot(new Vehicle(Vector2D(20, 350), rose, Vector2D(1800, 80))); 
        AddRobot(new Vehicle(Vector2D(200, 550), khaki, Vector2D(1800, 500))); 
        AddObject(new Square(Vector2D(1200, 430), red, 90));
        for (int i = 200; i < 1700; i += 400)
                AddObject(new Rectangle(Vector2D(i, 380), red, 100, 200));
        for (int i = 400; i < 1700; i += 400)
                AddObject(new Ellipse(Vector2D(i, 180), red, 100, 50));
        for (int i = 200; i < 1700; i += 400)
                AddObject(new Rectangle(Vector2D(i, 800), red, 90, 200));
        for (int i = 400; i < 1700; i += 400)
                AddObject(new Ellipse(Vector2D(i, 700), red, 50, 100));
        MapInit();
        Display();
}

void Manager::Update(bool paused, bool info)
{
        if (!paused) {
                for (int i = 0; i < amount; i++)
                        robots[i]->Update(objects, map);
        }
        Display();
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
        AddObject(ptr);
        AddObject(new Circle(ptr->GetTarget(), ptr->Colour(), 15.0));
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

