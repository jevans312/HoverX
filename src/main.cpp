#include <stdlib.h>
#include <stdio.h>
#include <iostream>
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

level mylvl; //clients level
ServerClass hxServer;
LocalClient LC; //local client data

//game states
//TODO: Move is stuff to client/server and MSGmode to UI
bool isConnected = false;   //net flag
bool isHost = false;        //net flag
bool isConsole = false;     //use text output only; no window/gl
bool MSGmode = false;       //game state for typing a message or command
bool done;                  //main loop control

//TODO: move to client render code
// SDL/GL/Window stuff
SDL_Window *window;
SDL_GLContext glContext;
int window_width = 640;
int window_height = 480;
int window_fullscreen = 0;
int DefaultTextureID;
int DesktopTexture;

//used to calculate frames per sec being drawn
int64_t DrawDeltaTime = 0;
uint64_t afterdraw = 0;
uint64_t beforedraw = 0;

// used by UI class //TODO: Get rid of these global
string hostMSGbuffer; //Move to UI code

int mouse_x = 320;
int mouse_y = 240;
float mx = 0;
float my = 0;


//100 fps
#define TICK_RATE 10

//!Warning this gives you the time difference since you lasted call this function!
uint64_t static TimeofLastUpdate = 0;
int64_t getDeltaTime() {
    int64_t returnvalue = 0;
    uint64_t timenow = SDL_GetTicks64();

    //get the time between the last tick and now
    returnvalue = timenow - TimeofLastUpdate;

    //set the time of last update to now as now is when we will tick
    TimeofLastUpdate = timenow;

    return returnvalue;
}

uint64_t static tickcount = 0;
void Tick() {
    //Do once a second
    if(tickcount == 1000/TICK_RATE) {
        cout << "DrawDeltaTime: " << DrawDeltaTime << '\n';
        tickcount = 0;
    }
    tickcount++;

    LC.Update();
    hxServer.Update();
}

uint64_t static deltatime = 0;
uint64_t static accumulator = 0;
void Ticker() {
     //find out how much time has pasted sence the last tick
    deltatime = getDeltaTime();
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
    beforedraw = SDL_GetTicks64();

    glLoadIdentity();   // Reset The View
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear The Screen And The Depth Buffer

    //draw world
	DrawWorld();

    //overlay hud
    DrawHUD();

    //Any errors accumulate? //Note: glGetError causes a full pipeline sync!
    GLenum errorcode;
    while((errorcode = glGetError()) != GL_NO_ERROR) {
        cout << "DrawGLScene() GL error: " << gluErrorString(errorcode) << '\n';
    }

    glFlush();

	//double buffered; swap buffers to display
    SDL_GL_SwapWindow(window);
    afterdraw = SDL_GetTicks64();
}

void initGlWindow(const int width, const int height, const int fs) {
    // gl version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // data and buffer sizes
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // antialiasing
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

    //SDL_GL_SetSwapInterval(1);  is supposed to enable vsync but just in case
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    //TODO: SDL_RESIZABLE and SDL_WINDOW_FULLSCREEN
    if (SDL_Init(SDL_INIT_VIDEO) < 0)  cout << "SDL_INIT_VIDEO Failed: " << SDL_GetError() << '\n';
    if(fs) cout << "initGlWindow: fullscreen has not been implemented" << '\n';
    SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow("HoverX", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, windowFlags);
    glContext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glContext);
    SDL_GL_SetSwapInterval(1); //Enable vsync

    //output openGL info
    GLenum err = glewInit();
    cout << '\n';
    cout << "====== OpenGL ======" << '\n';
    cout << "Vendor: " << glGetString(GL_VENDOR) << '\n';
    cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';
    cout << "Version: " << glGetString(GL_VERSION) << '\n';
    cout << "GLEW: " << ((GLEW_OK ==  err)?glewGetString(GLEW_VERSION):glewGetErrorString(err)) << '\n';
    if(!GLEW_VERSION_3_0) cout << "OpenGL 3.0: Not supported" << '\n';

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();    // Reset The Projection Matrix
    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height, 1.0f, 400.0f);
    glMatrixMode(GL_MODELVIEW);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);                   // Black Background
    glClearDepth(1.0f);                                     // Depth Buffer Setup

    glDepthFunc(GL_LESS);                                   // Type Of Depth Testing
    glEnable(GL_DEPTH_TEST);                                // Enable Depth Testing

    glEnable(GL_BLEND);                                     // Enable Blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // Enable Alpha Blending

    glAlphaFunc(GL_GREATER, 0.5f);                          // Set Alpha Testing
    glEnable(GL_ALPHA_TEST);                                // Enable Alpha Testing

    glEnable(GL_TEXTURE_2D);                                // Enable Texture Mapping
    glEnable(GL_CULL_FACE);                                 // Remove Back Face rendering
    glCullFace(GL_BACK);

    glShadeModel(GL_SMOOTH);                                // Select Smooth Shading
    glDisable(GL_DITHER);                                   // TODO: What does this do?
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);      // Set Perspective Calculations To Most Accurate

    //lighting
    glEnable(GL_LIGHTING);
    GLfloat White_Light[] = {0.4, 0.4, 0.4, 0.0};
    GLfloat Light_Position[] = {0.0, 0.0, 1.0};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, White_Light);
    glLightfv(GL_LIGHT0, GL_POSITION, Light_Position);
    glEnable(GL_LIGHT0);

    GLfloat Ambiant_Light[] = {0.7, 0.7, 0.7, 0.0};
    //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    //glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Ambiant_Light);
}

void InitLocalClient() {
    cout << "====== HoverX ======" << '\n';
    if (SDL_Init(SDL_INIT_TIMER) < 0) {
        cout << "InitLocalClient: SDL_INIT_TIMER Failed: " << SDL_GetError() << '\n';
    }

    //set up networking
    enet_initialize();
    //cout << "ENet: " << ENET_VERSION_MAJOR << "."
    //     << ENET_VERSION_MINOR <<  "." << ENET_VERSION_PATCH << '\n';

    LC.ClientHost = enet_host_create (NULL, 1,  MAXCHANNELS, 0, 0);
    if (LC.ClientHost == NULL) {
        cout <<  "InitLocalClient: could not create net client" << '\n';
    } else {
        isConnected = true;
    }

    //Display related initializations
    if(isConsole ==  false) {
        cout << "Client: Local" << '\n';
        //load settings
        if(FileExists((char*)"settings.xml")) {
            xmlfile settings;
            TiXmlElement *xGame = settings.getxmlfirstelement((char*)"settings.xml");
            TiXmlElement *xGraphics = settings.getelement(xGame, (char*)"graphics");
            window_width = (int)atoi(xGraphics->Attribute("w"));
            window_height = (int)atoi(xGraphics->Attribute("h"));
            window_fullscreen = (int)atoi(xGraphics->Attribute("fullscreen"));
            TiXmlElement *xUser = settings.getelement(xGame, (char*)"user");
            LC.Username = xUser->Attribute("name");
            settings.endxml();
            cout << "Settings.xml: Loaded" << '\n';
        } else {
            cout << "Settings.xml: No file" << '\n';
            LC.Username = "unnamed";
        }

        //open window and setup OpenGL
        initGlWindow(window_width, window_height, window_fullscreen);

        //2D drawing stuff
        initsimplefont((char*)"img/font.tga");
        UI_Setup();

        //default texture to load when one is missing
        DefaultTextureID = LoadGLTexture((char*)"img/default.png");
        if (DefaultTextureID ==  0) {
            cout << "InitGL: Could not load default texture img/default.png" << '\n';
        }

        //Background image of the window
        DesktopTexture = LoadGLTexture((char*)"img/desktop.jpg");
        if (DesktopTexture ==  0) {
            cout << "InitGL: Could not load img/desktop.jpg" << '\n';
        }

        //Show "desktop"
        LC.DrawWorld = false;
    }
}

//TODO: Move to client
void GetInput() {
    // console can hanlde its own input?
    if(isConsole)
        return;

    SDL_Event event;
    while ( SDL_PollEvent(&event) ) {
        switch(event.type) {
            case SDL_QUIT:
                done = 1;
                break;

            case SDL_KEYDOWN:
                //get input this sucks bad 96 instead of 1
                if ( (event.key.keysym.sym > 96 && event.key.keysym.sym < 123) || (event.key.keysym.sym == SDLK_SPACE) ||
                    (event.key.keysym.sym == SDLK_BACKSPACE) || (event.key.keysym.sym == SDLK_SLASH) || (event.key.keysym.sym == SDLK_PERIOD) ||
                    (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9) )  {
                        //add char to the string
                        if(MSGmode && event.key.keysym.sym != SDLK_BACKSPACE) {
                            hostMSGbuffer += event.key.keysym.sym;
                        }
                }

                //delete char of the end of the string
                if(event.key.keysym.sym == SDLK_BACKSPACE && !hostMSGbuffer.empty() ) {
                    hostMSGbuffer.erase( hostMSGbuffer.size() - 1 );
                }

                //enter message mode
                if ( event.key.keysym.sym == SDLK_t && !MSGmode ) {
                    if(!MSGmode) {
                        MSGmode = true;
                    }
                }

                //exit message mode
                if ( event.key.keysym.sym == SDLK_RETURN && MSGmode ) {
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

        if(argstr == "console") {
            isConsole = true;
        }
    }

    //Setup client and create if not in cosole mode
    InitLocalClient();

    //turn the server on; allow remote connections if in console mode
    hxServer.Start(isConsole);

    done = 0;
    while ( ! done ) {
        GetInput();
        Ticker();

        if(isConsole) {
            //TODO: Update console here
        } else {
            DrawGLScene();
        }
    }

    hxServer.Stop();
    SDL_Quit();
    return 0;
}

//helper functions
string IntToIpAddress(unsigned int ip) {
    char ips[20];
    unsigned char bytes[4];
    bytes[0] = ip & 0xFF;
    bytes[1] = (ip >> 8) & 0xFF;
    bytes[2] = (ip >> 16) & 0xFF;
    bytes[3] = (ip >> 24) & 0xFF;
    sprintf(ips, "%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0]);
    return ips;
}

string BoolToStr(const bool b) {
    if(b) {
        return "True";
    } else {
        return "False";
    }
}

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
        //cout << "slash1: " << slash1 << " slash2: " << slash2 << '\n';

        //use the end of the file as the end if there are no more '/'
        if(slash2 == string::npos) {
            slash2 = worldstr.length();
            endoftype = true;
        }

        if(slash1 != string::npos && slash2 != string::npos) {
            //seperate the type class from the rest of the string
            datapoint = worldstr.substr(slash1 + 1, slash2 - 1);
            //cout << "datapoint: " << datapoint << '\n';

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
                //cout << '\n' << "what to process: \"" << datapoint << "\"" << '\n';

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
                        //cout << "keyvaluestr: \"" << keyvaluestr << "\"" << '\n';

                        datapoint = datapoint.substr(strend, datapoint.length());
                        //cout << "new datapoint: \"" << datapoint << "\"" << '\n';

                        //now pull out the left and right side of the '='
                        TKV[i].KV[j].Key = keyvaluestr.substr(0, equalpos);
                        TKV[i].KV[j].sValue = keyvaluestr.substr(equalpos+1, strend);

                        //cout << "\"" << TKV[i].KV[j].Key << "\"" << "=" << "\"" << TKV[i].KV[j].sValue << "\"" << '\n';
                    }
                }
                else endofkv = true;

                j++;
            }
            if(j >= 31) cout << "HandleWorldString: more than 32 key/values in type" << '\n';
        }

        i++;
    }
    if(i >= 63) cout << "HandleWorldString: more than 64 types in string" << '\n';

}
