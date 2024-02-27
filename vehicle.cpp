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
double Vehicle::boost_resist = 0.75;
double Vehicle::master_max_speed = 100.0;
double Vehicle::slave_max_speed = 100.0;
double Vehicle::max_boost = 140.0;
double Vehicle::max_angle_speed = 1.5708;
double Vehicle::max_obstacle_density = 20.0;
double Vehicle::max_distance = 150.0;

Vehicle::Vehicle(const Vector2d& coord, double speedmax, int colour)
        : Circle(coord, colour, vehicle_radius),
        active_region(window_size, window_size),
        obstacle_density(histogram_size)
{
        angle = 0.0;
        speed = 0.0;
        boost = 0.0;
        angular_speed = 0.0;
        stopped = false;
        max_speed = speedmax;
}

void Vehicle::Update(Environment& map, const Vector2d& k_targ)
{
        Mapping(map, false);
        // coord += speed * Vector2d(cos(angle), sin(angle));
        // angle += angular_speed;
        // speed += boost;
        // angle = sin(angle) < 0.0 ? 2 * PI - acos(cos(angle)) : acos(cos(angle));
        // speed = SAT(0.01 + speed, 0.01, max_speed);
        active_region.CopyRegion(map, coord.X(), coord.Y());
        obstacle_density.Build(active_region);
        obstacle_density.Smooth();
        obstacle_density.DeleteValleys();
        obstacle_density.SearchValleys(0.01);
        Control(k_targ);
        coord += speed * Vector2d(cos(angle), sin(angle));
        angle += angular_speed;
        speed += boost;
        angle = sin(angle) < 0.0 ? 2 * PI - acos(cos(angle)) : acos(cos(angle));
        speed = SAT(0.01 + speed, 0.01, max_speed);
        Mapping(map, true);
}

void Vehicle::Respawn(Environment& map, const Vector2d& spawn)
{
        Mapping(map, false);
        coord = spawn;
        angle = 0.0;
        speed = 0.0;
        boost = 0.0;
        angular_speed = 0.0;
        stopped = false;
        Mapping(map, true);
}

void Vehicle::Control(const Vector2d& k_targ)
{
        Vector2d cur_dir = Vector2d::Direction(angle);
        Vector2d dir = SteerControl(k_targ);
        double s = SpeedControl(GetSector(angle));
        angular_speed = SGN(cur_dir ^ dir) * acos(cur_dir * dir);
        if (speed <= 0.05)
                angular_speed += Normal(0, 0.15);
        angular_speed = SAT(angular_speed, -max_angle_speed, max_angle_speed);
        double r = boost_resist * fabs(angular_speed) / max_angle_speed;
        s = fmin(s, (1.0 - r) * max_speed);
        boost = SAT(s - speed, -max_boost, max_boost);
}

double Vehicle::GetAngle(int k) const
{
        return 2 * PI * double(k) / double(histogram_size);
}

int Vehicle::GetSector(double phi) const
{
        return phi / (2 * PI) * (double)histogram_size;
}

Vector2d Vehicle::SteerControl(const Vector2d& _k_targ) const
{
        Valley *tmp, *best = 0;
        Vector2d k_targ = _k_targ;
        k_targ.Normalize();
        double score, best_score;
        for (tmp = obstacle_density.GetValleys(); tmp; tmp = tmp->next) {
                score = fmax(Score(tmp->begin, k_targ), Score(tmp->end, k_targ));
                if (!best || score > best_score) {
                        best_score = score;
                        best = tmp;
                }
        }
        if (!best || (best->begin == 0 && best->end == histogram_size - 1))
                return k_targ;
        Vector2d k1 = Vector2d::Direction(GetAngle(best->begin));
        Vector2d k2 = Vector2d::Direction(GetAngle(best->end));
        Vector2d k3 = Vector2d::Direction(GetAngle(best->begin +
                                                   max_valley_size));
        Vector2d k4 = Vector2d::Direction(GetAngle(best->end -
                                                   max_valley_size));
        Vector2d v1, v2;
        if (best->size <= max_valley_size) {
                v1 = k1 + k2;
                v1.Normalize();
                return v1;
        }
        v1 = k1 + k3;
        v2 = k2 + k4;
        v1.Normalize();
        v2.Normalize();
        Vector2d bis = Vector2d::Bis(v1, v2);
        if (bis * k_targ >= bis * v1)
                return k_targ;
        return k1 * k_targ >= k2 * k_targ ? v1 : v2;  
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

double Vehicle::Score(int k, const Vector2d& k_targ) const
{
        Vector2d dir = Direction(k);
        return k_targ * dir;
}

Vector2d Vehicle::Direction(int k) const
{
        double step = 2 * PI / histogram_size;
        return Vector2d::Direction(k * step + step / 2.0);
}

void Vehicle::ShowDirection() const
{
        Vector2d a = coord + 21.0 * Vector2d(cos(angle), sin(angle));
        Vector2d b = coord + 12.0 *
                Vector2d(cos(angle + 0.4), sin(angle + 0.4));
        Vector2d c = coord + 12.0 *
                Vector2d(cos(angle - 0.4), sin(angle - 0.4));
        glBegin(GL_TRIANGLES);
        glColor3ub(RED(red), GREEN(red), BLUE(red));
        glVertex2d(a.X(), a.Y());
        glVertex2d(b.X(), b.Y());
        glVertex2d(c.X(), c.Y());
        glEnd();
}

void Vehicle::ShowActiveWindow() const
{
        double offset = double(window_size) / 2.0 * Environment::cell_size;
        glBegin(GL_LINE_LOOP);
        glColor3ub(0, 255, 0);
        glVertex2d(coord.X() - offset, coord.Y() - offset);
        glVertex2d(coord.X() - offset, coord.Y() + offset);
        glVertex2d(coord.X() + offset, coord.Y() + offset);
        glVertex2d(coord.X() + offset, coord.Y() - offset);
        glEnd();
}

void Vehicle::ShowFreeValleys() const
{
        glLineWidth(1.6);
        glBegin(GL_LINES);
        Valley *tmp;
        for (tmp = obstacle_density.GetValleys(); tmp; tmp = tmp->next) {
                for (int k = tmp->begin; k <= tmp->end; k++) {
                        Vector2d dir = Direction(k);
                        Vector2d a = coord + dir * (Radius() + 8.0);
                        Vector2d b = coord + dir * Radius() * 8.0;
                        glColor3ub(RED(green), GREEN(green), BLUE(green));
                        glVertex2d(a.X(), a.Y());
                        glVertex2d(b.X(), b.Y());
                }
        }
        glEnd();
}

Vector2d Vehicle::CurrentDirection() const
{
        return Vector2d::Direction(angle);
}

void Vehicle::Mapping(Environment& map, bool s) const
{
        double x, y;
        double size = Environment::cell_size;
        for (x = -Radius(); x <= Radius(); x += 0.5) {
                for (y = -Radius(); y <= Radius(); y+= 0.5) {
                        if (IsInside(coord + Vector2d(x, y))) {
                                int j = int((coord.X() + x) / size);
                                int i = int((coord.Y() + y) / size);
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
        else if (!strcmp(var, "slave_max_speed"))
                slave_max_speed = atof(value);
        else if (!strcmp(var, "master_max_speed"))
                master_max_speed = atof(value);
        else if (!strcmp(var, "boost_resist"))
                boost_resist = atof(value);
        else if (!strcmp(var, "max_boost"))
                max_boost = atof(value);
        else if (!strcmp(var, "max_angle_speed"))
                max_angle_speed = atof(value);
        else if (!strcmp(var, "max_obstacle_density"))
                max_obstacle_density = atof(value);
        else if (!strcmp(var, "max_distance"))
                max_distance = atof(value);
        else
                fprintf(stderr, "variable %s not found\n", var);
}

double Vehicle::Normal(double ex, double dx)
{
        double z, sum = 0;
        const int n = 12;
        for (int i = 0; i < n; i++)
                sum += Random();
        z = (sum - (double)n / 2.0) / sqrt((double)n / 12.0);
        return z * dx + ex;
}

double Vehicle::Uniform(double a, double b)
{
        return a + (b - a) * Random();
}

double Vehicle::Random()
{
        return (double)rand() / (double)RAND_MAX;
}

