#pragma once

// Loads a texture from file and returns the OpenGL texture ID.
int LoadGLTexture(const char* filename);

// Switches to 2D rendering mode.
void go2d(float x1, float x2, float y1, float y2);

// Restores 3D rendering mode.
void leave2d();
