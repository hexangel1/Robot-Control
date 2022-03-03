#include <cmath>
#include <GLFW/glfw3.h>
#include "graphobject.hpp"

void GraphObject::Move(const Vector2d& move)
{
        Hide();
        coord = move;
        Show();
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
                glVertex2f(coord.X() + dx, coord.Y() + dy);
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
                glVertex2f(coord.X() + dx, coord.Y() + dy);
        }
        glEnd();
}

bool Ellipse::IsInside(const Vector2d& point) const
{
        Vector2d v = point - coord;
        return pow(v.X() / major_axis, 2) + pow(v.Y() / minor_axis, 2) <= 1;
}

void Rectangle::Show() const
{
        glBegin(GL_QUADS);
        glColor3ub(RED(colour), GREEN(colour), BLUE(colour));
        glVertex2f(coord.X() - width / 2, coord.Y() + height / 2);
        glVertex2f(coord.X() - width / 2, coord.Y() - height / 2);
        glVertex2f(coord.X() + width / 2, coord.Y() - height / 2);
        glVertex2f(coord.X() + width / 2, coord.Y() + height / 2);
        glEnd();
}

void Rectangle::Hide() const
{
        glBegin(GL_QUADS);
        glColor3ub(0, 0, 0);
        glVertex2f(coord.X() - width / 2, coord.Y() + height / 2);
        glVertex2f(coord.X() - width / 2, coord.Y() - height / 2);
        glVertex2f(coord.X() + width / 2, coord.Y() - height / 2);
        glVertex2f(coord.X() + width / 2, coord.Y() + height / 2);
        glEnd();
}

bool Rectangle::IsInside(const Vector2d& point) const
{
        Vector2d v = point - coord;
        return v.X() <= width / 2 && v.X() >= -width / 2 &&
               v.Y() <= height / 2 && v.Y() >= -height / 2;
}

void Triangle::Show() const
{
        glBegin(GL_TRIANGLES);
        glColor3ub(RED(colour), GREEN(colour), BLUE(colour));
        glVertex2d(coord.X() + va.X(), coord.Y() + va.Y());
        glVertex2d(coord.X() + vb.X(), coord.Y() + vb.Y());
        glVertex2d(coord.X() + vc.X(), coord.Y() + vc.Y());
        glEnd();
}

void Triangle::Hide() const
{
        glBegin(GL_TRIANGLES);
        glColor3ub(0, 0, 0);
        glVertex2d(coord.X() + va.X(), coord.Y() + va.Y());
        glVertex2d(coord.X() + vb.X(), coord.Y() + vb.Y());
        glVertex2d(coord.X() + vc.X(), coord.Y() + vc.Y());
        glEnd();
}

bool Triangle::IsInside(const Vector2d& p) const
{
        double a, b, c;
        Vector2d vat = coord + va;
        Vector2d vbt = coord + vb;
        Vector2d vct = coord + vc;
        a = (vat.X() - p.X()) * (vbt.Y() - vat.Y()) -
            (vat.Y() - p.Y()) * (vbt.X() - vat.X());
        b = (vbt.X() - p.X()) * (vct.Y() - vbt.Y()) -
            (vbt.Y() - p.Y()) * (vct.X() - vbt.X());
        c = (vct.X() - p.X()) * (vat.Y() - vct.Y()) -
            (vct.Y() - p.Y()) * (vat.X() - vct.X());
        return (a >= 0 && b >= 0 && c >= 0) || (a <= 0 && b <= 0 && c <= 0);
}

