#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <GLFW/glfw3.h>

#include "manager.hpp"
#include "ddpg.hpp"

Manager::Manager(int width, int height, unsigned int ep_iter,  unsigned int total_iter, bool graphics)
        : map(width / Environment::cell_size, height / Environment::cell_size),
        agents(false),
        evader_spawns(dgreen), pursuer_spawns(dred),
        env_width(width), env_height(height),
        episode_iter_max(ep_iter), simulation_time_max(total_iter), graphics_mode(graphics)
{
        simulation_time = 0;
        episode_iter = 0;
        vehicle_crashes = 0;
        obstacle_crashes = 0;
        vehicle_collisions = 0;
        obstacle_collisions = 0;
        if (graphics_mode)
                box = glGenLists(1);
}

Manager::~Manager()
{
        delete[] vehicle_collisions;
        delete[] obstacle_collisions;

        if (graphics_mode)
                glDeleteLists(box, 1);
}

bool Manager::Init(int timeout)
{
        ddpg_conn = ddpg_connect(timeout);
        if (ddpg_conn == -1)
                return false;
        BuildMap();
        map.Init(obstacles);
        for (size_t i = 0; i < pursuers.Size(); i++)
                agents.Add(pursuers[i]);
        for (size_t i = 0; i < evaders.Size(); i++)
                agents.Add(evaders[i]);
        InitCollisionArrays();
        if (graphics_mode) {
                glNewList(box, GL_COMPILE);
                evader_spawns.SpawnCompile();
                pursuer_spawns.SpawnCompile();
                ShowObstacles();
                glEndList();
        }
        return true;
}

void Manager::BuildMap()
{
        pursuers.Add(new Pursuer(Vector2d(1100, 900), blue));
        pursuers.Add(new Pursuer(Vector2d(100, 780)));
        //pursuers.Add(new Pursuer(Vector2d(100, 200)));
        evaders.Add(new Evader(Vector2d(300, 400)));

        pursuer_spawns.AddSpawn(Vector2d(150, 150));
        pursuer_spawns.AddSpawn(Vector2d(1005.000000, 801.000000));
        pursuer_spawns.AddSpawn(Vector2d(143.000000, 819.000000));
        pursuer_spawns.AddSpawn(Vector2d(1011.000000, 127.000000));
        pursuer_spawns.AddSpawn(Vector2d(580.000000, 494.000000));



        evader_spawns.AddSpawn(Vector2d(182.000000, 700.000000));
        evader_spawns.AddSpawn(Vector2d(364.000000, 842.000000));
        evader_spawns.AddSpawn(Vector2d(321.000000, 686.000000));
        evader_spawns.AddSpawn(Vector2d(508.000000, 740.000000));
        evader_spawns.AddSpawn(Vector2d(246.000000, 481.000000));
        evader_spawns.AddSpawn(Vector2d(500.000000, 593.000000));
        evader_spawns.AddSpawn(Vector2d(615.000000, 588.000000));
        evader_spawns.AddSpawn(Vector2d(688.000000, 591.000000));
        evader_spawns.AddSpawn(Vector2d(715.000000, 471.000000));
        evader_spawns.AddSpawn(Vector2d(415.000000, 447.000000));
        evader_spawns.AddSpawn(Vector2d(425.000000, 281.000000));
        evader_spawns.AddSpawn(Vector2d(592.000000, 273.000000));
        evader_spawns.AddSpawn(Vector2d(829.000000, 280.000000));
        evader_spawns.AddSpawn(Vector2d(905.000000, 411.000000));
        evader_spawns.AddSpawn(Vector2d(1045.000000, 462.000000));
        evader_spawns.AddSpawn(Vector2d(1026.000000, 346.000000));
        evader_spawns.AddSpawn(Vector2d(1070.000000, 253.000000));
        evader_spawns.AddSpawn(Vector2d(983.000000, 192.000000));
        evader_spawns.AddSpawn(Vector2d(888.000000, 129.000000));
        evader_spawns.AddSpawn(Vector2d(782.000000, 162.000000));
        evader_spawns.AddSpawn(Vector2d(850.000000, 256.000000));
        evader_spawns.AddSpawn(Vector2d(706.000000, 359.000000));
        evader_spawns.AddSpawn(Vector2d(260.000000, 783.000000));
        evader_spawns.AddSpawn(Vector2d(88.000000, 585.000000));
        evader_spawns.AddSpawn(Vector2d(666.000000, 371.000000));
        evader_spawns.AddSpawn(Vector2d(121.000000, 565.000000));



        evader_spawns.AddSpawn(Vector2d(560, 450));

        evader_spawns.Reset();
        pursuer_spawns.Reset();
        NewEpisode();
        // for (int i = 200; i < 1200; i += 400)
        //         obstacles.Add(new Rectangle(Vector2d(i, 620), red, 100, 200));
        // for (int i = 400; i < 1200; i += 400)
        //         obstacles.Add(new Ellipse(Vector2d(i, 820), red, 100, 50));
        // for (int i = 230; i < 1200; i += 400)
        //         obstacles.Add(new Rectangle(Vector2d(i, 210), red, 90, 200));
        // for (int i = 400; i < 1200; i += 400)
        //         obstacles.Add(new Pentagon(Vector2d(i, 330), red, 50));
        // for (int i = 400; i < 1200; i += 400)
        //         obstacles.Add(new Hexagon(Vector2d(i, 600), red, 50));

        // for (int i = 50; i < 1200; i += 400)
        //         obstacles.Add(new Triangle(Vector2d(i, 200), red, 50)); 
}

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

void Manager::EvalDistances() const
{
        for (size_t i = 0; i < pursuers.Size(); i++)
                pursuer_distance[i] = (pursuers[i]->GetXY() - evaders[0]->GetXY()).Module();
}

void Manager::InitCollisionArrays()
{
        size_t size = agents.Size() * (agents.Size() - 1) / 2;
        vehicle_collisions = new bool[size];
        obstacle_collisions = new bool[agents.Size()];
        pursuer_distance = new double[pursuers.Size()];
        pursuer_distance_old = new double[pursuers.Size()];
        EvalDistances();
        memcpy(pursuer_distance_old, pursuer_distance, sizeof(double)*pursuers.Size());
        for (size_t i = 0; i < size; i++)
                vehicle_collisions[i] = false;
        for (size_t i = 0; i < agents.Size(); i++)
                obstacle_collisions[i] = false;
}

double Manager::EvaluateReward(int& done) const
{
        double reward = 0.0;
        EvalDistances();
        for (size_t i = 0; i < pursuers.Size(); i++) {
                Vector2d coord = pursuers[i]->GetXY();

                if (pursuer_distance[i] < 80.0) {
                        fprintf(stderr, "HIT!!!\n");
                        reward = 10.0;
                        done = true;
                        break;
                }
                if (pursuer_distance[i] < 100.0)
                        reward += 0.2;
                else if (pursuer_distance[i] < 150.0)
                        reward += 0.15;
                else if (pursuer_distance[i] < 200.0)
                        reward += 0.1;
                else if (pursuer_distance[i] < 250.0)
                        reward += 0.05;
        
                if (pursuer_distance[i] < pursuer_distance_old[i]) {
                        reward += 0.1;
                } else {
                        double gap = 60.0;
                        if (coord.X() < gap || coord.X() > env_width - gap ||
                            coord.Y() < gap || coord.Y() > env_height - gap)
                                reward -= 1.0;
                        else
                                reward -= 0.2;
                }
        }
        return reward;
}

void Manager::MakeResponse(ddpg_response *resp) const
{
        for (size_t i = 0; i < agents.Size(); i++) {
                Vector2d coord = agents[i]->GetXY();
                Vector2d dir = agents[i]->CurrentDirection();
                resp->state[2 * i] = -1.0 + 2.0 * coord.X() / env_width;
                resp->state[2 * i + 1] = -1.0 + 2.0 * coord.Y() / env_height;
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
        if (episode_iter == 0) {
                ddpg_response initial_state;
                MakeResponse(&initial_state);
                response_action(ddpg_conn, &initial_state);
        }
        ddpg_action action_buf;
        get_action(ddpg_conn, &action_buf);
        for (size_t i = 0; i < pursuers.Size(); i++)
                pursuers[i]->Update(map, Vector2d(action_buf.action[2*i], action_buf.action[2*i+1]));
        for (size_t i = 0; i < evaders.Size(); i++)
                evaders[i]->Runaway(map, pursuers);
        CheckCollision();
        ddpg_response response_buf;
        MakeResponse(&response_buf);
        episode_iter++;
        if (episode_iter == episode_iter_max || response_buf.done) {
                NewEpisode();
                response_buf.done = 1;
        }
        response_action(ddpg_conn, &response_buf);
        simulation_time++;
        double *tmp;
        tmp = pursuer_distance;
        pursuer_distance = pursuer_distance_old;
        pursuer_distance_old = tmp;
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

