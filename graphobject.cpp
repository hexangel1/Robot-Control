#include <cmath>
#include <GLFW/glfw3.h>
#include "graphobject.hpp"

void GraphObject::Move(const Vector2D& move)
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

bool Ellipse::IsInside(const Vector2D& point) const
{
        Vector2D v = point - coord;
        return pow(v.X() / major_axis, 2) + pow(v.Y() / minor_axis, 2) <= 1;
}

void Ellipse::Mapping(LocalMap& map, bool s) const
{
        double x, y;
        int size = LocalMap::cell_size;
        for (x = -major_axis; x < major_axis; x++) {
                for (y = -minor_axis; y < minor_axis; y++) {
                        if (IsInside(coord + Vector2D(x, y))) {
                                int j = (int)(coord.X() + x) / size;
                                int i = (int)(coord.Y() + y) / size;
                                map.Set(i, j, s ? 1.0 : 0.0);
                        }
                }
        }
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

bool Rectangle::IsInside(const Vector2D& point) const
{
        Vector2D v = point - coord;
        return v.X() <= width / 2 && v.X() >= -width / 2 &&
               v.Y() <= height / 2 && v.Y() >= -height / 2;
}

void Rectangle::Mapping(LocalMap& map, bool s) const
{
        double x, y;
        int size = LocalMap::cell_size;
        for (x = -width / 2; x < width / 2; x++) {
                for (y = -height / 2; y < height / 2; y++) {
                        int j = (int)(coord.X() + x) / size;
                        int i = (int)(coord.Y() + y) / size;
                        map.Set(i, j, s ? 1.0 : 0.0);
                }
        }
}

