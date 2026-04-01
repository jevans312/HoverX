#include <list>
#include <string>
#include <iostream>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "client.h"
#include "glex.h"

using namespace std;

extern LocalClient LC;

struct texidS {
    GLuint id;
    string texname;
};
list<texidS> texlist;
list<texidS>::iterator i;

float GetMaxTextureAnisotropy() {
#if defined(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT) && defined(GL_TEXTURE_MAX_ANISOTROPY_EXT)
    if (SDL_GL_ExtensionSupported("GL_EXT_texture_filter_anisotropic") == SDL_TRUE) {
        GLfloat maxAnisotropy = 1.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
        return maxAnisotropy;
    }
#endif

    return 1.0f;
}

GLuint LoadGLTexture(const char* filename)  {
    string fns = filename;
    for(i = texlist.begin(); i != texlist.end(); i++) {
        if((*i).texname == fns) {
            cout << "texture " << filename << " alredy loaded" << '\n';
            return (*i).id;
        }
    }

    GLuint newid;
    SDL_Surface *loadedSurface;
    loadedSurface = IMG_Load(filename);
    if (loadedSurface != NULL) {
        SDL_Surface *rgbaSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA32, 0);
        if (rgbaSurface == NULL) {
            cout << "LoadGLTexture: Could not convert texture: " << filename
                 << "; Using default.png in it's place" << '\n';
            SDL_FreeSurface(loadedSurface);
            return LC.DefaultTextureID;
        }

        glGenTextures(1, &newid);
        glBindTexture(GL_TEXTURE_2D, newid);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    #if defined(GL_TEXTURE_MAX_ANISOTROPY_EXT)
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, GetMaxTextureAnisotropy());
    #endif
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, rgbaSurface->w, rgbaSurface->h, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, rgbaSurface->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        SDL_FreeSurface(rgbaSurface);

        //add to list of already loaded textures
        texidS ttexid;
        ttexid.texname = filename;
        ttexid.id = newid;
        texlist.push_front(ttexid);
    }
    else {
        cout << "LoadGLTexture: Could not load texture: " << filename
             << "; Using default.png in it's place" << '\n';
        newid = LC.DefaultTextureID;
    }

    //release the SDL object
    if (loadedSurface)
        SDL_FreeSurface(loadedSurface);

    //return texture id
    return newid;
}

void go2d(float x1, float x2,float y1, float y2) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();    // Reset The Projection Matrix

    glOrtho(x1, x2, y1, y2, -1.0f, 1.0f);
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
