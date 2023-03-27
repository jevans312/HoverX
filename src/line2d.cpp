#include "line2d.h"
#include "math.h"
#include "stdio.h"
#include "iostream"
using namespace std;

float crossz(vector2d a, vector2d b) {
    return a.x * b.y - b.x * a.y;
}

float dot(vector2d a, vector2d b) {
    return a.x * b.x + b.y * a.y;
}

bool line2d::docol(physpoint &player, float walltop) //TODO add friction... maby
{
    //line thickness
    float lthick = player.rad;

    //line vector and direction
    vector2d leq = *p2 - *p1;
    float tdist = len;//sqrt(leq.magsqr());
    if (tdist == 0)
        tdist = 0.0001;
    vector2d dir = leq.divfr(tdist);
    vector2d P = dir.getperp();
    vector2d playervel = player.c - player.o;

    // distance along line to closest point on line to point.
    vector2d B = player.c - *p1;
    float dist = B.x * dir.x + B.y * dir.y;

    //compare if point is on left or right
    //float test2 = crossz(*p2 - *p1, player.o - *p1);
    //float test = crossz(*p2 - *p1, B);
    //if(test2<0 || (flip && test2>0) || walltop != -10000)
    {
        //P.scalef(-1);//player is on the lines right, flip the perpendicular

        //check if we are even in range atall
        if (dist>-lthick && dist<tdist+lthick)//OPTIMISATION! this may be totaly unnecessary
        {
            //place nearest point on line inside line segment
            if (dist > tdist)
                dist = tdist;
            if (dist <0)
                dist = 0;

            //find nearestpoint on line
            vector2d pointonline = *p1+dir.scalefr(dist);

            //find distance to nearest point
            vector2d vectortopoint = player.c-pointonline;
            float disttoline = vectortopoint.magsqr();

            if(disttoline < lthick*lthick)// || bigjump)//should have this for bulletproofing
            {
                if(walltop != -10000) {
                    //walltop = 10;
                    if (player.c.z < walltop && player.o.z >=walltop) {
                        player.c.z=walltop;

                        return 1;
                    }
                }
                else {
                    disttoline=sqrtf(disttoline);
                    dist/=tdist;

                    float d = 2*(lthick-disttoline);//2*(rad-disttonearestpoint)

                    {
                        vectortopoint.divf(lthick);
                        P = vectortopoint;
                        P.divf(sqrtf(P.magsqr()));
                        vector2d newplayervel = playervel - (P.scalefr(2 * dot(playervel,P)));

                        /*player.o=player.c+vectortopoint.scalefr(d);
                        player.c=player.o;//+newplayervel;
                        player.o-=newplayervel;*/
                        player.c+=vectortopoint.scalefr(d);
                        player.o=player.c-newplayervel;

                        //y = x - 2 * dot(x,n) * n;
                        //x==vel
                        //y==result
                        //y = x - ((2 * dot(x,p)) * p);

                        return 1;
                    }
                }
            }
        }
    }

    //cout <<  "line2d::docol return 0" << endl;
    return 0;
}


void line2d::setup(vector2d& P1, vector2d &P2) {
    p1 = &P1;
    p2 = &P2;

    len = hypot(P1.x-P2.x,P1.y-P2.y);

}


/*
vector2d line2d::getperp() {
    vector2d rval;
    rval.x = -dir.y;
    rval.y = dir.x;
    //return rval;
}
*/
