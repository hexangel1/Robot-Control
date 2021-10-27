#include "environment.hpp"
#if DEBUG == 1
        #include <GLFW/glfw3.h>
#endif

const int Environment::cell_size = 5;

Environment::Environment(int w, int h)
{
        width = w;
        height = h;
        map = new double[width * height];
        for (int i = 0; i < width * height; i++)
                map[i] = 0.0;
}

Environment::~Environment()
{
        delete[] map;
}

#if DEBUG == 1
void Environment::Display()
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
#endif

