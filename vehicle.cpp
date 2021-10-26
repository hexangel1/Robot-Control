#include <cmath>
#include <GLFW/glfw3.h>
#include "vehicle.hpp"

const int Vehicle::window_size = 33;
const int Vehicle::histogram_size = 60;
const int Vehicle::target_points = 60;
const int Vehicle::smooth_factor = 3;
const int Vehicle::blur_factor = 2;
const double Vehicle::vehicle_radius = 8.0;
const double Vehicle::speed_constant = 0.015;
const double Vehicle::max_cell_weight = 100.0;
const double Vehicle::max_vehicle_speed = 45.0;
const double Vehicle::max_vehicle_boost = 1.0;
const double Vehicle::max_obstacle_density = 100.0;
const double Vehicle::safe_distance = 80.0;

Vehicle::Vehicle(const Vector2D& coord, int colour, const Vector2D& t)
        : Circle(coord, colour, vehicle_radius),
        speed(0.0, 0.0), boost(0.0, 0.0), target(t)
{
        accident = false;
        active_region = new double[window_size * window_size];
        obstacle_density = new double[histogram_size];
        for (int k = 0; k < histogram_size; k++)
                obstacle_density[k] = 0.0;
}

Vehicle::~Vehicle()
{
        delete[] active_region;
        delete[] obstacle_density;
}

void Vehicle::Update(GraphObjectItem *objects, LocalMap& map)
{
        if (accident)
                return;
        Mapping(map, false);
        ReadActiveRegion(map);
        BlurActiveRegion();
        PolarHistogram();
        coord += speed_constant * speed;
        speed += boost;
        boost = Control();
        speed.Saturation(0.0, max_vehicle_speed);
        boost.Saturation(0.0, max_vehicle_boost);
        Mapping(map, true);
        if (!CheckMove(objects))
                accident = true;
}

void Vehicle::ShowInfo() const
{
        ShowTargetSet();
        ShowActiveWindow();
        ShowFreeDirections();
}

void Vehicle::ReadActiveRegion(LocalMap& map)
{
        int i, j, offsetx, offsety;
        offsety = (int)coord.Y() / LocalMap::cell_size - window_size / 2;
        offsetx = (int)coord.X() / LocalMap::cell_size - window_size / 2;
        for (i = 0; i < window_size; i++) {
                for (j = 0; j < window_size; j++)
                        SetRegion(i, j, map.Get(offsety + i, offsetx + j));
        }
}

void Vehicle::BlurActiveRegion()
{
        int i, j, k;
        double tmp[window_size][window_size];
        for (k = 0; k < blur_factor; k++) {
                for (i = 0; i < window_size; i++) {
                        for (j = 0; j < window_size; j++) {
                                double v = GetRegion(i, j);
                                tmp[i][j] = v > 0.0 ? v : MidLocalVal(i, j);
                        }
                }
                for (i = 0; i < window_size; i++) {
                        for (j = 0; j < window_size; j++)
                                SetRegion(i, j, tmp[i][j]);
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
                        h[k] += pow(GetRegion(i, j), 2) * (a - b * d);
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

Vector2D Vehicle::Control() const
{
        int k = SteerControl();
        double s = SpeedControl(k);
        return IsTargetReached() ? -speed : Direction(k) * s - speed;
}

int Vehicle::SteerControl() const
{
        double min, score, best_score;
        int k, best = -1;
        min = MinDensity();
        for (k = 0; k < histogram_size; k++) {
                if (obstacle_density[k] == min) {
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
        double h = MIN(obstacle_density[k], max_obstacle_density);
        return max_vehicle_speed * (1.0 - h / max_obstacle_density);
}

double Vehicle::Score(int k) const
{
        Vector2D dir = Direction(k);
        double score, best_score = -1.0;
        for (int i = 0; i < target_points; i++) {
                Vector2D goal = Goal(i) - coord;
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
        double min = obstacle_density[0];
        for (k = 1; k < histogram_size; k++) {
                if (obstacle_density[k] < min)
                        min = obstacle_density[k];
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
                        if (GetRegion(i + k, j + l) != 0.0) {
                                sum += GetRegion(i + k, j + l);
                                amount++;
                        }
                }
        }
        return amount > 0 ? sum / (double)amount : 0.0;
}

Vector2D Vehicle::Direction(int k) const
{
        const double step = 360.0 / histogram_size;
        Vector2D dir(1.0, 0.0);
        dir.Rotate(DEG2RAD(-k * step - step / 2.0));
        return dir;
}

Vector2D Vehicle::Goal(int k) const
{
        const double step = 2 * PI / target_points;
        Vector2D dir(1.0, 0.0);
        dir.Rotate(-k * step);
        return target + safe_distance * dir;
}

bool Vehicle::IsTargetReached() const
{
        return abs((target - coord).Module() - safe_distance) <= Radius();
}

bool Vehicle::CheckMove(GraphObjectItem *objects) const
{
        GraphObjectItem *tmp;
        Vector2D ray(Radius(), 0.0);
        for (int i = 0; i < 60; i++) {
                for (tmp = objects; tmp; tmp = tmp->next) {
                        if (tmp->elem != this &&
                            tmp->elem->IsInside(coord + ray))
                                return false;
                }
                ray.Rotate(DEG2RAD(6.0));
        }
        return true;
}

void Vehicle::ShowTargetSet() const
{
        glBegin(GL_LINE_LOOP);
        glColor3ub(0, 255, 0);
        for (int i = 0; i < 100; i++) {
                double t = 2.0 * PI * (double)i / 100.0;
                Vector2D p = target + safe_distance * Vector2D(cos(t), sin(t));
                glVertex2f(p.X(), p.Y());
        }
        glEnd();
}

void Vehicle::ShowActiveWindow() const
{
        double offset = window_size / 2 * LocalMap::cell_size;
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
        for (k = 0; k < histogram_size; k++) {
                if (obstacle_density[k] == min) {
                        Vector2D dir = Direction(k);
                        Vector2D a = coord + dir * (Radius() + 5.0);
                        Vector2D b = coord + dir * Radius() * 10.0;
                        glVertex2f(a.X(), a.Y());
                        glVertex2f(b.X(), b.Y());
                }
        }
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

