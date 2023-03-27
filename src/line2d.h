#ifndef line2d_h
#define line2d_h

#include "math.h"
#include "vector2d.h"//shouldnt be necessary
#include "physpoint.h"


class line2d
{
public:
    vector2d* p1;
    vector2d* p2;
    float len;

    int sectionnum;

   // bool frozen;
    vector2d dir;


    // A default constructor
    line2d()
    {
        sectionnum = 0;
    }
    ;

    // This is our constructor that allows us to initialize our data upon creating an instance
    //line2d(physpoint& P1, physpoint &P2){}

    void setup(vector2d& P1, vector2d &P2);
    bool docol(physpoint &player, float walltop = -10000);


    vector2d getperp();


};

#endif
