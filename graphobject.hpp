#ifndef GRAPHOBJECT_HPP_SENTRY
#define GRAPHOBJECT_HPP_SENTRY

#include "vector2d.hpp"

#define RED(param)   (((param) & 0xFF0000) >> 0x10)
#define GREEN(param) (((param) & 0x00FF00) >> 0x8)
#define BLUE(param)  (((param) & 0x0000FF))

enum colour_constants {
        black   = 0x000000,
        red     = 0xFF0000,
        green   = 0x00FF00,
        blue    = 0x0000FF,
        yellow  = 0xFFFF00,
        magenta = 0xFF00FF,
        cyan    = 0x00FFFF,
        white   = 0xFFFFFF,
        brown   = 0x8B4513,
        khaki   = 0xBDB76B,
        indigo  = 0x4B0082,
        orange  = 0xFF4500,
        rose    = 0xEE82EE,
        dred    = 0x800000,
        dgreen  = 0x008000,
        dblue   = 0x000080
};

struct GraphObjectItem {
        class GraphObject *elem;
        GraphObjectItem *next;
};

class GraphObject {
protected:
        Vector2d coord;
        int colour;
public:
        GraphObject(const Vector2d& point, int col)
                : coord(point), colour(col) {}
        virtual ~GraphObject() {}
        virtual void Show() const = 0;
        virtual void Hide() const = 0;
        virtual bool IsInside(const Vector2d& v) const = 0;
        void Move(const Vector2d& move);
        inline Vector2d GetXY() const { return coord; }
        inline int Colour() const { return colour; }
};

class Polygon : public GraphObject {
        VertexItem *ptr;
public:
        Polygon(const Vector2d& coord, int colour)
                : GraphObject(coord, colour), ptr(0) {}
        ~Polygon();
        virtual void Show() const;
        virtual void Hide() const;
        virtual bool IsInside(const Vector2d& v) const;
        void AddVertex(double dx, double dy);
        void Rotate(double phi);
};

class Triangle : public Polygon {
public:
        Triangle(const Vector2d& coord, int colour, double a);
        virtual ~Triangle() {}
};

class Rectangle : public Polygon {
public:
        Rectangle(const Vector2d& coord, int colour, double a, double b);
        virtual ~Rectangle() {}
};

class Square : public Rectangle {
public:
        Square(const Vector2d& coord, int colour, double a)
                : Rectangle(coord, colour, a, a) {}
        virtual ~Square() {}
};

class RegularPolygon : public Polygon {
public:
        RegularPolygon(const Vector2d& coord, int colour, double r, int n);
        virtual ~RegularPolygon() {}
};

class Pentagon : public RegularPolygon {
public:
        Pentagon(const Vector2d& coord, int colour, double r)
                : RegularPolygon(coord, colour, r, 5) {}
        virtual ~Pentagon() {}
};

class Hexagon : public RegularPolygon {
public:
        Hexagon(const Vector2d& coord, int colour, double r)
                : RegularPolygon(coord, colour, r, 6) {}
        virtual ~Hexagon() {}
};

class Ellipse : public GraphObject {
        double major_axis;
        double minor_axis;
public:
        Ellipse(const Vector2d& coord, int colour, double a, double b)
                : GraphObject(coord, colour), major_axis(a), minor_axis(b) {}
        virtual ~Ellipse() {}
        virtual void Show() const;
        virtual void Hide() const;
        virtual bool IsInside(const Vector2d& v) const;
        inline double MajorAxis() const { return major_axis; }
        inline double MinorAxis() const { return minor_axis; }
};

class Circle : public Ellipse {
public:
        Circle(const Vector2d& coord, int colour, double radius)
                : Ellipse(coord, colour, radius, radius) {}
        virtual ~Circle() {}
        inline double Radius() const { return MajorAxis(); }
};

#endif /* GRAPHOBJECT_HPP_SENTRY */

