#include <list>
#include <string>
#include <iostream>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "glness.h"
#include "glex.h"

using namespace std;

extern int DefaultTextureID;

struct texidS {
    int id;
    string texname;
};
list<texidS> texlist;
list<texidS>::iterator i;

int LoadGLTexture(char*filename)  {
    string fns = filename;
    for(i = texlist.begin(); i != texlist.end(); i++) {
        if((*i).texname == fns) {
            cout << "texture " << filename << " alredy loaded" << '\n';
            return (*i).id;
        }
    }

    GLuint newid;
    SDL_Surface *TextureImage;
    TextureImage = IMG_Load(filename);
    if (TextureImage != NULL) {
        GLenum pfmat = GL_RGB;  //int iforget = 3;
        if(TextureImage->format->BitsPerPixel == 32) {
            pfmat = GL_RGBA; //iforget = 4;
        }

        glGenTextures(1, &newid);
        glBindTexture(GL_TEXTURE_2D, newid);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
        glTexImage2D(GL_TEXTURE_2D, 0, pfmat, TextureImage->w,TextureImage->h, 0,
                            pfmat, GL_UNSIGNED_BYTE, TextureImage->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        //gluBuild2DMipmaps(GL_TEXTURE_2D, pfmat, TextureImage->w, TextureImage->h,
        //pfmat, GL_UNSIGNED_BYTE, TextureImage->pixels);

        //add to list of already loaded textures
        texidS ttexid;
        ttexid.texname = filename;
        ttexid.id = newid;
        texlist.push_front(ttexid);
    }
    else {
        cout << "LoadGLTexture: Could not load texture: " << filename
             << "; Using default.png in it's place" << '\n';
        newid = DefaultTextureID;
    }

    //release the SDL object
    if (TextureImage)
        SDL_FreeSurface(TextureImage);

    //return texture id
    return newid;
}

void go2d(float x1, float x2,float y1, float y2) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();    // Reset The Projection Matrix

    glOrtho(x1,x2,y1,y2,-1.f,1.f);
    //glOrtho(-1,1,-1,1,-1,1);

    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    glLoadIdentity();    // Reset The Projection Matrix
}

void leave2d() {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
