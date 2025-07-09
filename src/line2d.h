#pragma once

#include "vector2d.h"
#include "physpoint.h"

class line2d {
public:
    vector2d* p1 = nullptr;
    vector2d* p2 = nullptr;
    float len = 0.0f;
    int sectionnum = 0;
    vector2d dir;

    // Constructors
    line2d() = default;

    // Member functions
    void setup(vector2d& P1, vector2d& P2);
    bool docol(physpoint& player, float walltop = -10000.0f);
    vector2d getperp();
};
