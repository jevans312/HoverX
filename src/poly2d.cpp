#include "poly2d.h"
#include "glness.h"

using namespace std;

extern float crossz(vector2d a, vector2d b);
list<wallrangeT>::iterator rangei;//WARNING!! this is prob actauly fine, but it should probably *not* be shared!

//list<wallrangeT>::iterator c1,c2;
wallrangeT walladd;

//when two rooms/polys share a line
//if they dont have an exactly equal floor and celing height
//this bit of code adds the necessary walls
void lc::checkagainst(lc& other)
{
    if (l == other.l)
    {
        alone = 0;
        other.alone = 0;
        /*c1 = range.begin();
        c2 = other.range.begin();
        if(range.size() == 1 && other.range.size() == 1 && c1->low == c2->low && c1-> high == c2->high) {
            cout << "MATCH!" << endl;
            range.clear();
            other.range.clear();
        }else {
        }*/
        //we are higher than other room?
        if(low >= other.low && high >= other.high)
        {
            walladd.low = other.high;
            walladd.high = high;
            range.push_back(walladd);
        }
        //lower than other room?
        if(low <= other.low && high <= other.high)
        {
            walladd.low = low;
            walladd.high = other.low;
            range.push_back(walladd);
        }
        //lower and higher?
        if(low <= other.low && high >= other.high)
        {
            walladd.low = low;
            walladd.high = other.low;
            range.push_back(walladd);
            walladd.low = other.high;
            walladd.high = high;
            range.push_back(walladd);
        }
    }
}

//is input position in this polygon?
int poly2d::pnpoly(float x, float y)
{
    int i = 0;
    int c = 0;
    for (i = 0; i < lnum; i++)
    {
        if ((((l[i].l->p1->y <= y) && (y < l[i].l->p2->y)) ||
                ((l[i].l->p2->y <= y) && (y < l[i].l->p1->y))) &&
                (x < (l[i].l->p2->x - l[i].l->p1->x) * (y - l[i].l->p1->y) / (l[i].l->p2->y - l[i].l->p1->y) + l[i].l->p1->x))
            c = !c;
    }
    return c;
}

//Collisin with the floor and celing!
//extra checks to make sure you are in the right room
void poly2d::docolfloor(Entity& ent)    {   //this should be called sector check
    //antionground = 0;
    float newfloor = floor+0.25;                            //TODO scaleme
    if(pnpoly(ent.pos.c.x, ent.pos.c.y)) {
        if(ent.pos.c.z < newfloor && ent.pos.o.z >= floor ) {
            ent.pos.c.z = newfloor;
            ent.onground = 1;
        }
        if(ent.pos.c.z > floor-ent.height && ent.pos.o.z <= floor-ent.height )    {
            ent.pos.c.z = ent.pos.o.z-0.1;
            ent.antionground = 1;
            ent.jumpingvel = 0;
        }
        if(ent.pos.c.z < roof+0.25 && ent.pos.o.z >=  roof) {
            ent.pos.c.z =  roof+0.25;
            ent.onground = 1;
        }
        if(ent.pos.o.z <= roof-ent.height && ent.pos.c.z > roof-ent.height)   {
            ent.pos.c.z = ent.pos.o.z-0.1 ;
            ent.antionground = 1;
            ent.jumpingvel = 0;
        }

        ent.SectorType = sectortype;
        ent.SectorValue = sectorvalue;

        /*
        if(ischeckpoint)    {
            if(checkpointid-1 == ent.currentcheckpoint)
                ent.currentcheckpoint++;
        }

        if(isfinish)    {
            if(ent.currentcheckpoint == ent.lastcheckpoint){
                if(ent.currentlap == 1){
                    ent.bestlaptime = SDL_GetTicks();
                } else {
                    int thislaptime = SDL_GetTicks() - ent.lastlaptime;
                    if (thislaptime < ent.bestlaptime) ent.bestlaptime = thislaptime;
                }

                ent.lastlaptime = SDL_GetTicks();
                ent.currentlap++;
                ent.currentcheckpoint = -1;
            }
        }
        */
        //onground = 1;
    }

    if(!ent.onground && !ent.antionground){
        for (int lid = 0; lid <lnum;lid++)  {
            if(l[lid].l->docol(ent.pos, newfloor))
                ent.onground = 1;

            if(l[lid].l->docol(ent.pos,  roof+0.25))
                ent.onground = 1;
        }
    }
}


// loop through all the walls in a poly, and check if an entity is within
//its height ranges, if so pass it to a line/circle collision func
void poly2d::docolwall(Entity& ent)
{
    for (int lid = 0; lid <lnum;lid++)
    {
        for (rangei = l[lid].range.begin(); rangei != l[lid].range.end();rangei++)
        {
            bool testMe = false;
            if(ent.pos.c.z > rangei->low && ent.pos.c.z < rangei->high){
                testMe = true;
            }
            if(ent.pos.c.z < rangei->low && ent.pos.c.z+ent.height > rangei->low){
                testMe = true;
            }
            if(ent.pos.c.z < rangei->high && ent.pos.c.z+ent.height > rangei->high){
                testMe = true;
            }
            //if(ent.pos.c.z > rangei->low && ent.pos.c.z < rangei->high){
            if(testMe){
                // wall jumping
                //if(l[lid].l->docol(ent.pos))ent.onground = 1;
                l[lid].l->docol(ent.pos);      // no wall jumpping
            }
        }
    }
}

//for each line in the poly, when looking from p1 to p2, is the centre
//of the poly on the left or the right?
void poly2d::sortflipped()
{
    for (int lid = 0; lid <lnum;lid++)
    {
        float ztest = crossz(*l[lid].l->p2 - *l[lid].l->p1, centre - *l[lid].l->p1);
        //cout << ztest << endl;
        if (ztest <= 0)
        {
            l[lid].flipped = 0;
        }
        else
        {
            l[lid].flipped = 1;
        }
    }
}

//find the centre of a poly(by averaging all the end points in its lines)
//then find the furthest point from the center
void poly2d::getcentre()
{
    centre.setpos(0,0);
    //add up all the points's position
    for (int lid = 0; lid <lnum;lid++)
    {
        centre += *l[lid].l->p1;
        centre += *l[lid].l->p2;
    }
    //divide by number to get mean average
    centre.divf((lnum*2));

    //loop through all points, to see which is furthest away
    furthestpointdist = 0;
    vector2d dist;
    for (int lid = 0; lid <lnum;lid++)
    {
        dist = centre - *l[lid].l->p1;
        if (dist.magsqr() > furthestpointdist)
            furthestpointdist = dist.magsqr();
        dist = centre - *l[lid].l->p2;
        if (dist.magsqr() > furthestpointdist)
            furthestpointdist = dist.magsqr();
    }
}

void poly2d::draw() {   //TODO fix this
    float dx = 0.0f;
    float dy = 0.0f;
    float dx2 = 0.0f;
    float dy2 = 0.0f;

    // first attempt at lighting
    GLfloat mat_amb_diff[] = {0.8, 0.8, 0.8, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);

    //glDisable(GL_CULL_FACE);
    //draw floor
    if(floortex != -1)
    {
        glBindTexture( GL_TEXTURE_2D, floortex );
        glBegin(GL_POLYGON);
        for(int i = lnum-1; i > -1; i--)
        {
            if (!l[i].flipped)
            {
                dx = l[i].l->p1->x;
                dy = l[i].l->p1->y;
            }
            else
            {
                dx = l[i].l->p2->x;
                dy = l[i].l->p2->y;
            }
            glTexCoord2f(dx/5, dy/5);//TODO scaleme
            glVertex3f( dx, dy, floor);
        }
        glEnd();


        /*glBegin(GL_TRIANGLE_FAN);
            glTexCoord2f(centre.x/5, centre.y/5);//TODO scaleme
            glVertex3f( centre.x, centre.y, floor+1);
        for(int i = 0; i < lnum; i++)
        {
            if (!l[i].flipped)
            {
                dx = l[i].l->p1->x;
                dy = l[i].l->p1->y;
            }
            else
            {
                dx = l[i].l->p2->x;
                dy = l[i].l->p2->y;
            }
            glTexCoord2f(dx/5, dy/5);//TODO scaleme
            glVertex3f( dx, dy, floor);
        }
        glEnd();*/
    }

    //draw roof
    if(rooftex != -1)
    {
        glBindTexture( GL_TEXTURE_2D, rooftex );
        glBegin(GL_POLYGON);
        for(int i = 0; i < lnum; i++)
        {
            if (!l[i].flipped)
            {
                dx = l[i].l->p1->x;
                dy = l[i].l->p1->y;
            }
            else
            {
                dx = l[i].l->p2->x;
                dy = l[i].l->p2->y;
            }
            glTexCoord2f(dx/5, dy/5);//TODO scaleme
            glVertex3f( dx, dy, roof);
        }
        glEnd();
    }

    //glEnable(GL_CULL_FACE);

    /*for(int i = 0; i < lnum; i++) {
    for(int i = 0; i < lnum; i++) {
        dx = l[i].l->p1->x;
        dy = l[i].l->p1->y;
        glTexCoord2f(dx/100, dy/100);
        glVertex3f( dx, dy, roof);
    }
    }*/

    //draw walls
    for(int i = 0; i < lnum; i++)
    {
        if(l[i].ltexid != -1)
        {
            glBindTexture( GL_TEXTURE_2D, l[i].ltexid );

            for(rangei = l[i].range.begin();rangei != l[i].range.end();rangei++)
            {
                if (!l[i].flipped)
                {
                    dx = l[i].l->p1->x;
                    dy = l[i].l->p1->y;
                    dx2 = l[i].l->p2->x;
                    dy2 = l[i].l->p2->y;
                }
                else
                {
                    dx2 = l[i].l->p1->x;
                    dy2 = l[i].l->p1->y;
                    dx = l[i].l->p2->x;
                    dy = l[i].l->p2->y;
                }
                glBegin(GL_QUADS);
                    glTexCoord2f(0.0f, 1.0f);
                    glVertex3f( dx, dy, rangei->low);

                    glTexCoord2f(l[i].l->len/5, 1.0f);  //TODO scaleme
                    glVertex3f( dx2, dy2, rangei->low);
                    
                    glTexCoord2f(l[i].l->len/5, 0.0f);  //TODO scaleme
                    glVertex3f( dx2, dy2, rangei->high);
                    
                    glTexCoord2f(0.0f, 0.0f);
                    glVertex3f( dx, dy, rangei->high);
                glEnd();
            }
        }
    }

}
