#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "string.h"

#include "glness.h"
#include "glex.h"

#include <list>
#include <iostream>
using namespace std;

GLuint newid;
extern int DefaultTextureID;

struct texidS
{
    int id;
    string texname;
};

list<texidS> texlist;
list<texidS>::iterator i;
int LoadGLTexture(char*filename)  {
    string fns = filename;
    for(i=texlist.begin();i!=texlist.end();i++)
    {
        if((*i).texname == fns) {
            cout << "texture " << filename << " alredy loaded" << endl;
            return (*i).id;
        }
    }

    SDL_Surface *TextureImage;
    TextureImage = IMG_Load(filename);
    if (TextureImage != NULL) {
        //printf("%s loading ok\n",filename);

        glGenTextures( 1, &newid );

        GLenum pfmat = GL_RGB;
        int iforget = 3;
        if(TextureImage->format->BitsPerPixel == 32)
        {
            pfmat = GL_RGBA;
            iforget = 4;
        }


        glBindTexture( GL_TEXTURE_2D, newid );
        glTexImage2D( GL_TEXTURE_2D, 0, iforget, TextureImage->w,
                      TextureImage->h, 0, pfmat,
                      GL_UNSIGNED_BYTE, TextureImage->pixels );

        //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
        gluBuild2DMipmaps(GL_TEXTURE_2D, iforget, TextureImage->w, TextureImage->h, pfmat, GL_UNSIGNED_BYTE, TextureImage->pixels);

    }
    else {
        cout << "could not load texture: " << filename << " replaced with default.png" << endl;
        return DefaultTextureID;
        //printf("TEXTURE ERROR\n");//should return 0 too., exit or use default texture
    }

    //release the SDL object
    if (TextureImage)
        SDL_FreeSurface(TextureImage);

    texidS ttexid;
    ttexid.texname = filename;
    ttexid.id = newid;
    texlist.push_front(ttexid);

    //return texture id
    return newid;
}

void drawquad(float x, float y, float z, float scale)
{
    glPushMatrix();
    glTranslatef(x,y,z);
    glScalef(scale,scale,scale);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(  1.f,-1.f,0.f );
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(  1.f, 1.f,0.f );
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f( -1.f, 1.f,0.f );
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f( -1.f,-1.f,0.f );
    glEnd();
    glPopMatrix();
}



void go2d(float x1, float x2,float y1, float y2)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();    // Reset The Projection Matrix

    glOrtho(x1,x2,y1,y2,-1.f,1.f);
    //glOrtho(-1,1,-1,1,-1,1);

    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    glLoadIdentity();    // Reset The Projection Matrix
}

void leave2d()
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
