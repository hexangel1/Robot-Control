#include "localmap.hpp"

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

