#ifndef HISTOGRAM_HPP_SENTRY
#define HISTOGRAM_HPP_SENTRY

#include "environment.hpp"

class Histogram {
        double *ph;
        int size;
public:
        Histogram(int s) { ph = new double[s]; size = s; }
        ~Histogram() { delete[] ph; }
        double operator[](int idx) const { return ph[(idx + size) % size]; }
        void Build(const Environment& region);
};

#endif /* HISTOGRAM_HPP_SENTRY */

