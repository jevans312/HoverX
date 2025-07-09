#pragma once

#include <list>
#include <string>

#include "entity.h"
#include "line2d.h"

class wallrangeT {
public:
    float low = 0.0f, high = 0.0f;
};

class lc {
public:
    line2d* l = nullptr;
    bool flipped = false;
    std::list<wallrangeT> range;
    float low = 0.0f, high = 0.0f;
    int ltexid = -1;
    bool alone = false;

    lc() = default;
    ~lc() = default;

    void checkagainst(lc& other);
};

class poly2d {
public:
    vector2d centre;
    int lnum = 0;
    lc* l = nullptr;
    float floor = 0.0f, roof = 0.0f;
    int floortex = -1, rooftex = -1;
    float furthestpointdist = 0.0f; // TODO: check player is within this distance!

    std::string sectortype;
    std::string sectorvalue;

    // Sector flags
    bool ischeckpoint = false;
    int checkpointid = -1;
    bool isfinish = false;
    bool isfuelzone = false;
    bool isspeedzone = false;

    poly2d() = default;
    ~poly2d() { delete[] l; }

    void draw();
    int pnpoly(float x, float y);
    void docolwall(Entity& ent);
    void docolfloor(Entity& ent);
    void getcentre();
    void sortflipped();
};
