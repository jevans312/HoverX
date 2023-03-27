#include "glex.h"
#include"glness.h"
#include"font.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int fonttexture;
int base;
float sizex=1.f;
float sizey=1.f;

void initsimplefont(char *file) {
    float cx, cy;
    float charsize = 1.f/16.f;

    fonttexture = LoadGLTexture(file);

    base=glGenLists(256);
    glBindTexture(GL_TEXTURE_2D, fonttexture);
    for (int i=0; i<256; i++)
    {
        cx=(float)(i%16)/16.0f;
        cy=(float)(i/16)/16.0f;

        glNewList(base+i,GL_COMPILE);
        glBegin(GL_QUADS);
        glTexCoord2f(cx+charsize,1.0f-cy);
        glVertex2i(16,0);
        glTexCoord2f(cx,1.0f-cy);
        glVertex2i(0,0);
        glTexCoord2f(cx,1.0f-cy-charsize);
        glVertex2d(0,16);
        glTexCoord2f(cx+charsize,1.0f-cy-charsize);
        glVertex2i(16,16);
        glEnd();
        glTranslated(12,0,0);
        glEndList();
    }
}

void unloadFont() {
    glDeleteLists(base,256);
}

int printgls(float x, float y, char *args,...)
{

    if (!args) return 1;

    va_list arglist;
    va_start(arglist, args);
    char    finaltext[1024];
    vsprintf(finaltext, args, arglist);
    va_end(arglist);

    glDisable(GL_DEPTH_TEST);
    go2d(0.f, 1, 1, 0.f);

    glBindTexture(GL_TEXTURE_2D, fonttexture);

    glLoadIdentity();
    glTranslated(x, 1 - y,0);   //the 1 - y converts to my cordinate system
    glListBase(base-32);
    glScalef(sizex/512, sizey/512, 1.0f);
    glCallLists(strlen(finaltext),GL_UNSIGNED_BYTE, finaltext);

    leave2d();
    glEnable(GL_DEPTH_TEST);


    return 1;
}
