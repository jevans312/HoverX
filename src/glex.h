#pragma once

#include "glness.h"

// Loads a texture from file and returns the OpenGL texture ID.
GLuint LoadGLTexture(const char* filename);

// Returns the maximum anisotropy level supported by the current GL context.
float GetMaxTextureAnisotropy();

// Switches to 2D rendering mode.
void go2d(float x1, float x2, float y1, float y2);

// Restores 3D rendering mode.
void leave2d();
