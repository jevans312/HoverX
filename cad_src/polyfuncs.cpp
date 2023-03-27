#include "hovercad.h"

int pnpoly(float x, float y, int polyid)
{
    int i, j, c = 0;
    for (i = 0; i < polys[polyid].sidecount+1; i++)
    {
        if ((((verts[lines[polys[polyid].p[i]].a].y <= y) && (y < verts[lines[polys[polyid].p[i]].b].y)) ||
                ((verts[lines[polys[polyid].p[i]].b].y <= y) && (y < verts[lines[polys[polyid].p[i]].a].y))) &&
                (x < (verts[lines[polys[polyid].p[i]].b].x - verts[lines[polys[polyid].p[i]].a].x) * (y - verts[lines[polys[polyid].p[i]].a].y) / (verts[lines[polys[polyid].p[i]].b].y - verts[lines[polys[polyid].p[i]].a].y) + verts[lines[polys[polyid].p[i]].a].x))
            c = !c;
    }
    if(!c)
        c=-1;
    return c;
}

//todo: should find 1 poly thats inside the poly
//todo: set the lines in the polys texture in the ui fllinetex->value(polys[i].tex+1);
/*int select_poly(float x, float y)
{
    bool found = false;
    //selected_poly = -1;
    for (int i = 0; i < poly_count;i++)
    {
        int tpol = pnpoly(x,y,i);
        if (tpol != -1)
        {
            polys[i].selected = true;
            found = true;
            flpolyroof->value(polys[i].roof) ;
            flpolyfloor->value(polys[i].floor);
            flrooftex->value(polys[i].rooftex+1);
            flfloortex->value(polys[i].floortex+1);
            //selected_poly = i;
        }
    }
    set_selection();
    return found;
}*/

//modified to find one poly that is the closest to the mouse note: a little buggy
int select_poly(float x, float y)
{
    int foundlist[63];
    foundlist[0] = -1;
    int foundcount = 0;

    for (int i = 0; i < poly_count;i++)
    {
        int tpol = pnpoly(x,y,i);
        if (tpol != -1) {
            foundlist[foundcount] = i;
            foundlist[foundcount+1] = -1;
            foundcount++;
        }
    }

    if(foundcount == 0) return -1;

    int closestpoly = -1;
    float closestdist = -1;
    float tdist = 0.f;
    float avagedist = 0.f;

    //find the closest poly and select just it
    for (int i = 0; i < foundcount; i++) {
        //get the aveage distance to all lines in the poly
        for (int ii = 0; polys[foundlist[i]].p[ii] != -1; ii++) {
            tdist = tdist + disttoline(x, y, polys[foundlist[i]].p[0] );
        }
        tdist = tdist/(polys[foundlist[i]].sidecount+1);

        if(tdist < closestdist || closestdist == -1) {
            closestpoly = foundlist[i];
            closestdist = tdist;
        }
    }

    polys[closestpoly].selected = true;
    flpolyroof->value(polys[closestpoly].roof) ;
    flpolyfloor->value(polys[closestpoly].floor);
    flrooftex->value(polys[closestpoly].rooftex+1) ;
    flfloortex->value(polys[closestpoly].floortex+1);

    set_selection();
    return closestpoly;
}

void deletepoly(int id)
{

 //vector<poly> polyi;
    polys.erase(polys.begin()+id);
    poly_count--;
}

//bug: random crash while creating polys, not totally reproducable maybe cause by this func?
void assemble_poly(float x, float y, bool close)    {
    int found_vert;
    static int polystartpoint = -1;
    static int prevpoint = -1;
    static int pbprevpoint = -1;

    if(prevpoint+1) {
        found_vert = nearest_vert(x, y);
        if(close)   {   //dont think this bit is used
            found_vert = polystartpoint;
        }
        if(/*(pbprevpoint != found_vert || pbprevpoint == -1) && */(prevpoint != found_vert || prevpoint == -1)){//uncomment bit here, eh?
            if(found_vert != polystartpoint)  { //add a line to the new poly
                int newline = find_add_line_xyv(x,y,prevpoint);
                polys[poly_count].p[polys[poly_count].sidecount] = newline;
                lines[newline].red = true;  //turn lines in poly being created red
                polys[poly_count].sidecount++;
                pbprevpoint = prevpoint;
                prevpoint = get_other_line_point(newline, prevpoint);
            } else {    //complete the new polly
                int newline = find_add_line_vv(polystartpoint,prevpoint);
                polys[poly_count].p[polys[poly_count].sidecount] = newline;
                polys[poly_count].complete = 1;
                poly_count++;
                prevpoint = -1;
                pbprevpoint = -1;

                //turn off red warnings
                for(int i = 0; i < line_count; i++) lines[i].red = false;
                for(int i = 0; i < vert_count; i++) verts[i].red = false;
            }
        } else {
             cout << "POLYMUST HAVE 3 COMPONENTS" << endl;
             cout << "3 points are " << found_vert << " at " << verts[found_vert].x << ", " << verts[found_vert].y << endl;
             cout << "and " << prevpoint << " at " << verts[prevpoint].x << ", " << verts[prevpoint].y << endl;
             cout << "and " << pbprevpoint << " at " << verts[pbprevpoint].x << ", " << verts[pbprevpoint].y << endl;
        }

    } else {  // we need to start a new poly
        poly temppoly;
        polys.push_back(temppoly);
        pbprevpoint = -1;
        prevpoint = add_vert_near(x,y);
        polystartpoint = prevpoint;
        verts[polystartpoint].red = true;   //turn first firt red to indicate where to finish
    }
}





