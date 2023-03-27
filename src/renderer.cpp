#include "renderer.h"
#include "level.h"
#include "font.h"
#include "ui.h"
#include "client.h"

//extern level mylvl;    //the one true world
extern LocalClient LC;

//draw everything
void DrawWorld() {
    DrawSky();
    SetCamera();
    DrawLevel();
    DrawEntities();

    //tell gl to draw
    glFlush();  //should this only go here? //JAE 8/23/21 - Word has it this screws with driver parallesim?

    //output all errors during rendering
    GLenum errorcode;
    while( (errorcode = glGetError()) != GL_NO_ERROR ) {
        cout << "DrawWorld() GL error: " << gluErrorString(errorcode) << endl;
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
    /*
    //again assuming dude 0 is the client
    mylvl.camavg = mylvl.dude[0].pos.c;

    //get direction in x/y
    vector2d v (-sinf(mylvl.dude[0].dirangle), -cosf(mylvl.dude[0].dirangle));

    gluLookAt(mylvl.camavg.x+v.x*5,   mylvl.camavg.y+v.y*5, 2+mylvl.dude[0].pos.c.z,
              mylvl.camavg.x,         mylvl.camavg.y,       1+mylvl.dude[0].pos.c.z,
              0,0,1);
    */

    int entcam = 0;
    if(LC.EntityAddress != -1) entcam = LC.EntityAddress;

    if(LC.lvl.Ent[entcam].isUsed) {
        LC.lvl.camavg = LC.lvl.Ent[entcam].pos.c;
        //cout << "cam x: " << mylvl.Ent[entcam].pos.c.x << " y: " << mylvl.Ent[entcam].pos.c.y << endl;

        vector2d v (-sinf(LC.lvl.Ent[entcam].Yaw), -cosf(LC.lvl.Ent[entcam].Yaw));

        gluLookAt(LC.lvl.camavg.x+v.x*5,   LC.lvl.camavg.y+v.y*5, 2+LC.lvl.Ent[entcam].pos.c.z,
                  LC.lvl.camavg.x,         LC.lvl.camavg.y,       1+LC.lvl.Ent[entcam].pos.c.z,
                  0,0,1);
    }
    else cout << "no cam!" << endl;
}

void DrawLevel() {
    glDisable(GL_CULL_FACE);
    glCallList(LC.lvl.DisplayList);
}

/*Draw all players
void DrawPlayers() {
    glEnable(GL_CULL_FACE);

    //go through the players drawing them one at a time
    for (int i = 0; i < mylvl.dudenum; i++) {
        if (mylvl.dude[i].Alive) {
            //draw the model
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
                //postion and roatate
                glTranslatef(mylvl.dude[i].pos.c.x, mylvl.dude[i].pos.c.y, mylvl.dude[i].pos.c.z);
                //glRotatef( -( mylvl.dude[i].dirangle * (180/3.14159265) ), 0.0, 0.0, 1.0  );

                //draw model
                glCallList(mylvl.dude[i].Model.DisplayList);
            glPopMatrix();
        }
    }
}
*/

void DrawEntities() {
    if(!LC.lvl.Loaded) {
        cout << "DrawEntities: attempting to draw world while no world exist" << endl;
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
    if(!LC.DrawWorld) {
        UI_DrawDesktop();
        if(LC.isConnectedToRemoteServer) UI_DrawUserList();
    }

    //draw messages
    UI_DrawMSGs();

    //draw buttons and such
    UI_DrawButtons();

    //draw lap info
    if(LC.DrawWorld) UI_DrawLapTimes();
}
