#include <cmath>
#include <GLFW/glfw3.h>
#include "graphobject.hpp"


void GraphObject::Move(const Vector2d& move)
{
        Hide();
        coord = move;
        Show();
}

Polygon::~Polygon()
{
        VertexItem *tmp;
        while (ptr) {
                tmp = ptr;
                ptr = ptr->next;
                delete tmp;
        }
}

void Polygon::Show() const
{
        VertexItem *tmp;
        glBegin(GL_POLYGON);
        glColor3ub(RED(colour), GREEN(colour), BLUE(colour));
        for (tmp = ptr; tmp; tmp = tmp->next) {
                Vector2d v = coord + tmp->vec;
                glVertex2d(v.X(), v.Y());
        }
        glEnd();
}

void Polygon::Hide() const
{
        VertexItem *tmp;
        glBegin(GL_POLYGON);
        glColor3ub(0, 0, 0);
        for (tmp = ptr; tmp; tmp = tmp->next) {
                Vector2d v = coord + tmp->vec;
                glVertex2d(v.X(), v.Y());
        }
        glEnd();
}

bool Polygon::IsInside(const Vector2d& v) const
{
        VertexItem *tmp;
        Vector2d A, B;
        for (tmp = ptr; tmp->next; tmp = tmp->next) {
                A = coord + tmp->vec;
                B = coord + tmp->next->vec;
                if (((B-A)^(v-A)) < 0.0)
                        return false;
        }
        A = coord + tmp->vec;
        B = coord + ptr->vec;
        if (((B-A)^(v-A)) < 0.0)
                return false;
        return true;
}

void Polygon::AddVertex(double dx, double dy)
{
        VertexItem *tmp = new VertexItem;
        tmp->vec = Vector2d(dx, dy);
        tmp->next = ptr;
        ptr = tmp;
}

void Polygon::Rotate(double phi)
{
        VertexItem *tmp;
        for (tmp = ptr; tmp; tmp = tmp->next)
                tmp->vec.Rotate(phi);
}

Triangle::Triangle(const Vector2d& coord, int colour, double a)
        : Polygon(coord, colour)
{
        AddVertex(0.0, a / sqrt(3));
        AddVertex(a / 2.0, -a / (3 * sqrt(3)));
        AddVertex(-a / 2.0, -a / (3 * sqrt(3)));
}

Rectangle::Rectangle(const Vector2d& coord, int colour, double a, double b)
        : Polygon(coord, colour)
{
        AddVertex(-a / 2.0, b / 2.0);
        AddVertex(a / 2.0, b / 2.0);
        AddVertex(a / 2.0, -b / 2.0);
        AddVertex(-a / 2.0, -b / 2.0);
}

RegularPolygon::RegularPolygon(const Vector2d& coord, int colour,
                               double r, int n)
        : Polygon(coord, colour)
{
        double step = 2.0 * PI / n;
        for (int i = n; i > 0; i--) {
                double x = r * cos(step * i);
                double y = r * sin(step * i);
                AddVertex(x, y);
        }
}

void Ellipse::Show() const
{
        glBegin(GL_TRIANGLE_FAN);
        glColor3ub(RED(colour), GREEN(colour), BLUE(colour));
        glVertex2f(coord.X(), coord.Y());
        double step = 2 * PI / 100;
        for (double phi = 0; phi <= 2 * PI; phi += step) {
                double dx = major_axis * cos(phi);
                double dy = minor_axis * sin(phi);
                glVertex2d(coord.X() + dx, coord.Y() + dy);
        }
        glEnd();
}

void Ellipse::Hide() const
{
        glBegin(GL_TRIANGLE_FAN);
        glColor3ub(0, 0, 0);
        glVertex2f(coord.X(), coord.Y());
        double step = 2 * PI / 100;
        for (double phi = 0; phi < 2 * PI; phi += step) {
                double dx = major_axis * cos(phi);
                double dy = minor_axis * sin(phi);
                glVertex2d(coord.X() + dx, coord.Y() + dy);
        }
        glEnd();
}

bool Ellipse::IsInside(const Vector2d& v) const
{
        double x = (v - coord).X();
        double y = (v - coord).Y();
        return pow(x / major_axis, 2) + pow(y / minor_axis, 2) <= 1.0;
}

