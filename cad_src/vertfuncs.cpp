#include "hovercad.h"


int nearest_vert(float x, float y, float maxdist)
{
    int i = -1;
    int nearestpoint = -1;

    while(++i < vert_count)
    {
        float td = hypot(x-verts[i].x,y-verts[i].y);
        if(td<maxdist)
        {
            maxdist = td;
            //cout << maxdist << endl;
            nearestpoint = i;
        }
    }
    //cout << "x is " << x << "   y is " << y << endl;
    //cout << "nearest point is " << nearestpoint << endl;
    return nearestpoint;

}

void checklinesafea(int lineid, int id)//TODO put this in line funcs and the header
{
        if(lines[lineid].containsvert(id)){
            deleteline(lineid);
            cout << "delling a line no " << lineid <<  endl;
        }
}

void checklinesafeb(int lineid, int id)//TODO put this in line funcs and the header
{
        if(lines[lineid].a > id){
            lines[lineid].a--;
            //cout << "line--" << endl;
        }
        if(lines[lineid].b > id){
            lines[lineid].b--;
            //cout << "line--" << endl;
        }
}

void deletevert(int id) {
 //vector<line> linei;
 /*for(int r = 0; r < 20;r++)
   for (int i = 0; i < line_count;i++){
        checklinesafea(i, id);
    }*/
   for (int i = 0; i < line_count;i++){
        checklinesafeb(i, id);
    }
    verts.erase(verts.begin()+id);
    vert_count--;
}

void SetupObjects() {
    //fill the vert type menu
    flverttype->add("Spawn Point");
    flverttype->add("Check Point");
    flverttype->add("Start,Finish Line");
    flverttype->add("3d object");
    flverttype->value(0);
}

bool add_object(float x, float y) {
    vert tempvert;
    verts.push_back(tempvert);

    //add to list
    verts[vert_count].x = x;
    verts[vert_count].y = y;
    verts[vert_count].type = (vert_type)CheckVertSelectionType();

    vert_count++;

    return 1;
}

bool add_vertex(float x, float y)
{

    vert tempvert;
    verts.push_back(tempvert);

    //add to list
    verts[vert_count].x = x;
    verts[vert_count].y = y;
    verts[vert_count].type = NORMAL_VERT;

    vert_count++;

    return 1;
}


bool select_vert(float x, float y)
{
    int i = 0;
    int nearvert;
    bool found = false;

    nearvert = nearest_vert(x, y);
    if(nearvert == -1)
        return 0;

    //vert_moved = 1;

    if(verts[nearvert].selected)
    {
        verts[nearvert].selected = false;
        found = true;
    }
    else
    {
        verts[nearvert].selected = true;
        found = true;

            flvertx->value(verts[nearvert].x) ;
            flverty->value(verts[nearvert].y);
    }

    //should have a fucntion 'selected_verts_objects()'
    //update the vert type and activate the selection
    if(verts[nearvert].type != NORMAL_VERT) {
        flverttype->activate();
        flverttype->value((int)verts[nearvert].type-1);
    } else {    //else make grey and unusable
        flverttype->deactivate();
    }

    //turn on or off the direction control
    if(verts[nearvert].type == START_POINT) {
        VertDir->activate();
        VertDir->value(verts[nearvert].VertDirection);  //assign value to the ui control
    } else {
        VertDir->deactivate();
    }

    set_selection();
    return found;
}

bool move_verts(float x, float y)
{
    int i = 0;

    while(i<vert_count)
    {
        if(verts[i].selected == true)
        {
            /* verts[i].x += verts[i].x + (x - verts[i].x);
             verts[i].y += verts[i].y + (y - verts[i].y);*/
            verts[i].x -= verts[i].x - (verts[i].x - x);
            verts[i].y -= verts[i].y - (verts[i].y - y);

        }
        i++;

    }
    return 1;
}


int add_vert_near(float x, float y)
{
    int found_vert;
    //if(importinghcu){
    //    found_vert = nearest_vert(x, y,0.1);
    //} else {
        found_vert = nearest_vert(x, y);
    //}





    if( found_vert == -1 )
    {
        add_vertex(x,y);
        return vert_count-1;
    }
    else
    {
        return found_vert;
    }
}

//search for existing vert and return its ID or -1 if none exist
//not sure if this even works
int FindVertex(float x, float y) {
    for(int i; i < vert_count; i++) {
        if(verts[i].x == x && verts[i].y == y) {
            cout << "found vert#" << i << endl;
            return i;
        }

    }

    return -1;
}
