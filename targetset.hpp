#ifndef TARGETSET_HPP_SENTRY
#define TARGETSET_HPP_SENTRY

#include "graphobject.hpp"

class TargetSet {
        Circle **targets;
        int used;
        int size;
public:
        TargetSet();
        ~TargetSet();
        int GetAmount() const { return used; }
        Vector2d operator[](int idx) const { return targets[idx]->GetXY(); }
        int GetCol(int idx) const { return targets[idx]->Colour(); }
        void Add(const Vector2d& vec, int colour);
        void Show() const;
};

#endif /* TARGET_HPP_SENTRY */

