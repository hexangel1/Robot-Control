#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include "manager.hpp"
#include <GLFW/glfw3.h>

const int Manager::colour_set[] = {
        green, blue, yellow, magenta, cyan,
        white, brown, khaki, indigo, orange,
        rose, dred, dgreen, dblue
};
 
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
        for (int i = 0; i < amount; i++)
                delete robots[i];
        delete[] robots;
        glDeleteLists(box, 1);
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
        Vehicle::ReadConfig("scripts/script1/config.txt");
        InitMap("scripts/script1/map.txt");
        map.Init(objects);
        glNewList(box, GL_COMPILE);
        GraphObjectItem *tmp;
        for (tmp = objects; tmp; tmp = tmp->next)
                tmp->elem->Show();
        set.Show();
        glEndList();
}

void Manager::SetTarget(double x, double y)
{
        static int i = 0;
        set.Add(Vector2d(x, y), colour_set[i]);
        i++;
        set.Show();
}

bool Manager::IsComment(const char *str)
{
        int i;
        for (i = 0; str[i] && isspace(str[i]); i++)
                ;
        return str[i] == '#';
}

void Manager::InitMap(const char *file)
{
        char buf[32], var[32], cmd[32];
        int res;
        double x, y;
        FILE *fp;
        fp = fopen(file, "r");
        if (!fp) {
                perror(file);
                return;
        }
        while (fgets(buf, sizeof(buf), fp)) {
                if (IsComment(buf))
                        continue;
                res = sscanf(buf, "%s %s %*[(] %lf %*[,] %lf %*[)]",
                             cmd, var, &x, &y);
                if (res != 4 || strcmp(cmd, "set")) {
                        fprintf(stderr, "Parsing error\n");
                        fclose(fp);
                        return;
                }
                if (!strcmp(var, "robot"))
                        SetRobot(x, y);
                else if (!strcmp(var, "target"))
                        SetTarget(x, y);
                else
                        fprintf(stderr, "unknown command: %s\n", var);
        }
        fclose(fp);
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

void Manager::SetRobot(double x, double y)
{
        static int i = 0;
        Master *m = new Master(Vector2d(x, y), red);
        Slave *s = new Slave(Vector2d(x + 0.0, y - 30.0), colour_set[i], 0);
        i++;
        s->Bind(m);
        AddVehicle(m);
        AddVehicle(s);
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

