#ifndef ENVIRONMENT_HPP_SENTRY
#define ENVIRONMENT_HPP_SENTRY

class Environment {
        int width;
        int height;
        double *map;
        double offsx;
        double offsy;
public:
        Environment(int w, int h);
        ~Environment();
        void CopyRegion(const Environment& map, double offx, double offy);
        double Angle(int i, int j) const;
        double Distance(int i, int j) const;
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
#if DEBUG == 1
        void Display();
#endif
};

#endif /* ENVIRONMENT_HPP_SENTRY */

