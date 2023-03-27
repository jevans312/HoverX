#include <iostream>
using namespace std;

#include <FL/Fl.H>
#include <FL/gl.h>
//#include <GL/glu.h>
#include <FL/Fl_Gl_Window.H>
#include <FL/math.h>

#include "MyWindow.H"


#include <math.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

/*void glooPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
   GLdouble xmin, xmax, ymin, ymax;

   ymax = zNear * tan(fovy * M_PI / 360.0);
   ymin = -ymax;
   xmin = ymin * aspect;
   xmax = ymax * aspect;


   glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}*/

void shape_window::draw()
{
/*#ifdef WIN32
Sleep(50);
#endif*/
    // the valid() property may be used to avoid reinitializing your
    // GL transformation for each redraw:
    if (!valid())
    {
        //we need to initilise opengl
        valid(1);
        cout << " initing opengl window" << endl;
        glLoadIdentity();
        glViewport(0,0,w(),h());
        cout << "w is " << w() << " h is " << h() << endl;
        glMatrixMode (GL_PROJECTION);
        glLoadIdentity ();
        //gluPerspective(45.0f,w()/h(),0,100);
        glOrtho(0,0,w(),h(),-100,100);
        glMatrixMode (GL_MODELVIEW);
        glLoadIdentity ();


        glClearColor(0.8f, 0.8f, 1.0f, 1.0);
        //glEnable(GL_COLOR_MATERIAL);

        //glClearColor (0.0f, 0.0f, 0.0f, 0.5f);      // Black Background
        glClearDepth (1.0f);          // Depth Buffer Setup
        glDepthFunc (GL_LEQUAL);         // The Type Of Depth Testing (Less Or Equal)
        glEnable (GL_DEPTH_TEST);         // Enable Depth Testing
        glShadeModel (GL_SMOOTH);         // Select Smooth Shading
        glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Set Perspective Calculations To Most Accurate

        // Setup lighting
        /*glEnable(GL_LIGHTING);
        float AmbientColor[] = { 0.0f, 0.1f, 0.2f, 0.0f };  glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
        float DiffuseColor[] = { 1.0f, 1.0f, 1.0f, 0.0f };  glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
        float SpecularColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };  glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
        float Position[]  = { 100.0f, 100.0f, 400.0f, 1.0f };  glLightfv(GL_LIGHT0, GL_POSITION, Position);
        glEnable(GL_LIGHT0);*/

    }
    ExtUpdate();
    ExtDraw();
}
int shape_window::handle(int event)
{
    switch(event)
    {
        case FL_PUSH:
        MouseBtn(1, Fl::event_button(), Fl::event_x(), Fl::event_y());
        focus(this);
        return 1;
        case FL_DRAG:
        MouseMove(Fl::event_x(), Fl::event_y());
        return 1;
        case FL_MOVE://TODO why dont this seem to work?
        cout << "mouse moved" << endl;
        MouseMove(Fl::event_x(), Fl::event_y());
        return 1;
        case FL_RELEASE:
        MouseBtn(0, Fl::event_button(), Fl::event_x(), Fl::event_y());
        return 1;
        case FL_MOUSEWHEEL:
        MouseWheel(Fl::event_dx(), Fl::event_dy());
        return 1;
        case FL_FOCUS :
        return 1;
        case FL_UNFOCUS :
        //... Return 1 if you want keyboard events, 0 otherwise
        return 1;
        case FL_KEYDOWN:
        //... keypress, key is in Fl::event_key(), ascii in Fl::event_text()
        //... Return 1 if you understand/use the keyboard event, 0 otherwise...
        if (Fl::event_key() > 256)
        {
            KeyPress(1, 65536-Fl::event_key());
        }
        else
        {
            KeyPress(1, Fl::event_key());
        }
        return 1;
        case FL_KEYUP:
        //... keypress, key is in Fl::event_key(), ascii in Fl::event_text()
        //... Return 1 if you understand/use the keyboard event, 0 otherwise...
        if (Fl::event_key() > 256)
        {
            KeyPress(0, /*65536-*/Fl::event_key());
        }
        else
        {
            KeyPress(0, Fl::event_key());
        }
        return 1;
        case FL_SHORTCUT:
        //... shortcut, key is in Fl::event_key(), ascii in Fl::event_text()
        //... Return 1 if you understand/use the shortcut event, 0 otherwise...
        return 0;
        default:
        // tell FLTK that I don't understand other events
        return 0;
    }
}
