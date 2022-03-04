#include <cmath>
#include <cstdio>
#include "vector2d.hpp"
#include "histogram.hpp"

Histogram::Histogram(int len) : size(len)
{
        ph = new double[size];
        for (int i = 0; i < size; i++)
                ph[i] = 0.0;
}

Histogram::~Histogram()
{
        delete[] ph;
}

void Histogram::Build(const Environment& region)
{
        double a = 100.0;
        double b = a / (((double)region.Width() - 1.0) * sqrt(2.0) / 2.0 * 4);
        for (int k = 0; k < size; k++)
                ph[k] = 0.0;
        for (int i = 0; i < region.Width(); i++) {
                for (int j = 0; j < region.Height(); j++) {
                        int k = region.Angle(i, j) / (2 * PI / size);
                        double d = region.Distance(i, j);
                        ph[k] += pow(region.Get(i, j), 2) * (a - b * d);
                }
        }
}

void Histogram::Smooth()
{
        const double weight[] = { 0.2, 0.16, 0.12, 0.08, 0.04 };
        double *tmp = new double[size];
        for (int k = 0; k < size; k++)
                tmp[k] = ph[k];
        for (int k = 0; k < size; k++) {
                ph[k] = 0.0;
                for (int i = -4; i <= 4; i++)
                        ph[k] += weight[abs(i)] * tmp[(size + k - i) % size];
        }
        delete[] tmp;
}

void Histogram::Output() const
{
        for (int i = 0; i < size; i++)
                fprintf(stdout, "%i %lf\n", 5*i, ph[i]);
}

Valley *Histogram::GetValleys(double threshold) const
{
        Valley *tmp, *ret = 0;
        int start = 0, length = 0;       
        for (int k = 0; k < size; k++) {
                if (ph[k] <= threshold && k != size - 1) {
                        length++;
                        continue;
                }
                if (length > 0) {
                        tmp = new Valley;
                        tmp->begin = start;
                        if (k == size - 1)
                                tmp->end = k;
                        else
                                tmp->end = k - 1;
                        tmp->size = length;
                        tmp->next = ret;
                        ret = tmp;
                        start = k;
                        length = 0;
                }
                start++;
        }
        if (ret && ret->next) {
                for (tmp = ret; tmp->next; tmp = tmp->next)
                        ;
                if (tmp->begin == 0 && ret->end == size - 1) {
                        ret->end = size + tmp->end;
                        Valley *tmp2 = ret;
                        while (tmp2->next != tmp)
                                tmp2 = tmp2->next;
                        tmp2->next = 0;
                        delete tmp;
                }
        }
        return ret;
}

