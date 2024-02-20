#ifndef VEHICLE_HPP_SENTRY
#define VEHICLE_HPP_SENTRY

#include "graphobject.hpp"
#include "environment.hpp"
#include "vector2d.hpp"
#include "histogram.hpp"
#include "array.hpp"

class Vehicle : public Circle {
        double angle;
        double speed;
        double boost;
        double angular_speed;
        bool stopped;
        double max_speed;
        Environment active_region;
        Histogram obstacle_density;
public:
        static int window_size;
        static int histogram_size;
        static int max_valley_size;
        static double vehicle_radius;
        static double boost_resist;
        static double master_max_speed;
        static double slave_max_speed;
        static double max_boost;
        static double max_angle_speed;
        static double max_obstacle_density;
        static double max_distance;
public:
        Vehicle(const Vector2d& coord, double speedmax, int colour);
        ~Vehicle() {}
        void Update(Environment& map, const Vector2d& k_targ);
        void Respawn(Environment& map, const Vector2d& spawn);
        void Stop() { stopped = true; }
        void ShowDirection() const;
        void ShowFreeValleys() const;
        void ShowActiveWindow() const;
        static bool ReadConfig(const char *file);
private:
        void Control(const Vector2d& k_targ);
        Vector2d SteerControl(const Vector2d& k_targ) const;
        double SpeedControl(int k) const;
        double GetAngle(int k) const;
        int GetSector(double phi) const;
        Vector2d Direction(int k) const;
        double Score(int k, const Vector2d& k_targ) const;
        void Mapping(Environment& map, bool val) const;
        static void SetValue(const char *var, const char *value);
        static double Normal(double ex, double dx);
        static double Uniform(double a, double b);
        static double Random();
};

#endif /* VEHICLE_HPP_SENTRY */

