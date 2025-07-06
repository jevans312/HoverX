#pragma once

// Initialize the simple font system with a font file
void initsimplefont(const char *file);

// Print formatted text at (x, y) using OpenGL
int printgls(float x, float y, const char *fmt, ...);

// Optionally, add this if you need scaling functionality
// void setscale(float x, float y);
