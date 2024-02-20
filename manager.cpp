#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>

#include "manager.hpp"
#include "ddpg.hpp"

#ifdef GRAPHICS_ENABLE
#include <GLFW/glfw3.h>
#endif

Manager::Manager(int width, int height, int episode)
        : map(width / Environment::cell_size, height / Environment::cell_size),
        agents(false),
        evader_spawns(dgreen), pursuer_spawns(dred),
        episode_iter(0), episode_length(episode)
{
        simulation_time = 0;
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

void Manager::Init()
{
        BuildMap();
        map.Init(obstacles);
        InitCollisionArrays();
        for (size_t i = 0; i < pursuers.Size(); i++)
                agents.Add(pursuers[i]);
        for (size_t i = 0; i < evaders.Size(); i++)
                agents.Add(evaders[i]);
        ddpg_conn = ddpg_connect();
#ifdef GRAPHICS_ENABLE
        glNewList(box, GL_COMPILE);
        evader_spawns.SpawnCompile();
        pursuer_spawns.SpawnCompile();
        ShowObstacles();
        glEndList();
#endif
}

void Manager::BuildMap()
{
        pursuers.Add(new Pursuer(Vector2d(1100, 900)));
        pursuers.Add(new Pursuer(Vector2d(100, 780)));
        pursuers.Add(new Pursuer(Vector2d(100, 200)));
        evaders.Add(new Evader(Vector2d(300, 400)));

        pursuer_spawns.AddSpawn(Vector2d(150, 190));
        pursuer_spawns.AddSpawn(Vector2d(350, 190));

        pursuer_spawns.AddSpawn(Vector2d(200, 900));
        pursuer_spawns.AddSpawn(Vector2d(600, 900));
        pursuer_spawns.AddSpawn(Vector2d(1000, 900));

        pursuer_spawns.AddSpawn(Vector2d(200, 50));
        pursuer_spawns.AddSpawn(Vector2d(600, 50));
        pursuer_spawns.AddSpawn(Vector2d(1000, 50));


        evader_spawns.AddSpawn(Vector2d(500, 450));

        evader_spawns.AddSpawn(Vector2d(300, 400));

        evader_spawns.Reset();
        pursuer_spawns.Reset();

        for (int i = 200; i < 1200; i += 400)
                obstacles.Add(new Rectangle(Vector2d(i, 620), red, 100, 200));
        for (int i = 400; i < 1200; i += 400)
                obstacles.Add(new Ellipse(Vector2d(i, 820), red, 100, 50));
        for (int i = 230; i < 1200; i += 400)
                obstacles.Add(new Rectangle(Vector2d(i, 210), red, 90, 200));
        for (int i = 400; i < 1200; i += 400)
                obstacles.Add(new Pentagon(Vector2d(i, 330), red, 50));
        for (int i = 400; i < 1200; i += 400)
                obstacles.Add(new Hexagon(Vector2d(i, 600), red, 50));

        for (int i = 50; i < 1200; i += 400)
                obstacles.Add(new Triangle(Vector2d(i, 200), red, 50)); 
}

#ifdef GRAPHICS_ENABLE
void Manager::Show(bool info)
{
        glCallList(box);
        if (info) {
                for (size_t i = 0; i < agents.Size(); i++) {
                        agents[i]->ShowActiveWindow();
                        agents[i]->ShowFreeValleys();
                }
        }
        ShowRobots();
}

void Manager::ShowRobots() const
{
        for (size_t i = 0; i < agents.Size(); i++) {
                agents[i]->Show();
                agents[i]->ShowDirection();
        }
}

void Manager::ShowObstacles() const
{
        for (size_t i = 0; i < obstacles.Size(); i++)
                obstacles[i]->Show();
}
#endif

void Manager::InitCollisionArrays()
{
        size_t size = agents.Size() * (agents.Size() - 1) / 2;
        vehicle_collisions = new bool[size];
        obstacle_collisions = new bool[agents.Size()];
        for (size_t i = 0; i < size; i++)
                vehicle_collisions[i] = false;
        for (size_t i = 0; i < agents.Size(); i++)
                obstacle_collisions[i] = false;
}

double Manager::EvaluateReward(bool& done) const
{
        done = true;
        return 1.0;
}

void Manager::MakeResponse(ddpg_response *resp) const
{
        for (size_t i = 0; i < agents.Size(); i++) {
                Vector2d coord = agents[i]->GetXY();
                resp->state[2 * i] = coord.X();
                resp->state[2 * i + 1] = coord.Y();
        }
        resp->done = false;
        resp->reward = EvaluateReward(resp->done);
}

void Manager::NewEpisode()
{
        for (size_t i = 0; i < pursuers.Size(); i++)
                pursuers[i]->Respawn(map, pursuer_spawns.GetSpawn());
        for (size_t i = 0; i < evaders.Size(); i++)
                evaders[i]->Respawn(map, evader_spawns.GetSpawn());
        pursuer_spawns.Reset();
        evader_spawns.Reset();
        episode_iter = 0;
}

void Manager::Sample()
{
        ddpg_action action_buf;
        get_action(ddpg_conn, &action_buf);
        for (int i = 0; i < 6; i++)
                action_buf.action[i] = 1.0;
        for (size_t i = 0; i < pursuers.Size(); i++)
                pursuers[i]->Update(map, Vector2d(action_buf.action[i], action_buf.action[i+1]));
        for (size_t i = 0; i < evaders.Size(); i++)
                evaders[i]->Runaway(map, pursuers);
        CheckCollision();
        ddpg_response response_buf;
        MakeResponse(&response_buf);
        episode_iter++;
        if (episode_iter == episode_length) {
                NewEpisode();
                response_buf.done = true;
        }
        response_action(ddpg_conn, &response_buf);
        simulation_time++;
}

void Manager::CheckCollision()
{
        size_t idx = 0;
        if (agents.Size() == 0)
                return;
        for (size_t i = 0; i < agents.Size() - 1; i++) {
                for (size_t j = i + 1; j < agents.Size(); j++, idx++) {
                        Vector2d v = agents[i]->GetXY() - agents[j]->GetXY();
                        double s = agents[i]->Radius() + agents[j]->Radius();
                        double distance = v.Module() - s;
                        if (distance <= 0.01) {
                                if (!vehicle_collisions[idx])
                                        vehicle_crashes++;
                                agents[i]->Stop();
                                agents[j]->Stop();
                                vehicle_collisions[idx] = true;
                        } else {
                                vehicle_collisions[idx] = false;
                        }
                        
                }
        }
#ifdef CHECK_OBSTACLES
        for (size_t i = 0; i < agents.Size(); i++) {
                Vector2d coord = agents[i]->GetXY();
                for (size_t j = 0; j < obstacles.Size(); j++) {
                        Vector2d dir(agents[i]->Radius(), 0.0);
                        for (int k = 0; k < 8; k++) {
                                if (obstacles[j]->IsInside(coord + dir)) {
                                        if (!obstacle_collisions[i])
                                                obstacle_crashes++;
                                        agents[i]->Stop();
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

