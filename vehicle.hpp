#ifndef VEHICLE_HPP_SENTRY
#define VEHICLE_HPP_SENTRY

#include "graphobject.hpp"
#include "environment.hpp"
#include "vector2d.hpp"

class Vehicle : public Circle {
        Vector2d speed;
        Vector2d boost;
        Vector2d target;
        Environment active_region;
        double *obstacle_density;
        bool accident;
        static const int window_size;
        static const int histogram_size;
        static const int target_points;
        static const int smooth_factor;
        static const int blur_factor;
        static const double vehicle_radius;
        static const double speed_constant;
        static const double max_cell_weight;
        static const double max_vehicle_speed;
        static const double max_vehicle_boost;
        static const double max_obstacle_density;
        static const double safe_distance;
public:
        Vehicle(const Vector2d& coord, int colour, const Vector2d& t);
        ~Vehicle();
        void Update(Vehicle **robots, Environment& map);
        void ShowInfo() const;
        inline Vector2d GetSpeed() const { return speed; }
        inline Vector2d GetBoost() const { return boost; }
        inline Vector2d GetTarget() const { return target; }
        inline bool AccidentHappened() const { return accident; }
private:
        void ReadActiveRegion(Environment& map);
        void BlurActiveRegion();
        void PolarHistogram();
        Vector2d Control() const;
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
        static int Sector(int i, int j);
        static double Distance(int i, int j);
};

#endif /* VEHICLE_HPP_SENTRY */

