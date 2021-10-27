#include <cstdio>
#include <cmath>
#include "vector2d.hpp"

void Vector2d::Normalize()
{
        double module = Module();
        if (module > 0.0) {
                x /= module;
                y /= module;
        }
}

void Vector2d::Rotate(double phi)
{
        const double SO2[2][2] = {
                { cos(phi), -sin(phi) },
                { sin(phi),  cos(phi) }
        };
        double new_x, new_y;
        new_x = SO2[0][0] * x + SO2[0][1] * y;
        new_y = SO2[1][0] * x + SO2[1][1] * y;
        x = new_x;
        y = new_y;
}

void Vector2d::Saturation(double min, double max)
{
        double len = Module();
        if (len > max || len < min) {
                double coef = len > max ? max : min;
                Normalize();
                x *= coef;
                y *= coef;
        }
}

double Vector2d::Norm(int p) const
{
        if (p == 1)
                return abs(x) + abs(y);
        if (p == 2)
                return sqrt(x * x + y * y);
        if (p == 3)
                return MAX(abs(x), abs(y));
        return -1.0;
}

double Vector2d::Module() const
{
        return Norm(2);
}

void Vector2d::Print() const
{
        fprintf(stderr, "[%f, %f]\n", x, y);
}

const Vector2d& Vector2d::operator=(const Vector2d& v)
{
        x = v.x;
        y = v.y;
        return *this;
}

const Vector2d& Vector2d::operator+=(const Vector2d& v)
{
        x += v.x;
        y += v.y;
        return *this;
}

const Vector2d& Vector2d::operator-=(const Vector2d& v)
{
        x -= v.x;
        y -= v.y;
        return *this;
}

const Vector2d& Vector2d::operator*=(double a)
{
        x *= a;
        y *= a;
        return *this;
}

const Vector2d& Vector2d::operator/=(double a)
{
        x /= a;
        y /= a;
        return *this;
}

Vector2d operator+(const Vector2d& v)
{
        return Vector2d(v.x, v.y);
}

Vector2d operator-(const Vector2d& v)
{
        return Vector2d(-v.x, -v.y);
}

Vector2d operator+(const Vector2d& v1, const Vector2d& v2)
{
        return Vector2d(v1.x + v2.x, v1.y + v2.y);
}

Vector2d operator-(const Vector2d& v1, const Vector2d& v2)
{
        return Vector2d(v1.x - v2.x, v1.y - v2.y);
}

Vector2d operator*(const Vector2d& v, double a)
{
        return Vector2d(v.x * a, v.y * a);
}

Vector2d operator*(double a, const Vector2d& v)
{
        return Vector2d(v.x * a, v.y * a);
}

Vector2d operator/(const Vector2d& v, double a)
{
        return Vector2d(v.x / a, v.y / a);
}

double operator*(const Vector2d& v1, const Vector2d& v2)
{
        return v1.x * v2.x + v1.y * v2.y;
}

bool operator==(const Vector2d& v1, const Vector2d& v2)
{
        return v1.x == v2.x && v1.y == v2.y;
}

bool operator!=(const Vector2d& v1, const Vector2d& v2)
{
        return v1.x != v2.x || v1.y != v2.y;
}

bool operator>=(const Vector2d& v1, const Vector2d& v2)
{
        return v1.x >= v2.x && v1.y >= v2.y;
}

bool operator<=(const Vector2d& v1, const Vector2d& v2)
{
        return v1.x <= v2.x && v1.y <= v2.y;
}

bool operator>(const Vector2d& v1, const Vector2d& v2)
{
        return v1.x > v2.x && v1.y > v2.y;
}

bool operator<(const Vector2d& v1, const Vector2d& v2)
{
        return v1.x < v2.x && v1.y < v2.y;
}

