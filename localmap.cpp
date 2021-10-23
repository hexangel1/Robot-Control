#include "localmap.hpp"

const int LocalMap::cell_size = 5;

LocalMap::LocalMap(int w, int h)
{
        int i, j;
        width = w / cell_size;
        height = h / cell_size;
        map = new double*[height];
        for (i = 0; i < height; i++)
                map[i] = new double[width];
        for (i = 0; i < height; i++)
                for (j = 0;  j < width; j++)
                        map[i][j] = 0.0;
}

LocalMap::~LocalMap()
{
        for (int i = 0; i < height; i++)
                delete[] map[i];
        delete[] map;
}

void LocalMap::Set(int i, int j, double v)
{
        if (0 <= i && i < height && 0 <= j && j < width)
                map[i][j] = v;
}

double LocalMap::Get(int i, int j) const
{
        if (0 <= i && i < height && 0 <= j && j < width)
                return map[i][j];
        return 1.0;
}

