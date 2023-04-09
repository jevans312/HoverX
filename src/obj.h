#ifndef obj_h
#define obj_h

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

#include "glness.h"
#include "glex.h"
#include "main.h"

using namespace std;

//max 3d object stuff
#define MAXOBJ_VERTEX 5000
#define MAXOBJ_FACE 5000
#define MAXOBJ_NORMAL 5000
#define MAXOBJ_TEXTURE 5000


struct Triangle {
       unsigned short int v1;
       unsigned short int v2;
       unsigned short int v3;
       unsigned short int vt1;
       unsigned short int vt2;
       unsigned short int vt3;
       unsigned short int vn1;
       unsigned short int vn2;
       unsigned short int vn3;
};

struct Vertex {
      float x;
      float y;
      float z;
};

struct Vertex2D {
    float x;
    float y;
};

class objModel {
public:
	bool Load(const string& modelfile, const string& texturefile);
	void Unload();
	//void SetTexture(const string& texturefile);
	void Draw(void);
	void Clear();

    objModel( void );
	~objModel( void );
public:
    bool       Loaded;
    GLuint     DisplayList;    //holds all the data and commands for rendering
    int        TextureID;                              //holds texture id

private:

};

#endif
