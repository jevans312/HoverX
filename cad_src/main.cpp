#include "hovercad.h"


/* ----------> TODO:
-add filename to the title bar

*/


Fl_Double_Window *window;
Fl_Double_Window *texwindow;

camstruct cam;

mousestruct mouse;

char *filename = "hoverlvl.xml";

//global vars
int vert_count = 0;
int line_count = 0;
int poly_count = 0;
int side_count = 0;
int TextureListCount = 0;

bool selection = false;

int mousemode[] =
    {
        666,0,-1,1//mouse buttons are 1, 2 and 3, 0 is not a button, so i mad it 666
    };

int editmode = 0;

//functions
int find_vertex(int vertcount, float x, float y);

/*
vector<vert> starts;
vector<vert> finishes;
vector<vert> checks;
*/
//vertex storage
vector<vert> verts;
vector<line> lines;
vector<poly> polys;

/*
void addstart(float x,float y)
{
    vert newpoint;
    newpoint.x = x;
    newpoint.y = y;
    starts.push_back(newpoint);
}
void addfinish(float x,float y)
{
    vert newpoint;
    newpoint.x = x;
    newpoint.y = y;
    finishes.push_back(newpoint);
}
void addcheck(float x,float y)
{
    vert newpoint;
    newpoint.x = x;
    newpoint.y = y;
    checks.push_back(newpoint);
}
*/

void clear_selection()  {
    int i = 0;
    while (i<vert_count)
    {
        verts[i].selected = false;
        i++;
    }
    i = 0;
    while (i<line_count)
    {
        lines[i].selected = false;
        i++;
    }
    i = 0;
    while (i<poly_count)
    {
        polys[i].selected = false;
        i++;
    }
}

bool set_selection()
{
    bool got_a_poly = 0;
    int i = 0;
    int j = 0;
    while (i<vert_count)
    {
        if(verts[i].selected)
        {
            selection = true;
            //return 1;
        }
        i++;
    }
    i = 0;
    while (i<line_count)
    {
        if(lines[i].selected)
        {
            selection = true;
            //return 1;
        }
        i++;
    }
    i = 0;
    while (i<poly_count)
    {
        if(polys[i].selected)
        {
            //select the verts in the poly
            j = 0;
            while(polys[i].p[j] != -1)
            {
                verts[lines[polys[i].p[j]].a].selected = true;
                verts[lines[polys[i].p[j]].b].selected = true;
                //cout << verts[lines[polys[i].p[j]].b].selected << endl;
                //cout << polys[i].p[j] << endl;
                j++;
            }

            selection = true;
            //return 1;
        }
        i++;
    }
    if(selection)
            return 1;
    selection = false;
}

bool select_object(float x, float y)
{
    if(!Fl::event_state(FL_CTRL))
        clear_selection();

    if(select_vert(x,y)) return 1;

    if(select_line(x,y)) return 1;

    if(select_poly(x,y)) return 1;

    return 0;
}


bool delete_selection() {
    //delete select polys
    for (int i = 0; i < poly_count;i++){
      if(polys[i].selected){
        deletepoly(i);
        i--;
      }
    }
    //mark all lines for removal
    for (int i = 0; i < line_count;i++){
        lines[i].condemed = 1;
    }
    //except ones we are still using!
    for (int i = 0; i < poly_count;i++){
        for(int r = 0; r<polys[i].sidecount+1;r++){
            lines[polys[i].p[r]].condemed = 0;
        }
    }
    //delete marked lines
    for (int i = 0; i < line_count;i++){
      if(lines[i].condemed){
        deleteline(i);
        i--;
      }
    }
    //mark all verts for removal
    for (int i = 0; i < vert_count;i++){
        verts[i].condemed = 1;

    }
    //err, except ones we use!
    for (int i = 0; i < line_count;i++){
            verts[lines[i].a].condemed = 0;
            verts[lines[i].b].condemed = 0;
    }
    //remove marked verts
    for (int i = 0; i < vert_count;i++){
        if(verts[i].condemed == 1 && verts[i].type == NORMAL_VERT){
            deletevert(i);
            i--;
        }
        else if(verts[i].type != NORMAL_VERT && verts[i].selected == true) {
            deletevert(i);
            i--;
        }

    }
}

int main() {
//setup ui
    window = make_window();
    window->resizable(window);
    texwindow = make_texture_window();
    texwindow->resizable(texwindow);
    window->show();

    DrawingBox->show();
    cam.x,cam.y = 0;
    cam.z = 1;

    SetupObjects();         //load objects into menus
    SetupTextures();        //load textures into menus

    flpolymode->value(1);    //set the poly mode button down
    editmode = POLY;        //set the edit mode to poly creation
    flverttype->deactivate();   //turn off vert type selection
    VertDir->deactivate(); //turn off direction control
//end setup ui

    while(window->shown()) {
        //do main loop update here
        Fl::wait(1);
        DrawingBox->redraw();
    }
    return 0;
}

bool inbound(float x,float y, float x1,float y1,float x2,float y2){
if((x>x1 && x < x2) || (x>x2 && x < x1))
if((y>y1 && y < y2) || (y>y2 && y < y1))
return 1;
return 0;
}

float rx,ry;
void rectselectstart(float x, float y)
{
    rx = x;
    ry = y;
}
void rectselectend(float x, float y)
{
    for (int i = 0; i < vert_count;i++){
        if (inbound(verts[i].x,verts[i].y,x,y,rx,ry))verts[i].selected = 1;
    }
    //loop through all verts, and check if bounded by rx,x,y,ry
    //allow for selection box to be flipped!

}

//IRREGULAR SELECT PLAN
/*
basicly, the mouse is clicked, a point is created(in a list or vector)
the mouse is dragged, with button held down, if distance
between mouse pos, and last point > 30(?) a new point is made
mouse is released - final point may be auto created as dupe of first?

for each vert, perhaps opt to that area -  a pnpoly check is done, if its in the poly, it gets selected!
simple XD
though, the pnpoly will prob have to be redone a little, instead of refrencing point[line[i].a].x(or y) and point[line[i].b]].x(or y)
it should be point[i].x(or y) and point[i+1].x(or y)
*/

//SCALETOOL!
//scale by multiplying dist to camera pos?

//what else?

void MouseBtn(bool press, int button, float x, float y)
{
    x = x-DrawingBox->w()/2;
    y = -y+DrawingBox->h()/2;
    x*=cam.z;
    y*=cam.z;
    mouse.sx=x;
    mouse.sy=y;

    mouse.x = x-cam.x;
    mouse.y = y-cam.y;

    mouse.button[button] = press;

    /*#define mode_movecam -1
      #define mode_makepoly 0
      #define mode_selectmove 1
      #define mode_selectvert 2
      #define mode_selectline 3
      #define mode_selectpoly 4*/

    /*
    button 1 will create polys if there is no selection
    button 1 will move objects if there is a selection//disabled for the min for the sake of tidyer code
    button 3 will select a object
    button 3 + cttl will select multiple objects
    button 2 will control zoom only
    */

    if(press){
        switch(mousemode[button])
        {
            case 0:
                if(editmode == POLY)    assemble_poly(mouse.x, mouse.y);
                if(editmode == OBJECT)  add_object(mouse.x, mouse.y);
            break;
            case 1:
            select_object(mouse.x, mouse.y);
            break;
            case 2:
            break;
            case 3:
            break;
            case 4:
            break;
            case 5:
            //addstart(mouse.x,mouse.y);
            break;
            case 6:
            //addcheck(mouse.x,mouse.y);
            break;
            case 7:
            //addfinish(mouse.x,mouse.y);
            break;
            case 8:
            //rectselectstart(mouse.x,mouse.y);
            break;
            default:
            break;
        }
    }
    else
    {
        switch(mousemode[button])
        {
            case 8:
            rectselectend(mouse.x,mouse.y);
            break;
            default:
            break;
        }

        }
}
void MouseMove(float x, float y)
{
    x = x-DrawingBox->w()/2;
    y = -y+DrawingBox->h()/2;
    x*=cam.z;
    y*=cam.z;
    if(mouse.button[MMB])
    {
        cam.x-= (mouse.sx-x);
        cam.y-=(mouse.sy-y);
    }
    if((mouse.button[RMB] && mousemode[RMB] == 1) || (mousemode[LMB] == 1 && mouse.button[LMB]))
    {
        //move_verts(verts[selected_vert].x-(mouse.sx-x), verts[selected_vert].y-(mouse.sy-y));
        move_verts(mouse.sx-x, mouse.sy-y);
    }

    mouse.sx=x;
    mouse.sy=y;

    mouse.x = x-cam.x;
    mouse.y = y-cam.y;

    /*if(mouse.button[3])
        move_vert(mouse.x, mouse.y);*/

}
void MouseWheel(int x, int y)
{
    cam.z+=y/10.f;
}
void KeyPress(bool press, int ascii)
{
    //cout << "butt pressed: " << ascii << " fl del is " << FL_Delete << endl;
    if (ascii == FL_Delete) delete_selection();
        /*for (int i = 0; i < vert_count;i++)
        {
            if (verts[i].selected)
            {
                deletevert(i);
                i--;
            }
        }*/

    ascii -= '0';
    int cmb = 0;
    if(Fl::event_state(FL_CTRL))
        cmb = 1;
    if(Fl::event_state(FL_SHIFT))
        cmb = 3;
    if(ascii >= 0 && ascii <= 9)
        mousemode[cmb] = ascii;
    ascii += '0';

}

void ExtUpdate()
{

    if (cam.z < 0.1)
        cam.z = 0.1;

    /*    flmx->value(mouse.x);//i agree, mouse pos not needed
        flmy->value(mouse.y);

        flcamx->value(cam.x);// i would like campos as an input tbh
        flcamy->value(cam.y);
        flcamz->value(cam.z);*/

    //mousemode[1] = (int)fllmb->value();//this would be good ti bring back, but using buttons or summin, not input box :)
    //mousemode[3] = (int)flrmb->value();

    /*if(selected_vert != -1 && vert_moved)
    {
    flvertx->value(verts[selected_vert].x);
    flverty->value(verts[selected_vert].y);
    vert_moved = 0;
    }
    else
    {
    verts[selected_vert].x = (float)flvertx->value();
    verts[selected_vert].y = (float)flverty->value();
    }*/
}
