#ifndef LOCALMAP_HPP_SENTRY
#define LOCALMAP_HPP_SENTRY

class LocalMap {
        int width;
        int height;
        double *map;
public:
        LocalMap(int w, int h);
        ~LocalMap();
        inline void Set(int i, int j, double v) {
                if (0 <= i && i < height && 0 <= j && j < width)
                        map[i * width + j] = v;
        }
        inline double Get(int i, int j) const {
                if (0 <= i && i < height && 0 <= j && j < width)
                        return map[i * width + j];
                return 1.0;
        }
        inline int Width() const { return width; }
        inline int Height() const { return height; }
        static const int cell_size;
#ifdef DEBUG
        void Display();
#endif
};

#endif /* LOCALMAP_HPP_SENTRY */

