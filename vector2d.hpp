#ifndef VECTOR2D_HPP_SENTRY
#define VECTOR2D_HPP_SENTRY

#include <cmath>
#define PI 3.1415926535
#define SAT(v, a, b) ((v) >= (a) && (v) <= (b) ? (v) : ((v) > (b) ? (b) : (a)))
#define SGN(a) ((a) >= 0.0 ? 1.0 : -1.0)
#define ANGLE_2PI(v) ((v) < 0.0 ? (v) + 2 * PI : (v))
#define RAD2DEG(param) ((param) * 180.0 / PI)
#define DEG2RAD(param) ((param) * PI / 180.0)

class Vector2d {
        double x;
        double y;
public:
        Vector2d(double a_x, double a_y) : x(a_x), y(a_y) {}
        Vector2d(const Vector2d& v) : x(v.x), y(v.y) {}
        Vector2d() : x(0.0), y(0.0) {}
        ~Vector2d() {}
        void Normalize();
        void Rotate(double phi);
        void Saturation(double min, double max);
        void Print() const;
        double Norm(int p) const;
        double Module() const { return sqrt(x * x + y * y); }
        double Argument() const { return ANGLE_2PI(atan2(y, x)); }
        inline double X() const { return x; }
        inline double Y() const { return y; }
        const Vector2d& operator=(const Vector2d& v);
        const Vector2d& operator+=(const Vector2d& v);
        const Vector2d& operator-=(const Vector2d& v);
        const Vector2d& operator*=(double a);
        const Vector2d& operator/=(double a);
        friend Vector2d operator+(const Vector2d& v);
        friend Vector2d operator-(const Vector2d& v);
        friend Vector2d operator+(const Vector2d& v1, const Vector2d& v2);
        friend Vector2d operator-(const Vector2d& v1, const Vector2d& v2);
        friend Vector2d operator*(const Vector2d& v, double a);
        friend Vector2d operator*(double a, const Vector2d& v);
        friend Vector2d operator/(const Vector2d& v, double a);
        friend double operator*(const Vector2d& v1, const Vector2d& v2);
        friend double operator^(const Vector2d& v1, const Vector2d& v2);
        friend bool operator==(const Vector2d& v1, const Vector2d& v2);
        friend bool operator!=(const Vector2d& v1, const Vector2d& v2);
        friend bool operator>=(const Vector2d& v1, const Vector2d& v2);
        friend bool operator<=(const Vector2d& v1, const Vector2d& v2);
        friend bool operator>(const Vector2d& v1, const Vector2d& v2);
        friend bool operator<(const Vector2d& v1, const Vector2d& v2);
        static Vector2d Direction(double phi);
        static Vector2d Bis(const Vector2d& v1, const Vector2d& v2);
};

struct VertexItem {
        Vector2d vec;
        VertexItem *next;
};

#endif /* VECTOR2D_HPP_SENTRY */

