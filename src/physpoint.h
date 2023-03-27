#ifndef physpoint_h
#define physpoint_h
#include "vector2d.h"

class physpoint {
public:
    vector2d c,o;
    float rad;

    physpoint() {}

    physpoint(vector2d C, vector2d O)   {
        c = C;  //current
        o = O;  //old
    }

    void dophys(float drag, float timediv)  {
        vector2d v = (c-o);

        o=c;
        drag = 1-drag;
        drag/=timediv;
        drag = 1-drag;
        v.scalef(drag);

        if (timediv == 1)
            c+=v;
        else    {
            float scalar = 1/timediv;
            c+=v.scalefr(scalar);
            o-=v.scalefr(1-scalar);
        }

    }

    void teleport(float x, float y) {
        c.x = x;
        o.x = x;
        c.y = y;
        o.y = y;

    }


};

#endif
