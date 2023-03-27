#include "hovercad.h"

float disttoline(float x, float y, int lineid)
{
    //line thickness

    float xa,ya,xb,yb;
    xa = verts[lines[lineid].a].x;
    ya = verts[lines[lineid].a].y;
    xb = verts[lines[lineid].b].x;
    yb = verts[lines[lineid].b].y;

    float xd,yd,px,py;
    int selectnearespoint = 0;
    if(selectnearespoint)
    {

        xd = xa-xb;
        yd = ya-yb;
        //line vector and direction
        //vector2d leq = *p2 - *p1;
        //float tdist = len;//sqrt(leq.magsqr());
        float tdist = sqrt(xd*xd+yd*yd);
        if (tdist == 0)
            tdist = 0.0001;
        //vector2d dir = leq.divfr(tdist);
        xd/=tdist;
        yd/=tdist;

        // distance along line to closest point on line to point.
        //vector2d B = player.c - *p1;
        float Bx = x-xa;
        float By = y-ya;
        //float dist = B.x * dir.x + B.y * dir.y;
        float distonline = Bx * xd + By * yd;

        //place nearest point on line inside line segment
        if (distonline > tdist)
            distonline = tdist;
        if (distonline <0)
            distonline = 0;

        //find nearestpoint on line
        //vector2d pointonline = *p1+dir.scalefr(distonline);
        px = xa+xd*distonline;
        py = ya+yd*distonline;
    }
    else
    {
        px = (xa+xb)/2;
        py = (ya+yb)/2;
    }

    //find distance to nearest point
    //vector2d vectortopoint = player.c-pointonline;
    //float disttoline = vectortopoint.magsqr();
    xd = px-x;
    yd = py-y;
    float td = (xd*xd+yd*yd);
    return td;

}

int find_line_polyside(int p, int l) {
    for(int i = 0; polys[p].p[i] != -1; i++) {
            if(l == polys[p].p[i]) return i;
    }

    return -1;
}

bool select_line(float x, float y)  {
    bool found = false;
    float maxdist = flvertsize->value()*cam.z;
    int nearestline = -1;

    //find 1 line within distance
    for (int i = 0; i < line_count; i++) {
        float chkdist = sqrt(disttoline(x,y,i));
        if (chkdist < maxdist)  {
            maxdist = chkdist;
            nearestline = i;
            found = true;
        }
    }

    //set line to selected
    if(found)   lines[nearestline].selected = true;

    //update texture display
    //select polys and the sides that are to be textured
    for(int i = 0; i < poly_count; i++) {
       int polylinenumber = find_line_polyside(i, nearestline);
            if(polylinenumber != -1) {
                cout << "poly " << i << " side #" << polylinenumber << endl; //<< polys[i].p[polylinenumber] << endl;
                polys[i].selected = true;
            fllinetex->value(polys[i].tex[polylinenumber] + 1);
            cout << "reading back poly tex data " << polys[i].tex[polylinenumber] << endl;
                //cout << "read poly#" << i << " side " << nearestline << " to " << fllinetex->value() << endl;
                //reading texture values for both sides
                //cout << "read texture value for poly #" << i << endl;
                //i = poly_count;
            }
    }

    set_selection();

    return found;
}

int checkpolysafe(int polyid, int id)//TODO put this in poly funcs and the header
{
    for(int i = 0; i <polys[polyid].sidecount+1;i++){
        cout << "poly is " << polyid << " side is " << polys[polyid].p[i] << " checker is " << id << endl;
        if(polys[polyid].p[i] == id){
            deletepoly(id);
            cout << "delling a poly" << endl;
            return 1;
            //polys[polyid].sidecount = -1;
            //polys[polyid].sidecount--;
            //polys[polyid].p[i]--;
        } else if (polys[polyid].p[i] > id) {
            polys[polyid].p[i]--;
        }
    }
    return 0;
    if(polys[polyid].sidecount  == -1){
            //deletepoly(id);
    }
}
int checkpolysafea(int polyid, int id)//TODO put this in poly funcs and the header
{
    for(int i = 0; i <polys[polyid].sidecount+1;i++){
        //cout << "poly is " << polyid << " side is " << polys[polyid].p[i] << " checker is " << id << endl;
        if(polys[polyid].p[i] == id){
            cout << "delling a poly" << endl;
        cout << "poly is " << polyid << " side is " << polys[polyid].p[i] << " checker is " << id << endl;
            deletepoly(id);
            return 0;
        }
    }
    return 0;
}
int checkpolysafeb(int polyid, int id)//TODO put this in poly funcs and the header
{
    for(int i = 0; i <polys[polyid].sidecount+1;i++){
        if (polys[polyid].p[i] > id) {
            polys[polyid].p[i]--;
        }
    }
    return 0;
}

void deleteline(int id)
{
    /*for (int i = 0; i < poly_count;i++){
        i-=checkpolysafe(i, id);
    }*/
/* for(int r = 0; r < 20;r++)
    for (int i = 0; i < poly_count;i++){
        checkpolysafea(i, id);
    }*/
    for (int i = 0; i < poly_count;i++){
        checkpolysafeb(i, id);
    }
    lines.erase(lines.begin()+id);
    line_count--;
}

/*
bool add_line(float x, float y, line_parts t)
{
    //check for vertex
    int checkedvert = nearest_vert(vert_count, x, y);
    if(checkedvert == -1)
        return 0;

        line templine;
    switch(t)
    {
        case PART_A:
        lines.push_back(templine);
        lines[line_count].a = checkedvert;
        return 1;
        case PART_B:
        lines[line_count].b = checkedvert;
        line_count++;
        return 1;
        case A_AND_B:
        lines.push_back(templine);
        lines[line_count+1].a = checkedvert;
        lines[line_count].b = checkedvert;
        line_count++;
        return 1;
        default:
        return 0;
    }
}
*/
int add_linevv(int a, int b)
{

        line templine;
        lines.push_back(templine);
    lines[line_count].a = a;
    lines[line_count].b = b;
    line_count++;
    return line_count-1;

}

int find_line_id(int a, int b)
{
    int linesfound = 0;
    int foundline = -1;
    for (int i = 0; i < line_count; i++)
    {
        if ((lines[i].a == a && lines[i].b == b) || (lines[i].a == b && lines[i].b == a))
        {
            linesfound++;
            if(linesfound > 1)
            {
                cout << "line duped at " << i << " and " << foundline << endl;
                linesfound--;
            }
            foundline = i;
        }
    }
    return foundline;
}

int get_other_line_point(int lineid, int pointid)
{
    if(lines[lineid].a == pointid)
        return lines[lineid].b;
    else
        return lines[lineid].a;
}


int find_add_line_xyv(float x, float y, int b)
{
    int a = add_vert_near(x,y);
    int myline = find_line_id(a,b);
    if(myline+1)//also could be if(myline != -1)
    {
        return myline;
    } else
    {
        return add_linevv(a,b);
        //make a new line
    }
}

int find_add_line_vv(int a, int b)
{
    int myline = find_line_id(a,b);
    if(myline+1)//also could be if(myline != -1)
    {
        return myline;
    } else
    {
        return add_linevv(a,b);
    }
}
