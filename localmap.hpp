#ifndef LOCALMAP_HPP_SENTRY
#define LOCALMAP_HPP_SENTRY

class LocalMap {
        int width;
        int height;
        double *map;
public:
        LocalMap(int w, int h);
        ~LocalMap();
        void Set(int i, int j, double v);
        double Get(int i, int j) const;
        inline int Width() const { return width; }
        inline int Height() const { return height; }
        static const int cell_size;
#ifdef DEBUG
        void Display();
#endif
};

#endif /* LOCALMAP_HPP_SENTRY */

