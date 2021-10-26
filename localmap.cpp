#include "localmap.hpp"
#include <GLFW/glfw3.h>

const int LocalMap::cell_size = 5;

LocalMap::LocalMap(int w, int h)
{
        width = w / cell_size;
        height = h / cell_size;
        map = new double[width * height];
        for (int i = 0; i < width * height; i++)
                map[i] = 0.0;
}

LocalMap::~LocalMap()
{
        delete[] map;
}

void LocalMap::Set(int i, int j, double v)
{
        if (0 <= i && i < height && 0 <= j && j < width)
                map[i * width + j] = v;
}

double LocalMap::Get(int i, int j) const
{
        if (0 <= i && i < height && 0 <= j && j < width)
                return map[i * width + j];
        return 1.0;
}

void LocalMap::Output()
{
        glBegin(GL_QUADS);
        int size = cell_size;
        glColor3f(1.0, 0.0, 0.0);
        for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                        if (map[i * width + j] < 1.0)
                                continue;
                        glVertex2f(j * size, i * size);
                        glVertex2f((j + 1) * size, i * size);
                        glVertex2f((j + 1) * size, (i + 1) * size);
                        glVertex2f(j * size, (i + 1) * size);
                }
        }
        glEnd();
}

