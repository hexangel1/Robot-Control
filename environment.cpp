#include <cmath>
#include "vector2d.hpp"
#include "environment.hpp"

#if DEBUG == 1
        #include <GLFW/glfw3.h>
#endif

const int Environment::cell_size = 4;

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

void Environment::CopyRegion(const Environment& map, double offx, double offy)
{
        int offsetx = (int)offx / cell_size - width / 2;
        int offsety = (int)offy / cell_size - height / 2;
        offsx = offx;
        offsy = offy;
        for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                        double v = map.Get(offsety + i, offsetx + j);
                        Set(i, j, v);
                }
        }
}

double Environment::Angle(int i, int j) const
{
/*        if (i == height / 2 && j == width / 2)
                return 0; 
        return sin((i - height / 2) / Distance(i, j)) >= 0 ? 
               acos((j - width / 2) / Distance(i, j)) :
               2*PI - acos((j - width / 2) / Distance(i, j));
*/
        double u = offsy - ((height / 2) * cell_size) + i * cell_size;
        double v = offsx - ((width / 2) * cell_size) + j * cell_size;
        return ANGLE_2PI(atan2(u - offsy, v -offsx));
        return ANGLE_2PI(atan2(i - height / 2, j - width / 2));
}

double Environment::Distance(int i, int j) const
{
        double u = offsy - ((height / 2) * cell_size) + i * cell_size;
        double v = offsx - ((width / 2) * cell_size) + j * cell_size;
        return sqrt(pow(u - offsy, 2) + pow(v - offsx, 2));
        return sqrt(pow(i - height / 2, 2) + pow(j - width / 2, 2));
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

