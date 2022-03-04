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
protected:
        static const int window_size;
        static const int histogram_size;
        static const int max_valley_size;
        static const double vehicle_radius;
        static const double speed_constant;
        static const double max_speed;
        static const double max_boost;
        static const double max_angle_speed;
        static const double max_obstacle_density;
public:
        Vehicle(const Vector2d& coord, int colour);
        virtual ~Vehicle() {}
        virtual Vector2d GetTarget() const = 0;
        double GetAngel(int k) const;
        int GetSector(double phi) const;
        void Update(Environment& map, const TargetSet& set);
        void ShowFreeValleys() const;
        void ShowActiveWindow() const;
        void ShowDirection() const;
        void WriteState() const;
        void WriteHistogram() const { obstacle_density.Output(); }
private:
        virtual void ChangeTargets(const TargetSet& set) = 0;
        void Control();
        int SteerControl() const;
        double SpeedControl(int k) const;
        double Score(int k) const;
        Vector2d Direction(int k) const;
        bool CheckMove(const Environment& map) const;
        void Mapping(Environment& map, bool val) const;
};

#endif /* VEHICLE_HPP_SENTRY */

