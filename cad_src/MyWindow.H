//#ifndef MYWINDOW_H
//#define MYWINDOW_H


#include <FL/gl.h>
#include <GL/glu.h>
#include <FL/Fl_Gl_Window.H>
#include <FL/math.h>

extern void ExtUpdate();
extern void ExtDraw();
extern void MouseBtn(bool press, int button, float x, float y);
extern void MouseMove(float x, float y);
extern void MouseWheel(int x, int y);
extern void KeyPress(bool press, int ascii);

class shape_window : public Fl_Gl_Window {
  void draw();
  int handle(int event);
  int summin;
public:
shape_window(int X, int Y, int W, int H, const char *L)
    : Fl_Gl_Window(X, Y, W, H, L) {}
};


//#endif // MYWINDOW_H
