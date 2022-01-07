#include <cmath>
#include <GLFW/glfw3.h>
#include "vehicle.hpp"

const int Vehicle::window_size = 43;
const int Vehicle::view_sectors = 8;
const int Vehicle::histogram_size = 60;
const int Vehicle::target_points = 30;
const int Vehicle::smooth_factor = 3;
const int Vehicle::blur_factor = 2;
const double Vehicle::vehicle_radius = 8.0;
const double Vehicle::speed_constant = 0.015;
const double Vehicle::max_cell_weight = 100.0;
const double Vehicle::max_vehicle_speed = 40.0;
const double Vehicle::max_vehicle_boost = 1.0;
const double Vehicle::max_obstacle_density = 100.0;
const double Vehicle::safe_distance = 80.0;

Vehicle::Vehicle(const Vector2d& coord, int colour, const Vector2d& t)
        : Circle(coord, colour, vehicle_radius),
        angle(0.0), speed(0.0), boost(0.0),
        angular_speed(0.0), target(t),
        active_region(window_size, window_size)
{
        obstacle_density = new double[histogram_size];
        for (int k = 0; k < histogram_size; k++)
                obstacle_density[k] = 0.0;
        accident = false;
}

Vehicle::~Vehicle()
{
        delete[] obstacle_density;
}

void Vehicle::Update(Vehicle **robots, Environment& map)
{
        if (accident)
                return;
        Mapping(map, false);
        ReadActiveRegion(map);
        BlurActiveRegion();
        PolarHistogram();
        coord += speed_constant * speed * Vector2d(cos(angle), -sin(angle));
        speed += boost;
        angle += angular_speed;
        Control();
        speed = MIN(speed, max_vehicle_speed);
        speed = MAX(speed, 0.0);
        while (angle > 2 * PI)
                angle -= 2 * PI;
        while (angle < 0)
                angle += 2 * PI;
        if (!CheckMove(map))
                accident = true;
        Mapping(map, true);
        (void)robots;
}

void Vehicle::ShowInfo() const
{
        ShowTargetSet();
        ShowActiveWindow();
        ShowFreeDirections();
}

void Vehicle::ReadActiveRegion(Environment& map)
{
        int i, j, offsetx, offsety;
        offsety = (int)coord.Y() / Environment::cell_size - window_size / 2;
        offsetx = (int)coord.X() / Environment::cell_size - window_size / 2;
        for (i = 0; i < window_size; i++) {
                for (j = 0; j < window_size; j++) {
                        double v = map.Get(offsety + i, offsetx + j);
                        active_region.Set(i, j, v);
                }
        }
}

void Vehicle::BlurActiveRegion()
{
        int i, j, k;
        double tmp[window_size][window_size];
        for (k = 0; k < blur_factor; k++) {
                for (i = 0; i < window_size; i++) {
                        for (j = 0; j < window_size; j++) {
                                double v = active_region.Get(i, j);
                                tmp[i][j] = v > 0.0 ? v : MidLocalVal(i, j);
                        }
                }
                for (i = 0; i < window_size; i++) {
                        for (j = 0; j < window_size; j++)
                                active_region.Set(i, j, tmp[i][j]);
                }
        }
}

void Vehicle::PolarHistogram()
{
        double a = max_cell_weight;
        double b = a / (((double)window_size - 1.0) * sqrt(2.0) / 2.0);
        double h[histogram_size];
        for (int k = 0; k < histogram_size; k++)
                h[k] = 0.0;
        for (int i = 0; i < window_size; i++) {
                for (int j = 0; j < window_size; j++) {
                        int k = Sector(i, j);
                        double d = Distance(i, j);
                        h[k] += pow(active_region.Get(i, j), 2) * (a - b * d);
                }
        }
        for (int k = 0; k < histogram_size; k++) {
                double sum = 0.0;
                obstacle_density[k] = 0.0;
                for (int i = -smooth_factor; i <= smooth_factor; i++) {
                        int r = histogram_size;
                        double weight = smooth_factor + 1 - abs(i);
                        obstacle_density[k] += weight * h[(r + k - i) % r];
                        sum += weight;
                }
                obstacle_density[k] /= sum;
        }
}

double sat(double val, double a, double b)
{
        if (val >= a && val <= b)
                return val;
        return val > b ? b : a;
}

void Vehicle::Control()
{
        if (IsTargetReached()) {
                speed = 0;
                angular_speed = 0;
                return;
        }
        int k = SteerControl();
        boost = SpeedControl(k) - speed;
        boost = sat(boost, -2.0, 2.0);
        angular_speed = GetAngel(k) - angle;
        angular_speed = sat(angular_speed, -0.02, 0.02);
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
        double min, score, best_score;
        int k, best = -1;
        min = MinDensity();
        int s = GetSector(angle);
        for (k = s - view_sectors; k <= s + view_sectors; k++) {
                if (obstacle_density[(histogram_size + k) % histogram_size] 
                                == min) {
                        score = Score(k);
                        if (best == -1 || score > best_score) {
                                best_score = score;
                                best = k;
                        }
                }
        }
        return best;
}

double Vehicle::SpeedControl(int k) const
{
        k = (k + histogram_size) % histogram_size;
        double h = MIN(obstacle_density[k], max_obstacle_density);
        return max_vehicle_speed * (1.0 - h / max_obstacle_density);
}

double Vehicle::Score(int k) const
{
        Vector2d dir = Direction(k);
        double score, best_score = -1.0;
        for (int i = 0; i < target_points; i++) {
                Vector2d goal = Goal(i) - coord;
                goal.Normalize();
                score = dir * goal;
                if (score > best_score)
                        best_score = score;
        }
        return best_score;
}

double Vehicle::MinDensity() const
{
        int k;
        int s = GetSector(angle);
        double min = obstacle_density[(histogram_size + s - view_sectors)
                                         % histogram_size];
        for (k = s - view_sectors + 1; k <= s + view_sectors; k++) {
                if (obstacle_density[(histogram_size + k) % histogram_size] 
                                        < min)
                        min = obstacle_density[(histogram_size + k)
                                                 % histogram_size];
        }
        return min;
}

double Vehicle::MidLocalVal(int i, int j) const
{
        double sum = 0.0;
        int amount = 0;
        for (int k = -1; k <= 1; k++) {
                for (int l = -1; l <= 1; l++) {
                        if (i + k < 0 || i + k >= window_size ||
                            j + l < 0 || j + l >= window_size)
                                continue;
                        if (active_region.Get(i + k, j + l) != 0.0) {
                                sum += active_region.Get(i + k, j + l);
                                amount++;
                        }
                }
        }
        return amount > 0 ? sum / (double)amount : 0.0;
}

Vector2d Vehicle::Direction(int k) const
{
        const double step = 360.0 / histogram_size;
        Vector2d dir(1.0, 0.0);
        dir.Rotate(DEG2RAD(-k * step - step / 2.0));
        return dir;
}

Vector2d Vehicle::Goal(int k) const
{
        const double step = 2 * PI / target_points;
        Vector2d dir(1.0, 0.0);
        dir.Rotate(-k * step);
        return target + safe_distance * dir;
}

bool Vehicle::IsTargetReached() const
{
        return abs((target - coord).Module() - safe_distance) <= Radius();
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

void Vehicle::ShowTargetSet() const
{
        glBegin(GL_LINE_LOOP);
        glColor3ub(0, 255, 0);
        for (int i = 0; i < 100; i++) {
                double t = 2.0 * PI * (double)i / 100.0;
                Vector2d p = target + safe_distance * Vector2d(cos(t), sin(t));
                glVertex2f(p.X(), p.Y());
        }
        glEnd();
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

void Vehicle::ShowFreeDirections() const
{
        int k;
        double min = MinDensity();
        glBegin(GL_LINES);
        glColor3ub(0, 255, 0);
        int s = GetSector(angle);
        for (k = s - view_sectors; k <= s + view_sectors; k++) {
                if (obstacle_density[(histogram_size + k) 
                                % histogram_size] == min) {
                        Vector2d dir = Direction(k);
                        Vector2d a = coord + dir * (Radius() + 8.0);
                        Vector2d b = coord + dir * Radius() * 10.0;
                        glVertex2f(a.X(), a.Y());
                        glVertex2f(b.X(), b.Y());
                }
        }
        glColor3ub(255, 0, 0);
        Vector2d dir = Direction(s);
        Vector2d a = coord + dir * (Radius() + 8.0);
        Vector2d b = coord + dir * Radius() * 10.0;
        glVertex2f(a.X(), a.Y());
        glVertex2f(b.X(), b.Y());
        glEnd();
}

int Vehicle::Sector(int i, int j)
{
        double alpha, phi;
        alpha = 360.0 / histogram_size;
        phi = atan2(window_size / 2 - i, j - window_size / 2);
        if (phi < 0.0)
                phi += 2.0 * PI;
        return (int)trunc(RAD2DEG(phi) / alpha);
}

double Vehicle::Distance(int i, int j)
{
        return sqrt(pow(i - window_size / 2, 2) + pow(j - window_size / 2, 2));
}

