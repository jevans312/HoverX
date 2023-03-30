#include "renderer.h"
#include "level.h"
#include "font.h"
#include "ui.h"
#include "client.h"

//extern level mylvl;    //the one true world
extern LocalClient LC;

//draw level
void DrawWorld() {
    if(LC.DrawWorld) {
        DrawSky();
        SetCamera();
        DrawLevel();
        DrawEntities();
    }
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
