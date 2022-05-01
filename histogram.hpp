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
        Valley *val;
public:
        Histogram(int len);
        ~Histogram();
        void Build(const Environment& region);
        void Smooth();
        void SearchValleys(double threshold);
        void DeleteValleys(); 
        void Output() const;
        Valley *GetValleys() const { return val; }
        double operator[](int idx) const { return ph[(idx + size) % size]; } 
};

#endif /* HISTOGRAM_HPP_SENTRY */

