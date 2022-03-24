#ifndef VEHICLE_HPP_SENTRY
#define VEHICLE_HPP_SENTRY

#include "graphobject.hpp"
#include "environment.hpp"
#include "vector2d.hpp"
#include "histogram.hpp"
#include "targetset.hpp"

class Vehicle : public Circle {
        double angle;
        double speed;
        double boost;
        double angular_speed;
        bool stopped;
        Environment active_region;
        Histogram obstacle_density;
        Valley *vall;
        static int window_size;
        static int histogram_size;
        static int max_valley_size;
        static double vehicle_radius;
        static double speed_constant;
        static double boost_resist;
        static double max_speed;
        static double max_boost;
        static double max_angle_speed;
        static double max_obstacle_density;
public:
        Vehicle(const Vector2d& coord, int colour);
        virtual ~Vehicle() { FreeValleys(); }
        void Update(Environment& map, const TargetSet& set);
        void Stop() { stopped = true; }
        void ShowDirection() const;
        void ShowFreeValleys() const;
        void ShowActiveWindow() const;
        void WriteLog() const;
        static bool ReadConfig(const char *file);
private:
        void Control();
        void FreeValleys();
        int SteerControl() const;
        double SpeedControl(int k) const;
        double GetAngel(int k) const;
        int GetSector(double phi) const;
        Vector2d Direction(int k) const; 
        double Score(int k) const;
        void Mapping(Environment& map, bool val) const;
        virtual void ChangeTargets(const TargetSet& set) = 0;
        virtual Vector2d GetTarget() const = 0;
        static void SetValue(const char *var, const char *value);
};

#endif /* VEHICLE_HPP_SENTRY */

