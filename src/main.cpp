#include <stdlib.h>
#include <stdio.h>
#include "iostream"

#include "SDL2/SDL.h"
#include "enet/enet.h"

#include "main.h"
//#include "math.h"
#include "level.h"
#include "tinerxml.h"
#include "glness.h"
#include "font.h"
#include "ui.h"
#include "renderer.h"
#include "server.h"
#include "client.h"

#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

using namespace std;

//move into ui code
string hostMSGbuffer;   //hold incomplete string to be pushed to the send buffer
// !!!!!!!!!!!!!!!


//my vars
//float number = 0;
Uint8*  keys = NULL;

level mylvl;    //the one true world
ServerClass hxServer;
LocalClient LC; //local client data
//menu mymnu; //delete

//game states
bool isConnected = false;  //net flag
bool isHost = false;    //net flag
bool isConsole = false;  //use text output only; no window/gl
bool MSGmode = false;   //game state for typing a message or command
bool done;               //MOST MAIN LOOP!!

int DefaultTextureID;
int DesktopTexture;

//used to calculate frames per sec being drawn
int DrawDeltaTime = 0;
int afterdraw = 0;
int beforedraw = 0;

int mouse_x = 320;
int mouse_y = 240;
float mx = 0;
float my = 0;

int window_width = 640;
int window_height = 480;
int window_fullscreen = 0;

// SDL/GL/Window stuff
SDL_Window *window;
SDL_GLContext   glContext;

//should go into renderer
void InitGL(int Width, int Height)  {
    glViewport(0, 0, Width, Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();    // Reset The Projection Matrix

    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height, 1.0f, 400.0f); // Calculate The Aspect Ratio Of The Window

    glMatrixMode(GL_MODELVIEW);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);                   // Black Background
    glClearDepth(1.0f);                                     // Depth Buffer Setup
    glDepthFunc(GL_LEQUAL);                                 // Type Of Depth Testing
    glEnable(GL_DEPTH_TEST);                                // Enable Depth Testing
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // Enable Alpha Blending (disable alpha testing)
    glEnable(GL_BLEND);                                     // Enable Blending       (disable alpha testing)
    glAlphaFunc(GL_GREATER,0.1f);                           // Set Alpha Testing     (disable blending)
    glEnable(GL_ALPHA_TEST);                                // Enable Alpha Testing  (disable blending)
    glEnable(GL_TEXTURE_2D);                                // Enable Texture Mapping
    glEnable(GL_CULL_FACE);                                 // Remove Back Face
    glShadeModel(GL_SMOOTH);                                // Select Smooth Shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);      // Set Perspective Calculations To Most Accurate


    //output some gl info
    cout << "maker: " << glGetString(GL_VENDOR) << endl;
    cout << "render path: " << glGetString(GL_RENDERER) << endl;
    cout << "opengl version: " << glGetString(GL_VERSION) << endl << endl;
    //cout << "extentions: " << glGetString(GL_EXTENSIONS) << endl;

    //lighting
    GLfloat White_Light[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat Ambiant_Light[] = {0.7, 0.7, 0.9, 1.0};
    GLfloat Light0_Position[] = {0.0, 0.0, 1.0, 1.0};
    //GLfloat Specular_Material[] = {1.0, 1.0, 1.0, 1.0};


    glLightfv(GL_LIGHT0, GL_DIFFUSE, White_Light);
    glLightfv(GL_LIGHT0, GL_POSITION, Light0_Position);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Ambiant_Light);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    //default texture to load when one is missing
    DefaultTextureID = LoadGLTexture((char*)"img/default.png");
    if (DefaultTextureID ==  0) {
        cout << "InitGL: Could not load default texture img/default.png" << endl;
    }

    //Background image of the window
    DesktopTexture = LoadGLTexture((char*)"img/desktop.jpg");
    if (DesktopTexture ==  0) {
        cout << "InitGL: Could not load img/desktop.jpg" << endl;
    }
}

//100 fps
#define TICK_RATE 10
int static TimeofLastUpdate = 0;

int GetDeltaTime() {
    int returnvalue = 0;
    int timenow = 0;
    timenow = SDL_GetTicks();

    //get the time between the last tick and now
    returnvalue = timenow - TimeofLastUpdate;

    //set the time of last update to now as now is when we will tick
    TimeofLastUpdate = timenow;

    return returnvalue;
}

int static tickcount = 0;
void Tick() {
    if(tickcount == 100) {
        //cout << "one sec of ticking, SDL_GetTicks: " << SDL_GetTicks() << endl;
        tickcount = 0;
    }
    tickcount++;

    LC.Update();
    if(hxServer.isRunning) hxServer.Update();
}

int static deltatime = 0;
int static accumulator = 0;

void Ticker() {
    //find out how much time has pasted sence the last tick
    deltatime = GetDeltaTime();
    deltatime = deltatime + accumulator;

    //tick how ever many times we missed
    while(deltatime >= TICK_RATE) {
        deltatime = deltatime - TICK_RATE;
        Tick();
    }

    //accumulate how much time we have lost this go around and add it to the next go
    accumulator = deltatime;

    //wait whats left of delta time and we will tick right on time
    if(accumulator > TICK_RATE) accumulator = TICK_RATE; //dont want to wait negitive time
    SDL_Delay(TICK_RATE - accumulator);
}

void DrawGLScene() {
    //fps counter
    DrawDeltaTime = afterdraw - beforedraw;
    beforedraw = SDL_GetTicks();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear The Screen And The Depth Buffer
    glLoadIdentity();   // Reset The View

    //draw world
    if(LC.DrawWorld) DrawWorld();

    //overlay hud
    DrawHUD();

    // swap buffers to display, since we're double buffered.
    //SDL_GL_SwapBuffers();
    SDL_GL_SwapWindow(window);
    afterdraw = SDL_GetTicks();
}

void initvid(int width,int height,int fs) {
    //TODO: SDL_RESIZABLE
    SDL_WindowFlags windowFlags;
    windowFlags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

    if(fs) {
        windowFlags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI |  SDL_WINDOW_FULLSCREEN);
    }

    window = SDL_CreateWindow("HoverX", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, windowFlags);
    glContext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glContext);
    SDL_GL_SetSwapInterval(1);                              //Enable vsync
    cout << "initvid: Completed window creation" << endl;

    InitGL(width, height);
}

void InitLocalClient() {
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
    }

    //load settings
    xmlfile settings;
    TiXmlElement *xGame = settings.getxmlfirstelement((char*)"settings.xml");
    TiXmlElement *xGraphics = settings.getelement(xGame, (char*)"graphics");
    window_width = (int)atoi(xGraphics->Attribute("w"));
    window_height = (int)atoi(xGraphics->Attribute("h"));
    window_fullscreen = (int)atoi(xGraphics->Attribute("fullscreen"));
    TiXmlElement *xUser = settings.getelement(xGame, (char*)"user");
    LC.Username = xUser->Attribute("name");
    settings.endxml();

    //open window
    initvid(window_width, window_height, window_fullscreen);

    //set up networking
    LC.ClientHost = enet_host_create (
        NULL        /* create a client host */,
        1           /* only allow 1 outgoing connection */,
        MAXCHANNELS /* max channels */,
        0           /* 0 is unlimited; 128000 12.8k of incoming bandwidth */,
        0           /* 0 is unlimited; 128000 12.8k of amount of outgoing bandwidth */);

    if (LC.ClientHost == NULL)
        cout <<  "client: could not create net client" << endl;
    isConnected = true;

    initsimplefont((char*)"img/font.tga");                  //font system
    UI_Setup(); //init ui system
    //keys = SDL_GetKeyState(NULL);                          //umm
    LC.DrawWorld = false;
}

void GetInput() {
        SDL_Event event;
        while ( SDL_PollEvent(&event) ) {
            switch(event.type) {
            case SDL_QUIT:
                done = 1;
                break;
            case SDL_KEYDOWN:
                //cout << event.key.keysym.sym << " pressed" << endl;
                //keys[event.key.keysym.sym] = 1;

                //get input this sucks bad 96 instead of 1
                if ( (event.key.keysym.sym > 96 && event.key.keysym.sym < 123) || (event.key.keysym.sym == SDLK_SPACE) ||
                     (event.key.keysym.sym == SDLK_BACKSPACE) || (event.key.keysym.sym == SDLK_SLASH) || (event.key.keysym.sym == SDLK_PERIOD) ||
                     (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9) )  {     //letter input.... this is a mess
                    if(MSGmode && event.key.keysym.sym != SDLK_BACKSPACE) {     //add char to the string
                        hostMSGbuffer += event.key.keysym.sym;
                    }
                }

                if(event.key.keysym.sym == SDLK_BACKSPACE && !hostMSGbuffer.empty() ) {    //delete char of the end of the string
                    hostMSGbuffer.erase( hostMSGbuffer.size() - 1 );
                }

                if ( event.key.keysym.sym == SDLK_t && !MSGmode ) {     //enter message mode
                    if(!MSGmode) {
                        MSGmode = true;
                    }
                }

                if ( event.key.keysym.sym == SDLK_RETURN && MSGmode ) {    //exit message mode
                    MSGmode = false;
                    if( !hostMSGbuffer.empty() ) {
                        LC.AddTextMessage(hostMSGbuffer);
                    }
                    hostMSGbuffer = "";
                }

                //standard controls
                if (event.key.keysym.sym == SDLK_LSHIFT ) LC.Keys.accel = true;
                if (event.key.keysym.sym == SDLK_SPACE ) LC.Keys.brake = true;
                if (event.key.keysym.sym == SDLK_LEFT ) LC.Keys.left = true;
                if (event.key.keysym.sym == SDLK_RIGHT ) LC.Keys.right = true;
                if (event.key.keysym.sym == SDLK_UP ) LC.Keys.jump = true;

                break;
            case SDL_KEYUP:
                //keys[event.key.keysym.sym] = 0;

                //standard controls
                if (event.key.keysym.sym == SDLK_LSHIFT ) LC.Keys.accel = false;
                if (event.key.keysym.sym == SDLK_SPACE ) LC.Keys.brake = false;
                if (event.key.keysym.sym == SDLK_LEFT ) LC.Keys.left = false;
                if (event.key.keysym.sym == SDLK_RIGHT ) LC.Keys.right = false;
                if (event.key.keysym.sym == SDLK_UP ) LC.Keys.jump = false;

                break;
            case SDL_MOUSEBUTTONDOWN:
                UIMousePress( event.button.x, event.button.y );
                break;
            case SDL_MOUSEBUTTONUP:
                break;
            }
        }
}

int main(int argc, char *argv[]) {
    //get commandline options
    for(int i = 1; i < argc; i++) {
        string argstr = argv[i];
        if(argstr == "console") isConsole = true;
    }

    enet_initialize(); //start networking
    SDL_Init(SDL_INIT_TIMER);

    //open window and load settings
    if(!isConsole)
        InitLocalClient();

    //turn the server on; allow remote connections if in console mode
    hxServer.Start(isConsole);

    done = 0;
    while ( ! done ) {
        Ticker();   //update worlds
        if(!isConsole) {
            DrawGLScene();  //draw world
            GetInput();
        }
    }

    //LC.lvl.Unload();  //this should be server unload if not connected to a remote server
    hxServer.Stop();
    SDL_Quit();
    return 0;
}

//helper functions
bool FileExists( const char* FileName ) {    //returns true if file exist
    FILE* fp = NULL;

    fp = fopen( FileName, "rb" );
    if( fp != NULL )    {
        fclose( fp );
        return true;
    }

    fclose( fp );
    return false;
}

int FindKeyByName(KeyValue *kv, string keytofind) {    //return the location of the data we are looking for or -1 on fail
    for(int i = 0; i < 31; i++) {
        if(kv[i].Key== keytofind) {
            return i;
        }
    }

    return -1;
}

void ParseTKV(string worldstr, TypeKeyValue *TKV) {     //turn data string into type key and value
    size_t slash1 = 0;
    size_t slash2 = 0;
    size_t spacepos = 0;
    size_t equalpos = 0;
    string datapoint = "";
    bool endofkv = false;
    bool endoftype = false;
    int i = 0;
    int j = 0;

    endoftype = false;
    while ( !endoftype && i < 63) {
        //find the '/'
        slash1 = worldstr.find('/', slash1);
        slash2 = worldstr.find('/', slash1 + 1);
        //cout << "slash1: " << slash1 << " slash2: " << slash2 << endl;

        //use the end of the file as the end if there are no more '/'
        if(slash2 == string::npos) {
            slash2 = worldstr.length();
            endoftype = true;
        }

        if(slash1 != string::npos && slash2 != string::npos) {
            //seperate the type class from the rest of the string
            datapoint = worldstr.substr(slash1 + 1, slash2 - 1);
            //cout << "datapoint: " << datapoint << endl;

            //remove datapoin from the world string so it doesnt get proccessed again
            worldstr = worldstr.substr(slash2, worldstr.length());

            //get the type
            spacepos = datapoint.find(' ');
            TKV[i].Type = datapoint.substr(0, spacepos);
            //cout << "type: " << TKV[i].Type << "->";

            //remove type from datapoint
            datapoint = datapoint.substr(spacepos, datapoint.length());

            j = 0;
            endofkv = false;
            while( !endofkv && j < 31 ) {
                //cout << endl << "what to process: \"" << datapoint << "\"" << endl;

                //remove space from begining of string; potentail bug only removes 1 leading ' '
                if(datapoint.length() != 0) {
                    if(datapoint.at(0) == ' ') datapoint.erase(0, 1);
                }


                if(datapoint.length() != 0) {
                    //find the next space
                    size_t strend = datapoint.find(' ');

                    //use the end of the file as the end if there is no space
                    if(strend == string::npos) strend = datapoint.length();

                    //find a '=' between the beginning and the next space/end of string; if not no reason to continue
                    equalpos = datapoint.find('=');

                    if(equalpos != string::npos) {
                        string keyvaluestr = datapoint.substr(0, strend);
                        //cout << "keyvaluestr: \"" << keyvaluestr << "\"" << endl;

                        datapoint = datapoint.substr(strend, datapoint.length());
                        //cout << "new datapoint: \"" << datapoint << "\"" << endl;

                        //now pull out the left and right side of the '='
                        TKV[i].KV[j].Key = keyvaluestr.substr(0, equalpos);
                        TKV[i].KV[j].sValue = keyvaluestr.substr(equalpos+1, strend);

                        //cout << "\"" << TKV[i].KV[j].Key << "\"" << "=" << "\"" << TKV[i].KV[j].sValue << "\"" << endl;
                    }
                }
                else endofkv = true;

                j++;
            }
            if(j >= 31) cout << "HandleWorldString: more than 32 key/values in type" << endl;
        }

        i++;
    }
    if(i >= 63) cout << "HandleWorldString: more than 64 types in string" << endl;

}
