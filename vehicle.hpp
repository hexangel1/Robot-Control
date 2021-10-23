#ifndef VEHICLE_HPP_SENTRY
#define VEHICLE_HPP_SENTRY

#include "vector2d.hpp"
#include "graphobject.hpp"

class Vehicle : public Circle {
        Vector2D speed;
        Vector2D boost;
        Vector2D target;
        bool accident;
        double *active_region;
        double *obstacle_density;
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
        Vehicle(const Vector2D& coord, int colour, const Vector2D& t);
        ~Vehicle();
        void Update(GraphObjectItem *objects, LocalMap& map);
        void ShowInfo() const;
        inline void SetRegion(int i, int j, double val)
                { active_region[i * window_size + j] = val; } 
        inline double GetRegion(int i, int j) const
                { return active_region[i * window_size + j]; } 
        inline Vector2D GetSpeed() const { return speed; }
        inline Vector2D GetBoost() const { return boost; }
        inline Vector2D GetTarget() const { return target; }
        inline bool AccidentHappened() const { return accident; }
private:
        void ReadActiveRegion(LocalMap& map);
        void BlurActiveRegion();
        void PolarHistogram();
        Vector2D Control() const;
        int SteerControl() const;
        double SpeedControl(int k) const;
        double Score(int k) const;
        double MinDensity() const;
        double MidLocalVal(int i, int j) const;
        Vector2D Direction(int k) const;
        Vector2D Goal(int k) const;
        bool IsTargetReached() const;
        bool CheckMove(GraphObjectItem *objects) const;
        void ShowTargetSet() const;
        void ShowActiveWindow() const;
        void ShowFreeDirections() const;
        static int Sector(int i, int j);
        static double Distance(int i, int j);
};

#endif /* VEHICLE_HPP_SENTRY */

