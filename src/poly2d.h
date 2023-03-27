#ifndef poly2d_h
#define poly2d_h

#include "entity.h"
#include "math.h"
//#include "vector2d.h"//shouldnt be necessary
#include "line2d.h"//shouldnt be necessary
#include "physpoint.h"
//#include "player.h"

#include <list>

using namespace std;

class wallrangeT {
public:
    float low, high;
};


class lc {
public:
    line2d* l;
    bool flipped;
    list<wallrangeT> range;
    float low, high;

    int ltexid;

    void checkagainst(lc& other);

    bool alone;

    // A default constructor
    lc() {};


    ~lc() {
        //delete l;
        range.clear();
    };
};

class poly2d {
public:
    vector2d centre;
    int lnum;
    lc* l;
    float floor, roof;
    int floortex, rooftex;
    float furthestpointdist;//TODO check playeris within this distance!

    string sectortype;
    string sectorvalue;

    //do away with this
    bool ischeckpoint;
    int checkpointid;
    bool isfinish;
    bool isfuelzone;
    bool isspeedzone;


    // A default constructor
    poly2d() {
        sectortype = "";
        sectortype = "";

        //do away with this
        ischeckpoint=0;
        isfinish=0;
        isfuelzone=0;
        isspeedzone=0;
        checkpointid = -1;
    };


    ~poly2d() {
        delete[] l;
    };

    void draw();
    int pnpoly(float x, float y);
    void docolwall(Entity &ent);
    void docolfloor(Entity &ent);
    void getcentre();
    void sortflipped();

};

#endif
