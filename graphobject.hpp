#ifndef GRAPHOBJECT_HPP_SENTRY
#define GRAPHOBJECT_HPP_SENTRY

#include "vector2d.hpp"
#include "localmap.hpp"

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
        Vector2D coord;
        int colour;
public:
        GraphObject(const Vector2D& point, int col)
                : coord(point), colour(col) {}
        virtual ~GraphObject() {}
        virtual void Show() const = 0;
        virtual void Hide() const = 0;
        virtual bool IsInside(const Vector2D& point) const = 0;
        virtual void Mapping(LocalMap& map, bool s) const = 0;
        void Move(const Vector2D& move);
        inline Vector2D GetXY() const { return coord; }
        inline int Colour() const { return colour; }
};

class Ellipse : public GraphObject {
        double major_axis;
        double minor_axis;
public:
        Ellipse(const Vector2D& coord, int colour, double a, double b)
                : GraphObject(coord, colour), major_axis(a), minor_axis(b) {}
        virtual ~Ellipse() {}
        virtual void Show() const;
        virtual void Hide() const;
        virtual bool IsInside(const Vector2D& point) const;
        virtual void Mapping(LocalMap& map, bool s) const;
        inline double MajorAxis() const { return major_axis; }
        inline double MinorAxis() const { return minor_axis; }
};

class Rectangle : public GraphObject {
        double width;
        double height;
public:
        Rectangle(const Vector2D& coord, int colour, double a, double b)
                : GraphObject(coord, colour), width(a), height(b) {}
        virtual ~Rectangle() {}
        virtual void Show() const;
        virtual void Hide() const;
        virtual bool IsInside(const Vector2D& point) const;
        virtual void Mapping(LocalMap& map, bool s) const;
        inline double Width() const { return width; }
        inline double Height() const { return height; }
};

class Triangle : public GraphObject {
        Vector2D va;
        Vector2D vb;
        Vector2D vc;
public:
        Triangle(const Vector2D& coord, int colour, double a)
                : GraphObject(coord, colour),
                va(Vector2D(0.0, -a / 1.7320508)), 
                vb(Vector2D(+a / 2.0, a / 3.4641016)), 
                vc(Vector2D(-a / 2.0, a / 3.4641016)) {}
        Triangle(const Vector2D& a, const Vector2D& b, const Vector2D& c,
                 int colour)
                : GraphObject((a + b + c) / 3, colour), 
                va(a - coord), vb(b - coord), vc(c - coord) {}
        virtual ~Triangle() {}
        virtual void Show() const;
        virtual void Hide() const;
        virtual bool IsInside(const Vector2D& point) const;
        virtual void Mapping(LocalMap& map, bool s) const;
        inline double LengthAB() const { return (va - vb).Module(); }
        inline double LengthBC() const { return (vb - vc).Module(); }
        inline double LengthAC() const { return (va - vc).Module(); }
};

class Circle : public Ellipse {
public:
        Circle(const Vector2D& coord, int colour, double radius)
                : Ellipse(coord, colour, radius, radius) {}
        virtual ~Circle() {}
        inline double Radius() const { return MajorAxis(); }
};

class Square : public Rectangle {
public:
        Square(const Vector2D& coord, int colour, double size)
                : Rectangle(coord, colour, size, size) {}
        virtual ~Square() {}
        inline double Size() const { return Width(); }
};

#endif /* GRAPHOBJECT_HPP_SENTRY */

