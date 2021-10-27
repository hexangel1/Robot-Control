#ifndef VECTOR2D_HPP_SENTRY
#define VECTOR2D_HPP_SENTRY

#define PI 3.1415926535
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define RAD2DEG(param) ((param) * 180.0 / 3.1415926535)
#define DEG2RAD(param) ((param) * 3.1415926535 / 180.0)

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
        double Module() const;
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
        friend bool operator==(const Vector2d& v1, const Vector2d& v2);
        friend bool operator!=(const Vector2d& v1, const Vector2d& v2);
        friend bool operator>=(const Vector2d& v1, const Vector2d& v2);
        friend bool operator<=(const Vector2d& v1, const Vector2d& v2);
        friend bool operator>(const Vector2d& v1, const Vector2d& v2);
        friend bool operator<(const Vector2d& v1, const Vector2d& v2);
};

#endif /* VECTOR2D_HPP_SENTRY */

