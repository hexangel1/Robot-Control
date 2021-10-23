#include <cstdio>
#include <cmath>
#include "vector2d.hpp"

void Vector2D::Normalize()
{
        double module = Module();
        if (module > 0.0) {
                x /= module;
                y /= module;
        }
}

void Vector2D::Rotate(double phi)
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

void Vector2D::Saturation(double min, double max)
{
        double len = Module();
        if (len > max || len < min) {
                double coef = len > max ? max : min; 
                Normalize();
                x *= coef;
                y *= coef;
        }
}

double Vector2D::Norm(int p) const
{
        if (p == 1)
                return abs(x) + abs(y);
        if (p == 2)
                return sqrt(x * x + y * y);
        if (p == 3)
                return MAX(abs(x), abs(y));
        return -1.0;
}

double Vector2D::Module() const
{
        return Norm(2);
}

void Vector2D::Print() const
{
        fprintf(stderr, "[%f, %f]\n", x, y);
}

const Vector2D& Vector2D::operator=(const Vector2D& v)
{
        x = v.x;
        y = v.y;
        return *this;
}

const Vector2D& Vector2D::operator+=(const Vector2D& v)
{
        x += v.x;
        y += v.y;
        return *this;
}

const Vector2D& Vector2D::operator-=(const Vector2D& v)
{
        x -= v.x;
        y -= v.y;
        return *this;
}

const Vector2D& Vector2D::operator*=(double a)
{
        x *= a;
        y *= a;
        return *this;
}

const Vector2D& Vector2D::operator/=(double a)
{
        x /= a;
        y /= a;
        return *this;
}

Vector2D operator+(const Vector2D& v)
{
        return Vector2D(v.x, v.y);
}

Vector2D operator-(const Vector2D& v)
{
        return Vector2D(-v.x, -v.y);
}

Vector2D operator+(const Vector2D& v1, const Vector2D& v2)
{
        return Vector2D(v1.x + v2.x, v1.y + v2.y);
}

Vector2D operator-(const Vector2D& v1, const Vector2D& v2)
{
        return Vector2D(v1.x - v2.x, v1.y - v2.y);
}

Vector2D operator*(const Vector2D& v, double a)
{
        return Vector2D(v.x * a, v.y * a);
}

Vector2D operator*(double a, const Vector2D& v)
{
        return Vector2D(v.x * a, v.y * a);
}

Vector2D operator/(const Vector2D& v, double a)
{
        return Vector2D(v.x / a, v.y / a);
}

double operator*(const Vector2D& v1, const Vector2D& v2)
{
        return v1.x * v2.x + v1.y * v2.y;
}

bool operator==(const Vector2D& v1, const Vector2D& v2)
{
        return v1.x == v2.x && v1.y == v2.y;
}

bool operator!=(const Vector2D& v1, const Vector2D& v2)
{
        return v1.x != v2.x || v1.y != v2.y;
}

bool operator>=(const Vector2D& v1, const Vector2D& v2)
{
        return v1.x >= v2.x && v1.y >= v2.y;
}

bool operator<=(const Vector2D& v1, const Vector2D& v2)
{
        return v1.x <= v2.x && v1.y <= v2.y;
}

bool operator>(const Vector2D& v1, const Vector2D& v2)
{
        return v1.x > v2.x && v1.y > v2.y;
}

bool operator<(const Vector2D& v1, const Vector2D& v2)
{
        return v1.x < v2.x && v1.y < v2.y;
}

