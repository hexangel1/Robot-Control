#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include "manager.hpp"

#ifdef GRAPHICS_ENABLE
#include <GLFW/glfw3.h>
#endif

Manager::Manager(int width, int height)
        : map(width / Environment::cell_size, height / Environment::cell_size)
{
        simulation_time = 0;
        target_visited_sum = 0;
        target_visited_min = ~0UL;
        lead_loss_time_sum = 0;
        lead_loss_time_max = 0;
        vehicle_crashes = 0;
        obstacle_crashes = 0;
        vehicle_collisions = 0;
        obstacle_collisions = 0;
#ifdef GRAPHICS_ENABLE
        box = glGenLists(1);
#endif
}

Manager::~Manager()
{
        delete[] vehicle_collisions;
        delete[] obstacle_collisions;
#ifdef GRAPHICS_ENABLE
        glDeleteLists(box, 1);
#endif
}

void Manager::PrintStatistics()
{
        CalculateStat();
 //       printf("Robots amount: %ld\n", robots.Size());
 //       printf("Simulation time: %ld\n", simulation_time);
        printf("Target visited mean:  %.1f\n",
               static_cast<double>(target_visited_sum) /
               static_cast<double>(robots.Size() / 2));
        printf("Target visited worse: %ld\n", target_visited_min);
        printf("Leader loss time mean:  %.2f %%\n",
               100.0 * static_cast<double>(lead_loss_time_sum) / 
               static_cast<double>(robots.Size() / 2 * simulation_time));
        printf("Leader loss time worse: %.2f %%\n",
               100.0 * static_cast<double>(lead_loss_time_max) /
               static_cast<double>(simulation_time));
        printf("Vehicle collision:  %ld\n", vehicle_crashes);
}

void Manager::Init()
{
        BuildMap();
        map.Init(obstacles);
        InitCollisionArrays();
#ifdef GRAPHICS_ENABLE
        glNewList(box, GL_COMPILE);
        ShowTargets();
        ShowObstacles();
        glEndList();
#endif
}

void Manager::BuildMap()
{
        robots.Add(new Master(Vector2d(1100, 900)));
        robots.Add(new Master(Vector2d(1800, 780)));
        robots.Add(new Master(Vector2d(100, 200)));
        robots.Add(new Master(Vector2d(1800, 150)));
        robots.Add(new Master(Vector2d(500, 250)));
        robots.Add(new Master(Vector2d(250, 900)));
        robots.Add(new Master(Vector2d(900, 200)));
        robots.Add(new Master(Vector2d(1200, 200)));
//        robots.Add(new Master(Vector2d(250, 900)));
//        robots.Add(new Master(Vector2d(250, 900)));
        SetSlaves();
        SetTarget(150, 100);
        SetTarget(530, 100);
        SetTarget(910, 100);
        SetTarget(1290, 100);
        SetTarget(1670, 100);
        
        SetTarget(150, 900);
        SetTarget(600, 900);
        SetTarget(1000, 900);
        SetTarget(1400, 900);
        SetTarget(1800, 900);
        SetTarget(900, 450);
        for (int i = 200; i < 1700; i += 400)
                obstacles.Add(new Rectangle(Vector2d(i, 620), red, 100, 200));
        for (int i = 400; i < 1700; i += 400)
                obstacles.Add(new Ellipse(Vector2d(i, 820), red, 100, 50));
        for (int i = 230; i < 1700; i += 400)
                obstacles.Add(new Rectangle(Vector2d(i, 210), red, 90, 200));
        for (int i = 400; i < 1700; i += 400)
                obstacles.Add(new Pentagon(Vector2d(i, 330), red, 50));
        for (int i = 400; i < 1700; i += 400)
                obstacles.Add(new Hexagon(Vector2d(i, 600), red, 50));
        obstacles.Add(new Triangle(Vector2d(1800, 500), red, 50));
        for (int i = 50; i < 1800; i += 400)
                obstacles.Add(new Triangle(Vector2d(i, 200), red, 50)); 
}

void Manager::CalculateStat()
{
        size_t n = robots.Size() / 2;
        for (size_t i = 0; i < n; i++) {
                Master *m = static_cast<Master*>(robots[i]);
                target_visited_sum += m->TargetVisited();
                if (target_visited_min > m->TargetVisited())
                        target_visited_min = m->TargetVisited();
        }
        for (size_t i = n; i < 2 * n; i++) {
                Slave *s = static_cast<Slave*>(robots[i]);
                lead_loss_time_sum += s->LeadLossTime();
                if (lead_loss_time_max < s->LeadLossTime())
                        lead_loss_time_max = s->LeadLossTime();
        }
}

void Manager::SetTarget(double x, double y)
{
        static int i = 0;
        static const int colour_set[] = {
                green, blue, yellow, magenta, cyan,
                brown, khaki, indigo, orange, rose,
                dred, dgreen, dblue
        };
        targets.Add(new Circle(Vector2d(x, y), colour_set[i], 15.0));
        i++;
}

#ifdef GRAPHICS_ENABLE
void Manager::Show(bool info)
{
        glCallList(box);
        if (info) {
                for (size_t i = 0; i < robots.Size(); i++) {
                        robots[i]->ShowActiveWindow();
                        robots[i]->ShowFreeValleys();
                }
        }
        ShowRobots();
}

void Manager::ShowRobots() const
{
        for (size_t i = 0; i < robots.Size(); i++) {
                robots[i]->Show();
                robots[i]->ShowDirection();
        }
}

void Manager::ShowTargets() const
{
        for (size_t i = 0; i < targets.Size(); i++)
                targets[i]->Show();
}

void Manager::ShowObstacles() const
{
        for (size_t i = 0; i < obstacles.Size(); i++)
                obstacles[i]->Show();
}
#endif

void Manager::InitCollisionArrays()
{
        size_t size = robots.Size() * (robots.Size() - 1) / 2;
        vehicle_collisions = new bool[size];
        obstacle_collisions = new bool[robots.Size()];
        for (size_t i = 0; i < size; i++)
                vehicle_collisions[i] = false;
        for (size_t i = 0; i < robots.Size(); i++)
                obstacle_collisions[i] = false;
}

void Manager::Sample()
{
        for (size_t i = 0; i < robots.Size(); i++)
                robots[i]->Update(map, targets);
        CheckCollision();
        simulation_time++;
}

void Manager::SetSlaves()
{
        size_t n = robots.Size();
        for (size_t i = 0; i < n; i++) {
                Vector2d coord = robots[i]->GetXY() - Vector2d(0.0, -30.0);
                robots.Add(new Slave(coord, static_cast<Master*>(robots[i])));
        }
}

void Manager::CheckCollision()
{
        size_t idx = 0;
        if (robots.Size() == 0)
                return;
        for (size_t i = 0; i < robots.Size() - 1; i++) {
                for (size_t j = i + 1; j < robots.Size(); j++, idx++) {
                        Vector2d v = robots[i]->GetXY() - robots[j]->GetXY();
                        double s = robots[i]->Radius() + robots[j]->Radius();
                        double distance = v.Module() - s;
                        if (distance <= 0.01) {
                                if (!vehicle_collisions[idx])
                                        vehicle_crashes++;
                                robots[i]->Stop();
                                robots[j]->Stop();
                                vehicle_collisions[idx] = true;
                        } else {
                                vehicle_collisions[idx] = false;
                        }
                        
                }
        }
#ifdef CHECK_OBSTACLES
        for (size_t i = 0; i < robots.Size(); i++) {
                Vector2d coord = robots[i]->GetXY();
                for (size_t j = 0; j < obstacles.Size(); j++) {
                        Vector2d dir(robots[i]->Radius(), 0.0);
                        for (int k = 0; k < 8; k++) {
                                if (obstacles[j]->IsInside(coord + dir)) {
                                        if (!obstacle_collisions[i])
                                                obstacle_crashes++;
                                        robots[i]->Stop();
                                        obstacle_collisions[i] = true;
                                        goto next_robot;
                                }
                                dir.Rotate(DEG2RAD(45));
                        }
                }
                obstacle_collisions[i] = false;
next_robot: ;
        }
#endif
}

