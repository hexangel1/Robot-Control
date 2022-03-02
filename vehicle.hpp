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
        int ctrl;
        Environment active_region;
        Histogram obstacle_density;
        Valley *vall;
        bool accident;
        static const int window_size;
        static const int histogram_size;
        static const double vehicle_radius;
        static const double speed_constant;
        static const double max_speed;
        static const double max_boost;
        static const double max_angle_speed;
        static const double max_obstacle_density;
        static const double safe_distance;
public:
        Vehicle(const Vector2d& coord, int colour);
        double GetAngel(int k) const;
        int GetSector(double phi) const;
        void Update(Environment& map, const TargetSet& set);
        void ShowInfo() const;
        void WriteHistogram() const { obstacle_density.Output(); }
        void WriteState() const;
        virtual Vector2d GetTarget() const = 0;
        inline bool AccidentHappened() const { return accident; }
private:
        virtual void ChangeTargets(const TargetSet& set) = 0;
        void Control();
        int SteerControl() const;
        double SpeedControl(int k) const;
        double Score(int k) const;
        Vector2d Direction(int k) const;
        bool IsTargetReached() const;
        bool CheckMove(const Environment& map) const;
        void ShowFreeValleys() const;
        void ShowActiveWindow() const;
};

#endif /* VEHICLE_HPP_SENTRY */

