#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <cmath>
#include <GLFW/glfw3.h>
#include "vehicle.hpp"

int Vehicle::window_size = 41;
int Vehicle::histogram_size = 72;
int Vehicle::max_valley_size = 21;
double Vehicle::vehicle_radius = 6.0;
double Vehicle::speed_constant = 0.015;
double Vehicle::boost_resist = 0.75;
double Vehicle::max_speed = 100.0;
double Vehicle::max_boost = 140.0;
double Vehicle::max_angle_speed = 1.5708;
double Vehicle::max_obstacle_density = 20.0;

Vehicle::Vehicle(const Vector2d& coord, int colour)
        : Circle(coord, colour, vehicle_radius),
        active_region(window_size, window_size),
        obstacle_density(histogram_size)
{
        angle = 0.0;
        speed = 0.0;
        boost = 0.0;
        angular_speed = 0.0;
        stopped = false;
        vall = 0;
}

void Vehicle::Update(Environment& map, const TargetSet& set)
{
        if (stopped)
                return;
        FreeValleys();
        ChangeTargets(set);
        Mapping(map, false);
        coord += speed_constant * speed * Vector2d(cos(angle), sin(angle));
        angle += speed_constant * angular_speed;
        speed += boost;
        angle = sin(angle) < 0.0 ? 2 * PI - acos(cos(angle)) : acos(cos(angle));
        speed = SAT(speed, 0.0, max_speed);
        active_region.CopyRegion(map, coord.X(), coord.Y());
        obstacle_density.Build(active_region);
        obstacle_density.Smooth();
        vall = obstacle_density.GetValleys(5.0);
        Control();
        Mapping(map, true); 
}

void Vehicle::WriteLog() const
{
        fprintf(stderr, "x     = %f\n", coord.X());
        fprintf(stderr, "y     = %f\n", coord.Y());
        fprintf(stderr, "angle = %f\n", angle);
        fprintf(stderr, "speed = %f\n", speed);
}
     
void Vehicle::Control()
{
        int k = SteerControl();
        double s = SpeedControl(GetSector(angle));
        double phi1 = fabs(GetAngel(k) - angle);
        double phi2 = 2 * PI - phi1;
        bool use_phi1 = phi1 <= phi2;
        if (GetAngel(k) - angle >= 0)
                phi2 *= -1;
        else
                phi1 *= -1;
        angular_speed = use_phi1 ? phi1 : phi2;
        angular_speed = SAT(angular_speed, -max_angle_speed, max_angle_speed);
        double r = boost_resist * fabs(angular_speed) / max_angle_speed;
        s = fmin(s, (1.0 - r) * max_speed);
        boost = SAT(s - speed, -max_boost, max_boost);
}

void Vehicle::FreeValleys()
{
        while (vall) {
                Valley *tmp = vall;
                vall = vall->next;
                delete tmp;
        }
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
        int k_targ = GetSector((GetTarget() - coord).Argument());
        if (best->begin == 0 && best->end == histogram_size - 1)
                return k_targ;
        if (best->end - best->begin >= 1.5 * (double)max_valley_size &&
            k_targ >= (best->begin + max_valley_size) % histogram_size && 
            k_targ <= (best->end - max_valley_size) % histogram_size)
                return k_targ;
        if (best->end - best->begin <= max_valley_size)
                return (best->end + best->begin) / 2 % histogram_size;
        if (Score(best->begin) < Score(best->end))
                return (2 * best->end - max_valley_size) / 2 % histogram_size;
        return (2 * best->begin + max_valley_size) / 2 % histogram_size;
}

double Vehicle::SpeedControl(int k) const
{
        double max = 0.0;
        for (int i = -2; i <= 2; i++) {
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

void Vehicle::ShowDirection() const
{
        Vector2d a = coord + 23.0 * Vector2d(cos(angle), sin(angle));
        Vector2d b = coord + 13.0 * 
                Vector2d(cos(angle + 0.5), sin(angle + 0.5));
        Vector2d c = coord + 13.0 * 
                Vector2d(cos(angle - 0.5), sin(angle - 0.5));
        glBegin(GL_TRIANGLES);
        glColor3ub(RED(red), GREEN(red), BLUE(red));
        glVertex2f(a.X(), a.Y());
        glVertex2f(b.X(), b.Y());
        glVertex2f(c.X(), c.Y());
        glEnd();
}

void Vehicle::ShowActiveWindow() const
{
        double offset = window_size / 2 * Environment::cell_size;
        glBegin(GL_LINE_LOOP);
        glColor3ub(0, 255, 0);
        glVertex2f(coord.X() - offset, coord.Y() - offset);
        glVertex2f(coord.X() - offset, coord.Y() + offset);
        glVertex2f(coord.X() + offset, coord.Y() + offset);
        glVertex2f(coord.X() + offset, coord.Y() - offset);
        glEnd();
}

void Vehicle::ShowFreeValleys() const
{
        glLineWidth(1.6);
        glBegin(GL_LINES);
        for (Valley *tmp = vall; tmp; tmp = tmp->next) {
                for (int k = tmp->begin; k <= tmp->end; k++) {
                        Vector2d dir = Direction(k);
                        Vector2d a = coord + dir * (Radius() + 8.0);
                        Vector2d b = coord + dir * Radius() * 10.0;
                        glColor3ub(RED(green), GREEN(green), BLUE(green));
                        glVertex2f(a.X(), a.Y());
                        glVertex2f(b.X(), b.Y());
                }
        }
        glEnd();
}

void Vehicle::Mapping(Environment& map, bool s) const
{
        double x, y;
        int size = Environment::cell_size;
        for (x = -Radius(); x < Radius(); x++) {
                for (y = -Radius(); y < Radius(); y++) {
                        if (IsInside(coord + Vector2d(x, y))) {
                                int j = (int)(coord.X() + x) / size;
                                int i = (int)(coord.Y() + y) / size;
                                map.Set(i, j, s ? 1.0 : 0.0);
                        }
                }
        }
}

bool Vehicle::ReadConfig(const char *file)
{
        char buf[128], var[32], val[32];
        FILE *fp = fopen(file, "r");
        if (!fp) {
                perror(file);
                fprintf(stderr, "reading config file %s failed\n", file);
                return false;
        }
        while (fgets(buf, sizeof(buf), fp)) {
                int res = sscanf(buf, "%31s %*[=] %31s", var, val);
                if (res != 2) {
                        fprintf(stderr, "config file parsing error\n");
                        fclose(fp);
                        return false;
                }
                SetValue(var, val);
        }
        fclose(fp);
        return true;
}

void Vehicle::SetValue(const char *var, const char *value)
{
        if (!strcmp(var, "window_size"))
                window_size = atoi(value);
        else if (!strcmp(var, "histogram_size"))
                histogram_size = atoi(value);
        else if (!strcmp(var, "max_valley_size"))
                max_valley_size = atoi(value);
        else if (!strcmp(var, "vehicle_radius"))
                vehicle_radius = atof(value);
        else if (!strcmp(var, "speed_constant"))
                speed_constant = atof(value);
        else if (!strcmp(var, "max_speed"))
                max_speed = atof(value);
        else if (!strcmp(var, "boost_resist"))
                max_speed = atof(value);
        else if (!strcmp(var, "max_boost"))
                max_boost = atof(value);
        else if (!strcmp(var, "max_angle_speed"))
                max_angle_speed = atof(value);
        else if (!strcmp(var, "max_obstacle_density"))
                max_obstacle_density = atof(value);
        else
                fprintf(stderr, "variable %s not found\n", var);
}

