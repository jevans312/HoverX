// generated by Fast Light User Interface Designer (fluid) version 1.0102

#include "UI.h"

shape_window *DrawingBox=(shape_window *)0;

Fl_Menu_Bar *asd=(Fl_Menu_Bar *)0;

Fl_Menu_Item menu_asd[] = {
 {"file", 0,  0, 0, 64, 0, 0, 14, 56},
 {"load", 0,  (Fl_Callback*)flload, 0, 0, 0, 0, 14, 56},
 {"save", 0,  (Fl_Callback*)flsave, 0, 0, 0, 0, 14, 56},
 {"reset", 0,  (Fl_Callback*)flreset, 0, 0, 0, 0, 14, 56},
 {"exit", 0,  (Fl_Callback*)flexit, 0, 0, 0, 0, 14, 56},
 {"save as", 0,  (Fl_Callback*)flsaveas, 0, 0, 0, 0, 14, 56},
 {0},
 {"tools", 0,  0, 0, 64, 0, 0, 14, 56},
 {"Textures", 0,  (Fl_Callback*)opentexturewin, 0, 0, 0, 0, 14, 56},
 {0},
 {0}
};

Fl_Value_Input *flvertx=(Fl_Value_Input *)0;

Fl_Value_Input *flverty=(Fl_Value_Input *)0;

Fl_Value_Input *flvertz=(Fl_Value_Input *)0;

Fl_Value_Slider *flvertsize=(Fl_Value_Slider *)0;

Fl_Choice *flverttype=(Fl_Choice *)0;

Fl_Choice *fllinetex=(Fl_Choice *)0;

Fl_Menu_Item menu_fllinetex[] = {
 {"None", 0,  0, 0, 4, 0, 0, 14, 56},
 {0}
};

Fl_Value_Input *flpolyroof=(Fl_Value_Input *)0;

Fl_Value_Input *flpolyfloor=(Fl_Value_Input *)0;

Fl_Choice *flrooftex=(Fl_Choice *)0;

Fl_Menu_Item menu_flrooftex[] = {
 {"None", 0,  0, 0, 4, 0, 0, 14, 56},
 {0}
};

Fl_Choice *flfloortex=(Fl_Choice *)0;

Fl_Menu_Item menu_flfloortex[] = {
 {"None", 0,  0, 0, 4, 0, 0, 14, 56},
 {0}
};

Fl_Button *flpolymode=(Fl_Button *)0;

Fl_Button *flobjmode=(Fl_Button *)0;

Fl_Dial *VertDir=(Fl_Dial *)0;

Fl_Double_Window* make_window() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = new Fl_Double_Window(925, 630, "Ground Height");
    w = o;
    { shape_window* o = DrawingBox = new shape_window(5, 35, 675, 590, "GlWindow");
      o->box(FL_ROUNDED_BOX);
      o->color(49);
      o->selection_color(49);
      o->labeltype(FL_NORMAL_LABEL);
      o->labelfont(0);
      o->labelsize(14);
      o->labelcolor(56);
      o->align(FL_ALIGN_CENTER);
      o->when(FL_WHEN_RELEASE);
    }
    { Fl_Menu_Bar* o = asd = new Fl_Menu_Bar(0, 0, 920, 30);
      o->align(FL_ALIGN_CLIP);
      o->menu(menu_asd);
    }
    { Fl_Group* o = new Fl_Group(695, 100, 215, 255, "Point");
      o->box(FL_SHADOW_BOX);
      { Fl_Value_Input* o = flvertx = new Fl_Value_Input(775, 105, 50, 25, "Y");
        o->color(49);
        o->callback((Fl_Callback*)flsetvertx);
        o->when(FL_WHEN_ENTER_KEY);
      }
      { Fl_Value_Input* o = flverty = new Fl_Value_Input(710, 105, 45, 25, "X");
        o->color(49);
        o->callback((Fl_Callback*)flsetverty);
      }
      { Fl_Value_Input* o = flvertz = new Fl_Value_Input(845, 105, 50, 25, "Z");
        o->color(49);
        o->callback((Fl_Callback*)flsetvertz);
        o->when(FL_WHEN_ENTER_KEY);
      }
      { Fl_Value_Slider* o = flvertsize = new Fl_Value_Slider(705, 220, 180, 30, "vert size");
        o->type(5);
        o->minimum(0.1);
        o->maximum(20);
        o->value(6);
        o->align(FL_ALIGN_TOP);
      }
      { Fl_Choice* o = flverttype = new Fl_Choice(735, 140, 165, 20, "Type");
        o->down_box(FL_BORDER_BOX);
        o->callback((Fl_Callback*)flsetverttype);
      }
      o->end();
    }
    { Fl_Group* o = new Fl_Group(700, 385, 210, 55, "Wall");
      o->box(FL_SHADOW_BOX);
      { Fl_Choice* o = fllinetex = new Fl_Choice(805, 390, 100, 20, "Texture");
        o->down_box(FL_BORDER_BOX);
        o->callback((Fl_Callback*)flsetlinetex);
        o->menu(menu_fllinetex);
      }
      o->end();
    }
    { Fl_Group* o = new Fl_Group(700, 460, 210, 160, "Room");
      o->box(FL_SHADOW_BOX);
      { Fl_Value_Input* o = flpolyroof = new Fl_Value_Input(825, 490, 80, 25, "Roof Height");
        o->callback((Fl_Callback*)flsetroofheight);
      }
      { Fl_Value_Input* o = flpolyfloor = new Fl_Value_Input(825, 520, 80, 25, "Ground Height");
        o->callback((Fl_Callback*)flsetfloorheight);
      }
      { Fl_Choice* o = flrooftex = new Fl_Choice(805, 555, 100, 25, "Roof Texture");
        o->down_box(FL_BORDER_BOX);
        o->callback((Fl_Callback*)flsetrooftex);
        o->menu(menu_flrooftex);
      }
      { Fl_Choice* o = flfloortex = new Fl_Choice(805, 585, 100, 25, "Floor Texture");
        o->down_box(FL_BORDER_BOX);
        o->callback((Fl_Callback*)flsetfloortex);
        o->menu(menu_flfloortex);
      }
      o->end();
    }
    { Fl_Button* o = flpolymode = new Fl_Button(700, 40, 105, 30, "Make Rooms");
      o->down_box(FL_DOWN_BOX);
      o->callback((Fl_Callback*)flsetpolymode);
    }
    { Fl_Button* o = flobjmode = new Fl_Button(810, 40, 105, 30, "Make Objects");
      o->callback((Fl_Callback*)flsetobjmode);
    }
    { Fl_Dial* o = VertDir = new Fl_Dial(705, 170, 35, 35, "Object Direction");
      o->type(1);
      o->minimum(-3);
      o->maximum(3);
      o->callback((Fl_Callback*)flSetVertDir);
      o->align(FL_ALIGN_RIGHT);
      o->step(0.75);
      o->angles(0, 360);
    }
    o->end();
  }
  return w;
}

Fl_Browser *fltexlist=(Fl_Browser *)0;

Fl_Button *flnewtexbutton=(Fl_Button *)0;

Fl_Button *fldeltexbutton=(Fl_Button *)0;

Fl_Double_Window* make_texture_window() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = new Fl_Double_Window(450, 330, "Ground Height: Texture list");
    w = o;
    { Fl_Browser* o = fltexlist = new Fl_Browser(25, 25, 260, 260, "Avalable Textures");
      o->type(2);
      o->box(FL_DOWN_BOX);
      o->align(FL_ALIGN_TOP);
    }
    { Fl_Button* o = flnewtexbutton = new Fl_Button(315, 100, 100, 40, "Add New");
      o->callback((Fl_Callback*)flnewtex);
    }
    { Fl_Button* o = fldeltexbutton = new Fl_Button(295, 160, 140, 25, "Remove Selected");
      o->callback((Fl_Callback*)fldeltex);
    }
    o->end();
  }
  return w;
}
