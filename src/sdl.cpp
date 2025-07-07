#include <iostream>

#include "SDL2/SDL.h"

#include "sdl.h"
#include "ui.h"
#include "client.h"

extern LocalClient LC;

void InitSDL() {
    cout << "SDL: " << SDL_MAJOR_VERSION << "." << SDL_MINOR_VERSION << "." << SDL_PATCHLEVEL << '\n';
    
    if(LC.isConsoleMode()) {
        // Only initialize SDL timer
        if (SDL_Init(SDL_INIT_TIMER) < 0) {
            cout << "SDL Error: " << SDL_GetError() << '\n';
        } else {
            cout << "SDL: Timer initialized successfully" << '\n';
        }
        return;
    }
    
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0) {
        cout << "SDL Error: " << SDL_GetError() << '\n';
    } else {
        cout << "SDL: Video and timer initialized successfully" << '\n';
    }

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
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    // double buffering and vsync; I feel this is ideal for our game
    SDL_GL_SetSwapInterval(1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    //TODO: SDL_RESIZABLE and SDL_WINDOW_FULLSCREEN
    SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
    LC.window = SDL_CreateWindow("HoverX", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LC.window_width, LC.window_height, windowFlags);
    if (LC.window == NULL) {
        cout << "SDL: Could not create window" << '\n';
        cout << "SDL ERROR: " << SDL_GetError() << '\n';
        return;
    }

    LC.glContext = SDL_GL_CreateContext(LC.window);
    if(LC.glContext == NULL) {
        cout << "SDL: Could not create OpenGL context" << '\n';
        cout << "SDL ERROR: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(LC.window);
        return;
    }

    if(SDL_GL_MakeCurrent(LC.window, LC.glContext) < 0) {
        cout << "SDL: Could not make OpenGL context current" << '\n';
        cout << "SDL ERROR: " << SDL_GetError() << '\n';
        SDL_DestroyWindow(LC.window);
        return;
    } else {
        cout << "SDL: Window and OpenGL context created successfully" << '\n';      
    }
}

void GetSDLInput() {
    // console can hanlde its own input?
    if(LC.isConsoleMode())
        return;

    SDL_Event event;
    while ( SDL_PollEvent(&event) ) {
        switch(event.type) {
            case SDL_QUIT:
                LC.StopClient();
                break;

            case SDL_KEYDOWN:
                //get input this sucks bad 96 instead of 1
                if ( (event.key.keysym.sym > 96 && event.key.keysym.sym < 123) || (event.key.keysym.sym == SDLK_SPACE) ||
                    (event.key.keysym.sym == SDLK_BACKSPACE) || (event.key.keysym.sym == SDLK_SLASH) || (event.key.keysym.sym == SDLK_PERIOD) ||
                    (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9) )  {
                        //add char to the string
                        if(LC.MSGmode && event.key.keysym.sym != SDLK_BACKSPACE) {
                            LC.hostMSGbuffer += event.key.keysym.sym;
                        }
                }

                //delete char of the end of the string
                if(event.key.keysym.sym == SDLK_BACKSPACE && !LC.hostMSGbuffer.empty() ) {
                    LC.hostMSGbuffer.erase( LC.hostMSGbuffer.size() - 1 );
                }

                //enter message mode
                if ( event.key.keysym.sym == SDLK_t && !LC.MSGmode ) {
                    if(!LC.MSGmode) {
                        LC.MSGmode = true;
                    }
                }

                //exit message mode
                if ( event.key.keysym.sym == SDLK_RETURN && LC.MSGmode ) {
                    LC.MSGmode = false;
                    if( !LC.hostMSGbuffer.empty() ) {
                        LC.AddTextMessage(LC.hostMSGbuffer);
                    }
                    LC.hostMSGbuffer = "";
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