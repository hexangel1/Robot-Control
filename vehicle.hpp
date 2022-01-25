#ifndef VEHICLE_HPP_SENTRY
#define VEHICLE_HPP_SENTRY

#include "graphobject.hpp"
#include "environment.hpp"
#include "vector2d.hpp"
#include "histogram.hpp"

class Vehicle : public Circle {
        double angle;
        double speed;
        double boost;
        double angular_speed;
        int ctrl;
        Vector2d target;
        Environment active_region;
        Histogram obstacle_density;
        bool accident;
        static const int window_size;
        static const int view_sectors;
        static const int histogram_size;
        static const int target_points;
        static const int blur_factor;
        static const double vehicle_radius;
        static const double speed_constant;
        static const double max_vehicle_speed;
        static const double max_vehicle_boost;
        static const double max_obstacle_density;
        static const double safe_distance;
public:
        Vehicle(const Vector2d& coord, int colour, const Vector2d& t);
        double GetAngel(int k) const;
        int GetSector(double phi) const;
        void Update(Vehicle **robots, Environment& map);
        void ShowInfo() const;
        inline Vector2d GetTarget() const { return target; }
        inline bool AccidentHappened() const { return accident; }
private:
        void BlurActiveRegion();
        void Control();
        int SteerControl() const;
        double SpeedControl(int k) const;
        double Score(int k) const;
        double MinDensity() const;
        double MidLocalVal(int i, int j) const;
        Vector2d Direction(int k) const;
        Vector2d Goal(int k) const;
        bool IsTargetReached() const;
        bool CheckMove(const Environment& map) const;
        void ShowTargetSet() const;
        void ShowActiveWindow() const;
        void ShowFreeDirections() const;
};

#endif /* VEHICLE_HPP_SENTRY */

