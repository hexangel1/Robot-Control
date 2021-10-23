#ifndef VECTOR2D_HPP_SENTRY
#define VECTOR2D_HPP_SENTRY

#define PI 3.1415926535
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define RAD2DEG(param) ((param) * 180.0 / 3.1415926535)
#define DEG2RAD(param) ((param) * 3.1415926535 / 180.0)

class Vector2D {
        double x;
        double y;
public:
        Vector2D(double a_x, double a_y) : x(a_x), y(a_y) {}
        Vector2D(const Vector2D& v) : x(v.x), y(v.y) {}
        Vector2D() : x(0.0), y(0.0) {}
        ~Vector2D() {}
        void Normalize();
        void Rotate(double phi);
        void Saturation(double min, double max);
        void Print() const;
        double Norm(int p) const;
        double Module() const;
        inline double X() const { return x; }
        inline double Y() const { return y; } 
        const Vector2D& operator=(const Vector2D& v);
        const Vector2D& operator+=(const Vector2D& v);
        const Vector2D& operator-=(const Vector2D& v);
        const Vector2D& operator*=(double a);
        const Vector2D& operator/=(double a);
        friend Vector2D operator+(const Vector2D& v);
        friend Vector2D operator-(const Vector2D& v);
        friend Vector2D operator+(const Vector2D& v1, const Vector2D& v2);
        friend Vector2D operator-(const Vector2D& v1, const Vector2D& v2);
        friend Vector2D operator*(const Vector2D& v, double a);
        friend Vector2D operator*(double a, const Vector2D& v);
        friend Vector2D operator/(const Vector2D& v, double a);
        friend double operator*(const Vector2D& v1, const Vector2D& v2);
        friend bool operator==(const Vector2D& v1, const Vector2D& v2);
        friend bool operator!=(const Vector2D& v1, const Vector2D& v2);
        friend bool operator>=(const Vector2D& v1, const Vector2D& v2);
        friend bool operator<=(const Vector2D& v1, const Vector2D& v2);
        friend bool operator>(const Vector2D& v1, const Vector2D& v2);
        friend bool operator<(const Vector2D& v1, const Vector2D& v2);
};

#endif /* VECTOR2D_HPP_SENTRY */

