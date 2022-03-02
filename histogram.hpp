#ifndef HISTOGRAM_HPP_SENTRY
#define HISTOGRAM_HPP_SENTRY

#include "environment.hpp"

struct Valley {
        int begin;
        int end;
        int size;
        Valley *next;
};

class Histogram {
        double *ph;
        int size;
public:
        Histogram(int len);
        ~Histogram();
        double operator[](int idx) const { return ph[(idx + size) % size]; }
        void Build(const Environment& region);
        void Smooth();
        void Output() const;
        Valley *GetValleys(double threshold) const;
};

#endif /* HISTOGRAM_HPP_SENTRY */

