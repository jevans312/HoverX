#ifndef ui_h
#define ui_h

#include "glex.h"
#include "glness.h"
#include "font.h"
#include <string>
#include <iostream>
using namespace std;

#define MAX_BUTTONS 23
#define MAX_DROPDOWNS 23
#define MAX_TEXTURES 23

//classes
/*
class UITexture {
    public:

    string Name[MAX_TEXTURES];
    int X;
    int Y;
    int Address[MAX_TEXTURES];
    int Count;

    //functions
    void LoadUITexture(string filename, string name);

    UITexture ( void ) {
        Count = 0;
    }

};
*/

class UIButton {
    public:

    //variables
    //top left
    float tl_x;
    float tl_y;

    //bottom right
    float br_x;
    float br_y;

    string label;
    string command;
    string arg;

    bool Draw;

    //functions
    UIButton( void )    {
        tl_x = 0;
        tl_y = 0;
        br_x = 0;
        br_y = 0;
        label = "";
        command = "";
        arg = "";
        Draw = false;
    }
};

class ButtonArray {     //this should prob be called a button list
    public:
    short unsigned int Buttons[15];   //16 max buttons per array
    int ButtonCount;
    string Name;
    bool Draw;

    ButtonArray( void )    {
        Name = "";
        ButtonCount = 0;
        Draw = false;
    }
};

//variables
extern int window_width;    //window size
extern int window_height;
extern int DesktopTexture;

//functions
void UI_AddMSG(const string& tMSG);
int UIAddButton(float x, float y, const string &name, const string &command, const string &arg);

void CreateMapDropdown();
void CreateMainMenu();
void CreateSectorsMenu();
void CreateSectorListMenu();

void DisplayMapDropdown(bool tDraw);
void DisplayMainMenuDropdown(bool tDraw);
void DisplaySectorsDropdown(bool tDraw);
void DisplaySectorsListDropdown(bool tDraw);

void UI_DrawButtons();
void UI_Setup();
void UI_DrawMSGs();         //draw command line, messages from users and the system, and frames per second for some reason
void UI_DrawDesktop();      //background image
void UI_DrawLapTimes();
void UI_DrawUserList();     //list of other users while in the lobby
void UIMousePress( float mousex, float mousey );

#endif
