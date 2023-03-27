
#include "hovercad.h"

void flsetroofheight(Fl_Value_Input*o, void*)
{
    int i = 0;
    while(i<poly_count)
    {
        if(polys[i].selected == true)
        {
            polys[i].roof = (float)o->value();
        }
        i++;
    }
}
void flsetfloorheight(Fl_Value_Input*o, void*)
{

    int i = 0;
    while(i<poly_count)
    {
        if(polys[i].selected == true)
        {
            polys[i].floor = (float)o->value();
        }
        i++;
    }
}
void flsetvertx(Fl_Value_Input*o, void*)
{
    int i = 0;
    while(i<vert_count)
    {
        if(verts[i].selected == true)
        {
            verts[i].x = (float)o->value();
        }
        i++;
    }
}
void flsetverty(Fl_Value_Input*o, void*)
{
    int i = 0;
    while(i<vert_count)
    {
        if(verts[i].selected == true)
        {
            verts[i].y = (float)o->value()-1;
        }
        i++;
    }
}

void flsetvertz(Fl_Value_Input*, void*){}

void flsetrooftex(Fl_Choice*o, void*)
{
    int i = 0;
    while(i<poly_count)
    {
        if(polys[i].selected == true)
        {
            polys[i].rooftex = (int)o->value()-1;
        }
        i++;
    }
}
void flsetfloortex(Fl_Choice*o, void*)
{
    int i = 0;
    while(i<poly_count)
    {
        if(polys[i].selected == true)
        {
            polys[i].floortex = (int)o->value()-1;
        }
        i++;
    }
}
void flsetlinetex(Fl_Choice*o, void*)   {

    int selectedlines[31];
    selectedlines[0] = -1;
    int linecount = 0;

    //find selected lines
    for(int i = 0; i < line_count; i++) {
        if(lines[i].selected) {
            selectedlines[linecount] = i;
            selectedlines[linecount+1] = -1;
            linecount++;
        }
    }

    //set the textures for all selected lines
    for(int j = 0; selectedlines[j] != -1; j++) {
        for(int i = 0; i < poly_count; i++) {
            //find the side of the poly that need to be changed with the selected line
            int polylinenumber = find_line_polyside(i, selectedlines[j]);
                if(polylinenumber != -1) {
                    polys[i].tex[polylinenumber] = (int)o->value()-1;
                    cout << "set poly#" << i << " side "
                    << polylinenumber << " to " << polys[i].tex[polylinenumber] << endl;

                }
        }
    }

}

void flsetverttype(Fl_Choice*o, void*) {
    for(int i = 0; i < vert_count; i++) {
        if(verts[i].type != 0 && verts[i].selected == true)   {
            int converter = (int)(o->value()+1);
            verts[i].type = (vert_type)converter;
        }
    }
}

int CheckVertSelectionType() {
    return (int)flverttype->value()+1;
}

void resetlvl(){
    /*
    starts.clear();
    checks.clear();
    finishes.clear();
    */
    verts.clear();
    lines.clear();
    polys.clear();
    vert_count = 0;
    line_count = 0;
    poly_count = 0;
}

void flload(Fl_Menu_*, void*)   {

    filename = fl_file_chooser("load level", "*.xml", "", 1);

    if(filename){
        resetlvl();
        loadxmlfile(filename);
        //readhcufile(filename);
    }

}


void flsave(Fl_Menu_*, void*)
{
    if(filename)
        savexmlfile(filename);
}
void flsaveas(Fl_Menu_*, void*)
{
    filename = fl_file_chooser("save level as", "*.xml", "", 1);

    if(filename)
        savexmlfile(filename);
}
void flreset(Fl_Menu_*, void*)
{
    resetlvl();
}
void flexit(Fl_Menu_*, void*)
{
    window->hide();//this quits!
}

void opentexturewin(Fl_Menu_*, void*)
{
    texwindow->show();
}

void flnewtex(Fl_Button*, void*){
    char *texfilename;
    texfilename = fl_file_chooser("open texture", "Image Files (*.{bmp,gif,jpg,png})", "", 1);
    cout << texfilename << endl;

    if(texfilename)
        AddTexture(texfilename);
}
void fldeltex(Fl_Button*, void*){
    DeleteTexture(fltexlist->value());
}

//go in to room creation mode
void flsetpolymode(Fl_Button*o, void*) {
    //turn off the vert type selection
    flverttype->deactivate();

    o->value(1);
    flobjmode->value(0);
    editmode = POLY;
}

//go into object creation mode
void flsetobjmode(Fl_Button*o, void*) {
    //turn on the vert type selection
    flverttype->activate();

    o->value(1);
    flpolymode->value(0);
    editmode = OBJECT;

}

//go through all the verts and assign value of the dial
//to verts that are both seleted and start points
void flSetVertDir (Fl_Dial*o, void*) {
    for(int i = 0; i < vert_count; i++) {
        if(verts[i].selected == true && verts[i].type == START_POINT) {
            verts[i].VertDirection = o->value();
        }

    }
}
