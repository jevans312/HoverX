#include "SDL2/SDL.h"

#include "renderer.h"
#include "level.h"
#include "font.h"
#include "ui.h"
#include "client.h"

//extern level mylvl;    //the one true world
extern LocalClient LC;

//Initialize OpenGL
void InitGL() {
    GLenum err = glewInit();
    cout << '\n' << "====== OpenGL ======" << '\n';
    cout << "Vendor: " << glGetString(GL_VENDOR) << '\n';
    cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';
    cout << "Version: " << glGetString(GL_VERSION) << '\n';
    cout << "GLEW: " << ((GLEW_OK ==  err)?glewGetString(GLEW_VERSION):glewGetErrorString(err)) << '\n';

    glViewport(0, 0, LC.window_width, LC.window_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();    // Reset The Projection Matrix
    gluPerspective(45.0f, (GLfloat)LC.window_width/(GLfloat)LC.window_height, 1.0f, 400.0f);
    glMatrixMode(GL_MODELVIEW);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);                   // Black Background
    glClearDepth(1.0f);                                     // Depth Buffer Setup

    glDepthFunc(GL_LESS);                                   // Type Of Depth Testing
    glEnable(GL_DEPTH_TEST);                                // Enable Depth Testing

    //WARNING! Memory bandwith is often doubled with blending enabled!
    glEnable(GL_BLEND);                                     // Enable Blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // Enable Alpha Blending
    //WARNING! Find out how to keep this disabled most of the time!

    glAlphaFunc(GL_GREATER, 0.5f);                          // Set Alpha Testing
    glEnable(GL_ALPHA_TEST);                                // Enable Alpha Testing

    glEnable(GL_TEXTURE_2D);                                // Enable Texture Mapping
    glEnable(GL_CULL_FACE);                                 // Remove Back Face rendering
    glCullFace(GL_BACK);

    glShadeModel(GL_SMOOTH);                                // Select Smooth Shading
    glDisable(GL_DITHER);                                   // TODO: What does this do?
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);      // Set Perspective Calculations To Most Accurate

    //lighting
    GLfloat White_Light[] = {0.4, 0.4, 0.4, 0.0};
    GLfloat Light_Position[] = {0.0, 0.0, 1.0};
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, White_Light);
    glLightfv(GL_LIGHT0, GL_POSITION, Light_Position);
    glEnable(GL_LIGHT0);

    GLfloat Ambiant_Light[] = {0.7, 0.7, 0.7, 0.0};
    //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    //glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Ambiant_Light);
}

uint64_t afterDrawTime = 0;
uint64_t beforeDrawTime = 0;
void DrawScene() {
    beforeDrawTime = SDL_GetTicks64();

    glLoadIdentity();   // Reset The View
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear The Screen And The Depth Buffer

    //draw world
	DrawWorld();

    //overlay hud
    DrawHUD();

    //swap buffers/vsync
    SDL_GL_SwapWindow(LC.window);
    
    //fps counter
    afterDrawTime = SDL_GetTicks64();
    LC.frameTime = afterDrawTime - beforeDrawTime;
}

//draw level
void DrawWorld() {
    if(LC.DrawWorld) {
        DrawSky();
        SetCamera();
        DrawLevel();
        DrawEntities();
    }
}

// 2D HUD overlay
void DrawHUD() {
    //draw desktop if not in game
    if(LC.DrawWorld) {
        UI_DrawLapTimes();
    } else {
        UI_DrawDesktop();
    }

    //draw messages
    UI_DrawMSGs();

    //draw buttons and such
    UI_DrawButtons();
}

//Draw the sky box
//TODO: reduce scale and position bottom closer to the horizen
void DrawSky() {
    int entcam = 0;
    if(LC.EntityAddress != -1) entcam = LC.EntityAddress;

    glDisable(GL_LIGHTING);
    glBindTexture( GL_TEXTURE_2D, LC.lvl.gridtexid);

    glColor3f(1.f, 1.f, 1.f);   //reset the color

    float xrot = -(LC.lvl.Ent[entcam].Yaw/2);

    glDisable(GL_DEPTH_TEST);
    glPushMatrix();
        glTranslatef(0,0,-10);
        glScalef(7,7,7);

        glBegin(GL_QUADS);
            glTexCoord2f(1.0f-xrot, 1.0f);  glVertex3f(  1.f,-1.f,0.f );
            glTexCoord2f(1.0f-xrot, 0.0f);  glVertex3f(  1.f, 1.f,0.f );
            glTexCoord2f(0.0f-xrot, 0.0f);  glVertex3f( -1.f, 1.f,0.f );
            glTexCoord2f(0.0f-xrot, 1.0f);  glVertex3f( -1.f,-1.f,0.f );
        glEnd();
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

}

//should be conditional on wether a player is loaded
void SetCamera() {
    const int entcam = LC.EntityAddress;

    if(entcam != -1 && LC.lvl.Ent[entcam].isUsed) {
        vector2d const entPos = LC.lvl.Ent[entcam].pos.c;
        vector2d const v (-sinf(LC.lvl.Ent[entcam].Yaw), -cosf(LC.lvl.Ent[entcam].Yaw));

        gluLookAt(entPos.x + (v.x*5),	entPos.y + (v.y*5),		entPos.z + 2,
                  entPos.x,				entPos.y,				entPos.z + 1,
                  0,0,1);
    } else {
        cout << "SetCamera::Invalid Entity:" << IntToStr(entcam)
             <<  " LC.EntityAddress:" << IntToStr(LC.EntityAddress) <<'\n';

        if(LC.lvl.Loaded) {
            //Maybe server disconnected so we just unload level?
            cout << "SetCamera::Unloading level" <<  '\n';
            LC.lvl.Unload();
        } else {
            //This is probably in an unrecoverable state...
            cout << "SetCamera::Setting LC.DrawWorld = false" << '\n';
            LC.DrawWorld = false;
        }
    }
}

void DrawLevel() {
    glDisable(GL_CULL_FACE);
    glCallList(LC.lvl.DisplayList);
    glEnable(GL_CULL_FACE);
}

void DrawEntities() {
    if(LC.lvl.Loaded ==  false) {
        cout << "DrawEntities: No level is loaded" << '\n';
        return;
    }

    for (int i = 0; i < MAXENTITIES; i++) {
        //draw existing entities
        if(LC.lvl.Ent[i].isUsed) {

            //cout << "drawing ent #" << i << endl;
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
                //postion and roatate
                glTranslatef(LC.lvl.Ent[i].pos.c.x, LC.lvl.Ent[i].pos.c.y, LC.lvl.Ent[i].pos.c.z);
                glRotatef( -( LC.lvl.Ent[i].Yaw * (180/3.14159265) ), 0.0, 0.0, 1.0  );

                //draw model
                glCallList(LC.lvl.Ent[i].Model.DisplayList);
            glPopMatrix();
        }
    }
}
