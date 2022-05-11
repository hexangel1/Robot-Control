#include <cmath>
#include <cstdio>
#include "vector2d.hpp"
#include "histogram.hpp"

Histogram::Histogram(int len) : size(len), val(0)
{
        ph = new double[size];
        for (int i = 0; i < size; i++)
                ph[i] = 0.0;
}

Histogram::~Histogram()
{
        delete[] ph;
        DeleteValleys();
}

void Histogram::Build(const Environment& region)
{
        double d_max = static_cast<double>(region.Width() - 1) * 
                       sqrt(2.0) / 2.0 * Environment::cell_size;
        double a = 10.0;
        double b = a / d_max;
        double alpha = 2 * PI / size;
        for (int k = 0; k < size; k++)
                ph[k] = 0.0;
        for (int i = 0; i < region.Width(); i++) {
                for (int j = 0; j < region.Height(); j++) {
                        int k = region.Angle(i, j) / alpha;
                        double d = region.Distance(i, j);
                        ph[k] += region.Get(i, j) * (a - b * d);
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

void Histogram::SearchValleys(double threshold)
{
        Valley *tmp;
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
                        tmp->next = val;
                        val = tmp;
                        start = k;
                        length = 0;
                }
                start++;
        }
        if (val && val->next) {
                for (tmp = val; tmp->next; tmp = tmp->next)
                        ;
                if (tmp->begin == 0 && val->end == size - 1) {
                        val->end = size + tmp->end;
                        val->size += tmp->size;
                        Valley *tmp2 = val;
                        while (tmp2->next != tmp)
                                tmp2 = tmp2->next;
                        tmp2->next = 0;
                        delete tmp;
                }
        }
}

void Histogram::DeleteValleys()
{
         while (val) {
                Valley *tmp = val;
                val = val->next;
                delete tmp;
        }
}

void Histogram::Output() const
{
        for (int i = 0; i < size; i++)
                printf("%i %f\n", 5*i, ph[i]);
}

