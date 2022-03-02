#include "targetset.hpp"
#include "graphobject.hpp"

TargetSet::TargetSet()
{
        used = 0;
        size = 4;
        targets = new Circle*[size];
}

TargetSet::~TargetSet()
{
        for (int i = 0; i < used; i++)
                delete targets[i];
        delete[] targets;
}

void TargetSet::Add(const Vector2d& vec, int colour)
{
        if (used == size) {
                int old_size = size;
                size += 4;
                Circle **tmp = new Circle*[size];
                for (int i = 0; i < old_size; i++)
                        tmp[i] = targets[i];
                delete[] targets;
                targets = tmp;
        }
        targets[used] = new Circle(vec, colour, 15.0);
        used++;
}

void TargetSet::Show() const
{
        for (int i = 0; i < used; i++)
                targets[i]->Show();
}

