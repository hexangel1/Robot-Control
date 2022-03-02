#include <cmath>
#include <cstdio>
#include <GLFW/glfw3.h>
#include "vehicle.hpp"

const int Vehicle::window_size = 41;
const int Vehicle::histogram_size = 72;
const double Vehicle::vehicle_radius = 8.0;
const double Vehicle::speed_constant = 0.015;
const double Vehicle::max_speed = 50.0;
const double Vehicle::max_boost = 1.0;
const double Vehicle::max_angle_speed = 0.02;
const double Vehicle::max_obstacle_density = 80.0;
const double Vehicle::safe_distance = 80.0;

Vehicle::Vehicle(const Vector2d& coord, int colour)
        : Circle(coord, colour, vehicle_radius),
        active_region(window_size, window_size),
        obstacle_density(histogram_size)
{
        angle = 0.0;
        speed = 0.0;
        boost = 0.0;
        angular_speed = 0.0;
        accident = false;
        vall = 0;
        ctrl = 0;
}

void Vehicle::Update(Environment& map, const TargetSet& set)
{
        if (accident)
                return;
        while (vall) {
                Valley *tmp = vall;
                vall = vall->next;
                delete tmp;
        }
        ChangeTargets(set);
        Mapping(map, false);
        coord += speed_constant * speed * Vector2d(cos(angle), sin(angle));
        speed += boost;
        angle += angular_speed;
        angle = sin(angle) < 0.0 ? 2 * PI - acos(cos(angle)) : acos(cos(angle));
        speed = SAT(speed, 0.0, max_speed);
        active_region.CopyRegion(map, coord.X(), coord.Y());
        obstacle_density.Build(active_region);
        obstacle_density.Smooth();
        vall = obstacle_density.GetValleys(0.01);
        Control();
        if (!CheckMove(map))
                accident = true;
        Mapping(map, true);
}


void Vehicle::WriteState() const
{
        fprintf(stderr, "x = %lf\n", coord.X());
        fprintf(stderr, "y = %lf\n", coord.Y());
        fprintf(stderr, "angle = %lf\n", angle);
        fprintf(stderr, "speed = %lf\n", speed);
        fprintf(stderr, "%lf\n", obstacle_density[GetSector(angle)]);
}
     
void Vehicle::ShowInfo() const
{
        ShowActiveWindow();
        ShowFreeValleys();
}

void Vehicle::Control()
{
        int k = SteerControl();
        double s = SpeedControl(GetSector(angle));
        boost = SAT(s - speed, -max_boost, max_boost);
        double phi1 = fabs(GetAngel(k) - angle);
        double phi2 = 2*PI - phi1;
        bool use_phi1 = phi1 <= phi2;
        if (GetAngel(k) - angle >= 0)
                phi2 *= -1;
        else
                phi1 *= -1;
        angular_speed = use_phi1 ? phi1 : phi2;
        angular_speed = SAT(angular_speed, -max_angle_speed, max_angle_speed);
        ctrl = k;
}

double Vehicle::GetAngel(int k) const
{
        return 2 * PI * k / (double)histogram_size;
}

int Vehicle::GetSector(double phi) const
{
        return phi / (2 * PI) * (double)histogram_size;
}

int Vehicle::SteerControl() const
{
        Valley *tmp, *best = 0;
        double score, best_score;
        for (tmp = vall; tmp; tmp = tmp->next) {
                score = fmax(Score(tmp->begin), Score(tmp->end));
                if (!best || score > best_score) {
                        best_score = score;
                        best = tmp;
                }
        }
        if (!best)
                return -1;
        if (best->end - best->begin <= 24)
                return (best->end + best->begin) / 2 % histogram_size;
        if (Score(best->begin) >= Score(best->end))
                return (best->begin + 24 + best->begin) / 2 % histogram_size;
        return (best->end - 24 + best->end) / 2 % histogram_size;
}

double Vehicle::SpeedControl(int k) const
{
        double max = 0.0;
        for (int i = -3; i <= 3; i++) {
                if (obstacle_density[k + i] >= max)
                        max = obstacle_density[k + i];
        }
        double h = fmin(max, max_obstacle_density);
        return max_speed * (1.0 - h / max_obstacle_density);
}

double Vehicle::Score(int k) const
{
        Vector2d dir = Direction(k);
        Vector2d goal = GetTarget() - coord;
        goal.Normalize();
        return goal * dir;
}

Vector2d Vehicle::Direction(int k) const
{
        const double step = 360.0 / histogram_size;
        Vector2d dir(1.0, 0.0);
        dir.Rotate(DEG2RAD(k * step + step / 2.0));
        return dir;
}

bool Vehicle::IsTargetReached() const
{
        return abs((GetTarget() - coord).Module() - safe_distance) <= Radius();
}

bool Vehicle::CheckMove(const Environment& map) const
{
        Vector2d ray(Radius(), 0.0);
        for (int k = 0; k < 8; k++) {
                int j = (int)(coord + ray).X() / Environment::cell_size;
                int i = (int)(coord + ray).Y() / Environment::cell_size;
                if (map.Get(i, j) > 0.0)
                        return false;
                ray.Rotate(DEG2RAD(45.0));
        }
        return true;
}

void Vehicle::ShowActiveWindow() const
{
        double offset = window_size / 2 * Environment::cell_size;
        glDisable(GL_MULTISAMPLE);
        glBegin(GL_LINE_LOOP);
        glColor3ub(0, 255, 0);
        glVertex2f(coord.X() - offset, coord.Y() - offset);
        glVertex2f(coord.X() - offset, coord.Y() + offset);
        glVertex2f(coord.X() + offset, coord.Y() + offset);
        glVertex2f(coord.X() + offset, coord.Y() - offset);
        glEnd();
        glEnable(GL_MULTISAMPLE);
}

void Vehicle::ShowFreeValleys() const
{
        int col;
        glBegin(GL_LINES);
        for (Valley *tmp = vall; tmp; tmp = tmp->next) {
                col = tmp->begin <= ctrl && tmp->end >= ctrl ? green : yellow;
                for (int k = tmp->begin; k <= tmp->end; k++) {
                        Vector2d dir = Direction(k);
                        Vector2d a = coord + dir * (Radius() + 8.0);
                        Vector2d b = coord + dir * Radius() * 10.0;
                        glColor3ub(RED(col), GREEN(col), BLUE(col));
                        glVertex2f(a.X(), a.Y());
                        glVertex2f(b.X(), b.Y());
                }
        }
        glEnd();
}

