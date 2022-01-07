#include <cmath>
#include "vector2d.hpp"
#include "histogram.hpp"

void Histogram::Build(const Environment& region)
{
        double a = 100.0;
        double b = a / (((double)region.Width() - 1.0) * sqrt(2.0) / 2.0);
        double h[size];
        for (int k = 0; k < size; k++)
                h[k] = 0.0;
        for (int i = 0; i < region.Width(); i++) {
                for (int j = 0; j < region.Height(); j++) {
                        int k = region.Angle(i, j) / (2 * PI / size);
                        double d = region.Distance(i, j);
                        h[k] += pow(region.Get(i, j), 2) * (a - b * d);
                }
        }
        for (int k = 0; k < size; k++) {
                double sum = 0.0;
                ph[k] = 0.0;
                for (int i = -2; i <= 2; i++) {
                        int r = size;
                        double weight = 2 + 1 - abs(i);
                        ph[k] += weight * h[(r + k - i) % r];
                        sum += weight;
                }
                ph[k] /= sum;
        }
}

