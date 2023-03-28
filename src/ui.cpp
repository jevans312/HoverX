#include "ui.h"
#include "server.h"
#include "client.h"
#include "level.h"

extern level mylvl;    //the one true world
extern ServerClass hxServer;
extern LocalClient LC;

//game states
extern string hostMSGbuffer;
//extern bool ingame;
extern bool MSGmode;
extern bool isConnected;
extern bool isHost;
extern bool done;
extern int DrawDeltaTime;

//0 Map list
ButtonArray Dropdownmenus[MAX_DROPDOWNS];

UIButton ButtonList[MAX_BUTTONS];
//UITexture TextureList;
unsigned short int UIButtonCount;

string MSGbuffer0;
string MSGbuffer1;
string MSGbuffer2;
string MSGbuffer3;

/*
UIButton::UIButton( void )    {
    tl_x, tl_y, br_x, br_y = 0;
    label = "";
    command = "";
    arg = "";
    Draw = false;
}

ButtonArray::ButtonArray( void )    {
    Name = "";
    ButtonCount = 0;
    Draw = false;
}
*/

void UI_Setup() {
    MSGbuffer0 = "";
    MSGbuffer1 = "";
    MSGbuffer2 = "";
    MSGbuffer3 = "";

    UIButtonCount = 0;

    UIAddButton(0.f, 1, "Menu", "mainmenu", "");
    UIAddButton(0.9, 1, "Exit", "exit", "");    //.9 cuz there are 4 chars...
    CreateMapDropdown();
    CreateMainMenu();
    CreateSectorsMenu();
    CreateSectorListMenu();

    //TextureList.LoadUITexture("img/1.png", "1");
}

void UI_AddMSG(const string& tMSG) {
    string newMSG0;
    string newMSG1;
    string newMSG2;
    string newMSG3;

    //setup new message buffer
    newMSG0 = tMSG;
    newMSG1 = LC.MSGDrawBuffer0;
    newMSG2 = LC.MSGDrawBuffer1;
    newMSG3 = LC.MSGDrawBuffer2;

    //assign new buffer
    LC.MSGDrawBuffer0 = newMSG0;
    LC.MSGDrawBuffer1 = newMSG1;
    LC.MSGDrawBuffer2 = newMSG2;
    LC.MSGDrawBuffer3 = newMSG3;
}

void UI_DrawDesktop() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    go2d(0.f,1.f,0.f,1.f);
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, DesktopTexture);

    glBegin(GL_QUADS);
        glTexCoord2f(0, 1);
        glVertex3f( 0, 0, 0.f );    //left bottom
        glTexCoord2f(1, 1);
        glVertex3f( 1, 0, 0.f );    //right bottom
        glTexCoord2f(1, 0);
        glVertex3f( 1, 1, 0.f );    //right top
        glTexCoord2f(0, 0);
        glVertex3f( 0, 1, 0.f );    //left top
    glEnd();

/*    glBindTexture(GL_TEXTURE_2D, numberonetexture);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 1);
        glVertex3f( 0.4, 0.4, 0.f );    //left bottom
        glTexCoord2f(1, 1);
        glVertex3f( 0.6, 0.4, 0.f );    //right bottom
        glTexCoord2f(1, 0);
        glVertex3f( 0.6, 0.6, 0.f );    //right top
        glTexCoord2f(0, 0);
        glVertex3f( 0.4, 0.6, 0.f );    //left top
    glEnd();
*/

    glPopMatrix();
    leave2d();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    if(LC.isConnectedToRemoteServer) {
        UI_DrawUserList();
    }
}

void UI_DrawUserList() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    go2d(0.f,1.f,0.f,1.f);
    glPushMatrix();
        glColor4f(.32, .55, .64, 0.5);     //set drawing color to hxgreen and 50% alpha

        glBegin(GL_QUADS);
            glVertex3f( 0.70, 0.15, 0.f );    //left bottom
            glVertex3f( 0.95, 0.15, 0.f );    //right bottom
            glVertex3f( 0.95, 0.90, 0.f );    //right top
            glVertex3f( 0.70, 0.90, 0.f );    //left top
        glEnd();

        glEnable(GL_TEXTURE_2D);    //enable texturing for fonts

        //user list
        glColor3f(0, 0, 0); //draw black
        printgls(0.70, 0.90, (char*)"%s", "Users");

        glColor3f(1, 1, 1); //draw white
        float ywriteposition = 0.85;
        for(int i = 0; i < MAXCLIENTS; i++) {
            if(LC.Clients[i].Name != "unnamed") {
                printgls(0.70, ywriteposition, (char*)"%s", LC.Clients[i].Name.c_str());
                ywriteposition = ywriteposition - .05;
            }
        }

    glPopMatrix();
    leave2d();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void UI_DrawMSGs() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    go2d(0.f,1.f,0.f,1.f);
    glPushMatrix();
        //glTranslatef(0.025, 0.025 , 0);     //move everything

        if(MSGmode) {   //draw a box to indicate message mode
            glColor3f(0.5, 0.5, 0.5);     //set drawing color to gray

            glBegin(GL_QUADS);
                glVertex3f( 0.025, 0.025, 0.f );    //left bottom
                glVertex3f( 0.975, 0.025, 0.f );    //right bottom
                glVertex3f( 0.975, 0.050, 0.f );    //right top
                glVertex3f( 0.025, 0.050, 0.f );      //left top
            glEnd();
        }

        glColor3f(1, 1, 1);
        glEnable(GL_TEXTURE_2D);    //enable texturing for fonts

        //draw message line
        if(MSGmode) printgls(0.025, 0.050, (char*)"%s", hostMSGbuffer.c_str());

        //draw messages
        printgls(0.025, 0.075, (char*)"%s", LC.MSGDrawBuffer0.c_str());
        printgls(0.025, 0.100, (char*)"%s", LC.MSGDrawBuffer1.c_str());
        printgls(0.025, 0.125, (char*)"%s", LC.MSGDrawBuffer2.c_str());
        printgls(0.025, 0.150, (char*)"%s", LC.MSGDrawBuffer3.c_str());

        //draw fps
        glColor3f(0, 0, 0);
        //cout << "DrawDeltaTime: " << DrawDeltaTime;
        if(DrawDeltaTime < 1) DrawDeltaTime = 1;
        printgls(0.825, 0.025, (char*)"FPS %i", 1000/DrawDeltaTime);
        //cout << " FPS: " << (1000/DrawDeltaTime) << '\n';

    glPopMatrix();
    leave2d();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void UIMousePress( float mousex, float mousey ) {
    float mouse_x = 0;
    float mouse_y = 0;
    int ButtonToExecute = -1;
    //convert mouse data to something i can use

    //normalize data first
    mousex = mousex/window_width;
    mousey = mousey/window_height;

    //convert cordinates to match the gui system
    mouse_x = mousex;
    mouse_y = 1 - mousey;
    //cout << "x: " << mousex << "y: " << mousey << endl;

    //find button that was pressed if there is one
    for(int i = 0; i < UIButtonCount; i++) {
        //check to see if the click is in the area of the button and set it to be executed
        if( ( (ButtonList[i].tl_x < mouse_x) && (mouse_x < ButtonList[i].br_x) ) && ( (ButtonList[i].tl_y > mouse_y) && (mouse_y > ButtonList[i].br_y) ) )  {
            if(ButtonList[i].Draw == true) {    //if its visable
                ButtonToExecute = i;
                i = UIButtonCount;  //end the loop
            }
        }
    }

    //execute the found button if one was found
    //if(ButtonToExecute != -1)   UIExecuteCommand(ButtonList[ButtonToExecute].command, ButtonList[ButtonToExecute].arg);
    if(ButtonToExecute != -1) {
        string newmsg = "/" + ButtonList[ButtonToExecute].command + " " + ButtonList[ButtonToExecute].arg;
        LC.AddTextMessage(newmsg);
    }
    else {  //close dropdowns
        DisplayMapDropdown(false);
        DisplaySectorsDropdown(false);
        DisplaySectorsListDropdown(false);
        DisplayMainMenuDropdown(false);

    }
}

//locate button in button array and return its address
int UIFindButtonByName(string nametofind) {
    int returnval = -1;

    for(int i = 0; i < MAX_BUTTONS; i++) {
        if(ButtonList[i].label == nametofind) returnval = i;
    }

    return returnval;
}

int UIAddButton(float x, float y, const string &name, const string &command, const string &arg) {
    //check that there are not to many buttons
    if(UIButtonCount+1 > MAX_BUTTONS) {
        cout << "UIAddButton: tried to add too many buttons, \"" << name << "\" not added" << endl;
        return -1;
    }

    int found_button = -1;

    //find empty spot for our new button
    for(int i = 0; i < MAX_BUTTONS; i++) {
        //found button
        if(ButtonList[i].label == "") {
            found_button = i;
            i = MAX_BUTTONS;
        }
    }
    if(found_button == -1) {
        cout << "UIAddButton: button not assigned" << endl;
        return -1;
    }

    //set the top left position
    ButtonList[found_button].tl_x = x;
    ButtonList[found_button].tl_y = y;

    //set bottom right position
    ButtonList[found_button].br_x = x + (name.size() * .025);
    ButtonList[found_button].br_y = (y - .025);

    ButtonList[found_button].label = name;
    ButtonList[found_button].command = command;
    ButtonList[found_button].arg = arg;

    ButtonList[found_button].Draw = true;

    UIButtonCount++;
    return found_button;
}

void UI_DrawButtons() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    go2d(0.f,1.f,0.f,1.f);
    glPushMatrix();
        //glTranslatef(0.0, 0.0 , 0);     //move everything for the fonts to be in the right place

        glColor3f(.32, .55, .64);     //set drawing color to the hx color

        //draw box for the button
        for(int i = 0; i < UIButtonCount; i++) {
            if(ButtonList[i].Draw == true) {    //only draw if we want to
                glBegin(GL_QUADS);
                    glVertex3f( ButtonList[i].tl_x, ButtonList[i].tl_y, 0.f );  //top left
                    glVertex3f( ButtonList[i].tl_x, ButtonList[i].br_y, 0.f );  //bottom left
                    glVertex3f( ButtonList[i].br_x, ButtonList[i].br_y, 0.f );  //bottom right
                    glVertex3f( ButtonList[i].br_x, ButtonList[i].tl_y, 0.f );  //top right
                glEnd();
            }
        }

        glColor3f(1, 1, 1);     //set drawing color to white
        glEnable(GL_TEXTURE_2D);    //enable textures for the font

        //draw labels on the buttons
        for(int i = 0; i < UIButtonCount; i++) {
            if(ButtonList[i].Draw == true) {
                printgls(ButtonList[i].tl_x, ButtonList[i].tl_y, (char*)"%s", ButtonList[i].label.c_str());
            }
        }

    glPopMatrix();
    leave2d();

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void UI_DrawLapTimes() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    go2d(0.f,1.f,0.f,1.f);
    glPushMatrix();
        glColor3f(1, 1, 1);
        int entaddress = LC.EntityAddress;
        int besttime = LC.lvl.Ent[entaddress].BestLapTime;

        string bestlapstr = "Best Time: " + PrettyPrintTime(besttime);
        float besttimexpos = 1 - ( 0.025 * bestlapstr.length() );
        printgls(besttimexpos, 0.950, (char*)"%s", bestlapstr.c_str() );

//        printgls(0.0, 0.975,"LAP TIME: %f", ((SDL_GetTicks()-mylvl.dude[0].lastlaptime)/1000.f));

//        printgls(0.0, 0.950,"LAP: #%i", mylvl.dude[0].currentlap);

//        if(mylvl.dude[0].currentlap > 1)  printgls(0.0, 0.925,"BESTLAP: %f", mylvl.dude[0].bestlaptime/1000.f);
//        else  printgls(0.0, 0.925,"BESTLAP: NA");

        //static int framecount = 1;
        //framecount++;
        //printgls(0.0, 0.00,"fps is %i", 1000/(SDL_GetTicks()/framecount));

        /*
        //printgls(0.0, 0.05,"time is %f", (SDL_GetTicks()/1000.f));
        //printgls(0.0, 0.05,"time is %f", (globaltime/1000.f));
        printgls(0.0, 0.075,"LAP time is %f", ((SDL_GetTicks()-mylvl.dude[0].lastlaptime)/1000.f));
        printgls(0.0, 0.10,"currentcheckis %i", mylvl.dude[0].currentcheckpoint);
        printgls(0.0, 0.125,"current lap is %i", mylvl.dude[0].currentlap);
        for (int pid = 0; pid < mylvl.polynum;pid++)    {
            if(mylvl.lvlpoly[pid].pnpoly(mylvl.dude[0].pos.c.x, mylvl.dude[0].pos.c.y)){
                printgls(0.0, 0.15,"current poly is %i", pid);
                if(mylvl.lvlpoly[pid].ischeckpoint)
                printgls(0.0, 0.20,"poly is checkpoint %i", mylvl.lvlpoly[pid].checkpointid);
            }
        }
        */
    glPopMatrix();
    leave2d();

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

//create the main menu
void CreateMainMenu() {
    //properties
    Dropdownmenus[1].Name = "main";
    Dropdownmenus[1].Draw = false;

    //add buttons
    Dropdownmenus[1].Buttons[0] = UIAddButton(0, .975, "Go Online", "connect", "38.102.164.40");
    Dropdownmenus[1].Buttons[1] = UIAddButton(0, .950, "Sectors", "sectormenu", "");

    //disable drawing and testing of buttons until we want them
    ButtonList[Dropdownmenus[1].Buttons[0]].Draw = false;
    ButtonList[Dropdownmenus[1].Buttons[1]].Draw = false;

    //set the length of the drop down
    ButtonList[Dropdownmenus[1].Buttons[0]].br_x = .25;
    ButtonList[Dropdownmenus[1].Buttons[1]].br_x = .25;
}

void CreateSectorsMenu() {
    //properties
    Dropdownmenus[2].Name = "sectors";
    Dropdownmenus[2].Draw = false;

    //add buttons
    Dropdownmenus[2].Buttons[0] = UIAddButton(.25, .950, "New", "mapmenu", "");
    Dropdownmenus[2].Buttons[1] = UIAddButton(.25, .925, "Sectors", "openrooms", "");

    //disable drawing and testing of buttons until we want them
    ButtonList[Dropdownmenus[2].Buttons[0]].Draw = false;
    ButtonList[Dropdownmenus[2].Buttons[1]].Draw = false;

    //set the length of the drop down
    ButtonList[Dropdownmenus[2].Buttons[0]].br_x = .45;
    ButtonList[Dropdownmenus[2].Buttons[1]].br_x = .45;
}

void CreateSectorListMenu() {
    //properties
    Dropdownmenus[3].Name = "sectorslist";
    Dropdownmenus[3].Draw = false;

    //add buttons
    Dropdownmenus[3].Buttons[0] = UIAddButton(.45, .925, "(none)", "join", "0");
    Dropdownmenus[3].Buttons[1] = UIAddButton(.45, .900, "(none)", "join", "1");
    Dropdownmenus[3].Buttons[2] = UIAddButton(.45, .875, "(none)", "join", "2");
    Dropdownmenus[3].Buttons[3] = UIAddButton(.45, .850, "(none)", "join", "3");
    Dropdownmenus[3].Buttons[4] = UIAddButton(.45, .825, "(none)", "join", "4");
    Dropdownmenus[3].Buttons[5] = UIAddButton(.45, .800, "(none)", "join", "5");
    Dropdownmenus[3].Buttons[6] = UIAddButton(.45, .775, "(none)", "join", "6");

    //disable drawing and testing of buttons until we want them
    ButtonList[Dropdownmenus[3].Buttons[0]].Draw = false;
    ButtonList[Dropdownmenus[3].Buttons[1]].Draw = false;
    ButtonList[Dropdownmenus[3].Buttons[2]].Draw = false;
    ButtonList[Dropdownmenus[3].Buttons[3]].Draw = false;
    ButtonList[Dropdownmenus[3].Buttons[4]].Draw = false;
    ButtonList[Dropdownmenus[3].Buttons[5]].Draw = false;
    ButtonList[Dropdownmenus[3].Buttons[6]].Draw = false;

    //set the length of the drop down
    ButtonList[Dropdownmenus[3].Buttons[0]].br_x = .80;
    ButtonList[Dropdownmenus[3].Buttons[1]].br_x = .80;
    ButtonList[Dropdownmenus[3].Buttons[2]].br_x = .80;
    ButtonList[Dropdownmenus[3].Buttons[3]].br_x = .80;
    ButtonList[Dropdownmenus[3].Buttons[4]].br_x = .80;
    ButtonList[Dropdownmenus[3].Buttons[5]].br_x = .80;
    ButtonList[Dropdownmenus[3].Buttons[6]].br_x = .80;
}

//create a dropdown menu for of the maps
void CreateMapDropdown() {
    //set properties
    Dropdownmenus[0].Name = "maps";
    Dropdownmenus[0].Draw = false;

    //add buttons
    Dropdownmenus[0].Buttons[0] = UIAddButton(.45, .950, "alley", "createroom", "alley.xml");
    Dropdownmenus[0].Buttons[1] = UIAddButton(.45, .925, "sumo", "createroom", "sumo.xml");
    Dropdownmenus[0].Buttons[2] = UIAddButton(.45, .900, "classic 8", "createroom", "classic8.xml");

    //disable drawing and testing of buttons until we want them
    ButtonList[Dropdownmenus[0].Buttons[0]].Draw = false;
    ButtonList[Dropdownmenus[0].Buttons[1]].Draw = false;
    ButtonList[Dropdownmenus[0].Buttons[2]].Draw = false;

    //set the length of the drop down
    ButtonList[Dropdownmenus[0].Buttons[0]].br_x = .80;
    ButtonList[Dropdownmenus[0].Buttons[1]].br_x = .80;
    ButtonList[Dropdownmenus[0].Buttons[2]].br_x = .80;

}

//member of drop down class?
void DisplayMapDropdown(bool tDraw) {
    Dropdownmenus[0].Draw = tDraw;

    ButtonList[Dropdownmenus[0].Buttons[0]].Draw = tDraw;
    ButtonList[Dropdownmenus[0].Buttons[1]].Draw = tDraw;
    ButtonList[Dropdownmenus[0].Buttons[2]].Draw = tDraw;
}

void DisplaySectorsDropdown(bool tDraw) {
    Dropdownmenus[2].Draw = tDraw;

    ButtonList[Dropdownmenus[2].Buttons[0]].Draw = tDraw;
    ButtonList[Dropdownmenus[2].Buttons[1]].Draw = tDraw;
}

void DisplaySectorsListDropdown(bool tDraw) {
    Dropdownmenus[3].Draw = tDraw;

    ButtonList[Dropdownmenus[3].Buttons[0]].Draw = tDraw;
    ButtonList[Dropdownmenus[3].Buttons[1]].Draw = tDraw;
    ButtonList[Dropdownmenus[3].Buttons[2]].Draw = tDraw;
    ButtonList[Dropdownmenus[3].Buttons[3]].Draw = tDraw;
    ButtonList[Dropdownmenus[3].Buttons[4]].Draw = tDraw;
    ButtonList[Dropdownmenus[3].Buttons[5]].Draw = tDraw;
    ButtonList[Dropdownmenus[3].Buttons[6]].Draw = tDraw;
}

void DisplayMainMenuDropdown(bool tDraw) {
    Dropdownmenus[1].Draw = tDraw;

    ButtonList[Dropdownmenus[1].Buttons[0]].Draw = tDraw;
    ButtonList[Dropdownmenus[1].Buttons[1]].Draw = tDraw;
}

/*
void UITexture::LoadUITexture(int x, int y, string filename, string name) {
    X = x;
    Y = y;
    Address[Count] = LoadGLTexture( filename.c_str() );
    Name[Count] = name;
    Count++;
}
*/
