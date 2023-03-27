#ifndef ASDASD
#define ADSASD

#include <iostream>
#include <vector>
#include <string>
using namespace std;
#include "UI.h"

#include <FL/Fl.H>
#include <FL/gl.h>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Dial.H>
#include <FL/filename.H>

#include "tinerxml.h"
#include "math.h"

#define PI 3.14159265
#define MAX_SIDES 30//TODO replace this with dynamicness, dont wanna waste mem or be limited ot 30 sides a poly do we!

#define mode_movecam -1
#define mode_makepoly 0
#define mode_selectmove 1
#define mode_selectvert 2
#define mode_selectline 3
#define mode_selectpoly 4

#define LMB 1
#define MMB 2
#define RMB 3


extern Fl_Double_Window *window;
extern Fl_Double_Window *texwindow;

//vars
extern int vert_count;
extern int line_count;
extern int poly_count;
extern int side_count;
extern char *filename;
extern bool selection;


extern int TextureListCount;
extern string TextureList[255];

struct camstruct
{
    float x,y,z;
};

struct mousestruct
{
    float x,y;
    float sx,sy;
    bool button[5];
};

//enumerations
enum edit_mode{
    POLY = 0,
    OBJECT = 1
};

enum vert_type{
    NORMAL_VERT = 0,
    START_POINT = 1,
    CHECK_POINT = 2,
    START_FINISH = 3
};

//classes
class vert  {
public:
    float x, y;
    bool selected;
    bool condemed;
    bool red;
    float VertDirection;
    vert_type type;

    vert()  {
        x = -1;
        y = -1;
        VertDirection = 0.0f;
        selected = false;
        condemed = false;
        type = NORMAL_VERT;
    }
};

class line  {
public:
    int a, b;
    bool selected;
    bool condemed;
    bool red;

    line()
    {
        //so i can tell these have never been used
        a = -1;
        b = -1;
        selected = false;
        condemed = false;
        red = false;
    }
    bool containsvert(int v){
      if(a == v || b == v) return 1;
      return 0;
    }
};

class poly  {
public:
    vector<int> p;
    vector<int> tex;
    int sidecount;
    bool complete;
    bool selected;
    bool condemed;

    float floor;
    float roof;
    int floortex;
    int rooftex;

    poly()
    {
        floor = 0.f;
        roof = 4.f;
        floortex = 0;
        rooftex = -1;
        complete = false;
        selected = false;
        condemed = false;
        sidecount = 0;

        int i = 0;
        while(i < MAX_SIDES)
        {
            //p[i] = -1;
            p.push_back(-1);
            tex.push_back(0);
            //tex[i] = 0;
            i++;
        }
    }
};



extern vector<vert> starts;
extern vector<vert> finishes;
extern vector<vert> checks;

extern vector<vert> verts;
extern vector<line> lines;
extern vector<poly> polys;

extern int editmode;
extern bool importinghcu;

extern camstruct cam;
extern mousestruct mouse;


//functions
void readhcufile(char *filename);
int CheckVertSelectionType();

//texture
void AddTexture(char* texname);
void DeleteTexture(int id);
int FindTexture(char *tex);
void SetupTextures();
void ClearTextureList();

//misc
bool add_vertex(float x, float y);
bool add_object(float x, float y);
void addstart(float x,float y);
void deletevert(int id);
void SetupObjects();

//bool add_line(float x, float y, line_parts t);
int add_linevv(int a, int b);
int add_vert_near(float x, float y);

void deleteline(int id);
int find_add_line_vv(int a, int b);
int find_add_line_xyv(float x, float y, int b);
int find_line_polyside(int p, int l);

int find_line_id(int a, int b);
int get_other_line_point(int lineid, int pointid);

bool move_verts(float x, float y);

void clear_selection();
bool set_selection();

bool select_object(float x, float y);

bool select_vert(float x, float y);
int nearest_vert(float x, float y, float maxdist=flvertsize->value()*cam.z);
int FindVertex(float x, float y);

bool select_line(float x, float y);
float disttoline(float x, float y, int lineid);

int select_poly(float x, float y);
int pnpoly(float x, float y, int polyid);
void deletepoly(int i);

void assemble_poly(float x, float y, bool close = 0);

//drawing
void glCircle(float x, float y, float, int , int numsides=15);
void ExtDraw();
void drawvertexs();
void drawlines();
void drawpolys();

//input
void MouseBtn(bool press, int button, float x, float y);
void MouseMove(float x, float y);
void MouseWheel(int x, int y);
void KeyPress(bool press, int ascii);

//xml
int loadxmlfile(char*filename);
int savexmlfile(char*filename);


#endif
