#pragma once

#include "glness.h"

// Maximum 3D object limits
constexpr int MAXOBJ_VERTEX  = 5000;
constexpr int MAXOBJ_FACE    = 5000;
constexpr int MAXOBJ_NORMAL  = 5000;
constexpr int MAXOBJ_TEXTURE = 5000;

struct Triangle {
    unsigned short v1, v2, v3;
    unsigned short vt1, vt2, vt3;
    unsigned short vn1, vn2, vn3;
};

struct Vertex {
    float x, y, z;
};

struct Vertex2D {
    float x, y;
};

class objModel {
public:
    objModel();
    ~objModel();

    bool Load(const std::string& modelfile, const std::string& texturefile);
    void Unload();
    void Draw();
    void Clear();

    bool   Loaded = false;
    GLuint DisplayList = 0; // Holds all the data and commands for rendering
    int    TextureID = 0;   // Holds texture id

private:
    // Add private members here if needed
};
