
/********** HCUToolkit :: hcuReader **********
 *
 * Purpose:  Read .hcu files and return a manager object containing all of the data in the file.
 * Used by:  No classes in HCUToolkit.
 * Uses:   manager, to save all the data.
 * Note:   Try to open a file that isn't in the correct format, and it'll just return an empty manager.
 *
 * Author: Ryan Curtin
 * Date: 02/21/2005
 */
#include "hovercad.h"
 #include <iostream>

#include <fstream>
#include <ios>
// Public
bool importinghcu = 0;

void youveGotError()
{
    cout << "error" << endl;
    //MessageBox(NULL, "There was an error opening the file.", "Prepare for crash!", MB_OK);
}

void readhcufile(char *filename)
{
    importinghcu = 1;
    fstream f(filename, std::ios::in); // Get the file open and ready to be read.
    if(!f.is_open()) // If the file isn't there,
        return; //ERROR NO FILE
    char buffer[500];
    string temp;
    string tempb;
    f.getline(buffer, 500); // [Header]
    temp = buffer;
    tempb = temp.erase(8, 9);//iggly didnt need this, but i do? :S
    cout << temp << endl;
    if(temp != "[Header]")  // Test to see if it's [Header].
        return; //ERROR NO HEADER
    f.getline(buffer, 500); // Description=
    temp = buffer;
    if(temp.find("Description=", 0) == string::npos) // Anything like this is just checking for the right syntax.
        youveGotError(); // If it finds an error, let the user know that they're stupid.
    temp = temp.erase(0, 12); // Narrow down temp object to just the description
    //m->setDescription((char *) temp.c_str());
    //DESCRPTION = temp.c_str()
    f.getline(buffer, 500); // Background=
    temp = buffer;
    if(temp.find("Background=", 0) == string::npos) // Syntax check
        youveGotError();
    temp = temp.erase(0, 11); // Narrow down string to just background
    //m->setBackground((char *) temp.c_str());
    //BACKGROUND = temp.c_str()
    f.getline(buffer, 500); // blank line
    bool done = false;
    bool doneWalls = false;
    cout << "CHECKPOINT!!" << endl;
    float floorh, roofh;
    while(f.getline(buffer, 500))
    {
        //f.getline(buffer, 500); // [Room], [Feature], whatever
        if(strncmp(buffer, "[Room]",6) == 0/* || strncmp(buffer, "[Feature]",9) == 0*/)
        {
            //room *r = new room();
            f.getline(buffer, 500); // Id=
            int roomid;
            sscanf(buffer, "Id=%i", &roomid);
            //if(roomid == 77)return;
            cout << "this rooms id is " << roomid << endl;
            f.getline(buffer, 500); // floor=
            sscanf(buffer, "floor=%f", &floorh);
            //tempb = new System::String((signed char *) buffer);
            //r = getFloor(r, tempb);
            f.getline(buffer, 500); // ceiling=
            sscanf(buffer, "ceiling=%f", &roofh);
            //tempb = new System::String((signed char *) buffer);
            //r = getCeiling(r, tempb);*/
            doneWalls = false;
            float fx,fy;
            bool firstpoint = 1;
            while(!doneWalls)
            {
                f.getline(buffer, 500); // Wall=
                tempb = buffer;
                //tempb = temp.erase(0, 5);
                //cout << " tempb is: " << tempb << endl;
                if(tempb.c_str()[0] != 'w')
                {
                    doneWalls = true;
                    cout << "walls are " << fx << " and " << fy << endl;
                    polys[poly_count].floor = floorh;
                    polys[poly_count].roof = roofh;
                    assemble_poly(fx,fy,1);
                }
                else
                {
                    float w1,w2;
                    float i1,i2;
                    sscanf(tempb.c_str(), "wall=%f, %f, %i, %i",&w1,&w2,&i1,&i2);
                    cout << "walls are " << w1 << " and " << w2 << endl;
                    //r = getWall(r, tempb);
                    if(firstpoint)
                    {
                        fx = w1;
                        fy = w2;
                        firstpoint = 0;
                    }
                    assemble_poly(w1,w2);
                }
            }
            //done = 1;
            //break;
            //m->addRoom(*r);
        }
    }
    /*
     else if(strcmp(buffer, "[Feature]") == 0) {
      feature *fe = new feature();
      f.getline(buffer, 500); // Id=
      f.getline(buffer, 500); // Parent=
      tempb = new System::String((signed char *) buffer);
      fe = getParent(fe, tempb);
      f.getline(buffer, 500); // floor=
      tempb = new System::String((signed char *) buffer);
      fe = getFloor(fe, tempb);
      f.getline(buffer, 500); // ceiling=
      tempb = new System::String((signed char *) buffer);
      fe = getCeiling(fe, tempb);
      doneWalls = false;
      while(!doneWalls) {
       f.getline(buffer, 500); // Wall=
       tempb = new System::String((signed char*) buffer);
       if(tempb->IndexOf("wall=") == -1)
        doneWalls = true;
       else {
        fe = getWall(fe, tempb);
       }
      }
      m->addFeature(*fe);
     }
     else if(strcmp(buffer, "[Free_Element]") == 0) {
      element *e = new element();
      f.getline(buffer, 500); // Section=
      tempb = new System::String((signed char *) buffer);
      e = getSection(e, tempb);
      f.getline(buffer, 500); // Position=
      tempb = new System::String((signed char *) buffer);
      e = getPosition(e, tempb);
      f.getline(buffer, 500); // Orientation=
      tempb = new System::String((signed char *) buffer);
      e = getOrientation(e, tempb);
      f.getline(buffer, 500); // Type=
      tempb = new System::String((signed char *) buffer);
      e = getType(e, tempb);
      f.getline(buffer, 500); // empty line
      m->addElement(*e);
     }
     else if(strcmp(buffer, "[Initial_Position]") == 0) {
      initpos *i = new initpos();
      f.getline(buffer, 500); // Section=
      tempb = new System::String((signed char *) buffer);
      i = getSection(i, tempb);
      f.getline(buffer, 500); // Position=
      tempb = new System::String((signed char *) buffer);
      i = getPosition(i, tempb);
      f.getline(buffer, 500); // Orientation=
      tempb = new System::String((signed char *) buffer);
      i = getOrientation(i, tempb);
      f.getline(buffer, 500); // Team=
      tempb = new System::String((signed char *) buffer);
      i = getTeam(i, tempb);
      f.getline(buffer, 500); // empty line
      m->setInitPos(*i);
     }
     else if(strcmp(buffer, "[Connection_List]") == 0) {
      doneWalls = false; // even though I really mean doneConns, doesn't matter
      while(!doneWalls) {
       f.getline(buffer, 500); // connection
       if(strcmp(buffer, "") == 0) {
        doneWalls = true;
        done = true;
       }
       else {
        tempb = new System::String((signed char *) buffer);
        m->addConn(*getConnection(tempb));
       }
      }
     }
    }*/
    importinghcu = 0;
    return;
}


/*
room *hcuReader::getFloor(room *r, System::String *f) {
 if(f->IndexOf("floor= ") == -1)
  youveGotError();
 f = f->Remove(0, 7); // get rid of 'floor= '
 r->setFloor((float) System::Double::Parse(f->Substring(0, f->IndexOf(",")))); // x.xxxxxx
 f = f->Remove(0, f->IndexOf(",") + 2); // remove x.xxxxxx, leave y, z
 r->setFloorDLL(System::Int32::Parse(f->Substring(0, f->IndexOf(",")))); // y
 f = f->Remove(0, f->IndexOf(",") + 2); // remove y
 r->setFloorTex(System::Int32::Parse(f)); // and use z
 return r;
}

feature *hcuReader::getFloor(feature *fe, System::String *f) {
 if(f->IndexOf("floor= ") == -1)
  youveGotError();
 f = f->Remove(0, 7); // get rid of 'floor= '
 fe->setFloor((float) System::Double::Parse(f->Substring(0, f->IndexOf(",")))); // x.xxxxxx
 f = f->Remove(0, f->IndexOf(",") + 2); // remove x.xxxxxx, leave y, z
 fe->setFloorDLL(System::Int32::Parse(f->Substring(0, f->IndexOf(",")))); // y
 f = f->Remove(0, f->IndexOf(",") + 2); // remove y
 fe->setFloorTex(System::Int32::Parse(f)); // and use z
 return fe;
}

room *hcuReader::getCeiling(room *r, System::String *f) {
 if(f->IndexOf("ceiling= ") == -1)
  youveGotError();
 f = f->Remove(0, 9); // get rid of 'ceiling= '
 r->setCeil((float) System::Double::Parse(f->Substring(0, f->IndexOf(",")))); // x.xxxxxx
 f = f->Remove(0, f->IndexOf(",") + 2); // remove x.xxxxxx, leave y, z
 r->setCeilDLL(System::Int32::Parse(f->Substring(0, f->IndexOf(","))));
 f = f->Remove(0, f->IndexOf(",") + 2); // remove y
 r->setCeilTex(System::Int32::Parse(f));
 return r;
}

feature *hcuReader::getCeiling(feature *fe, System::String *f) {
 if(f->IndexOf("ceiling= ") == -1)
  youveGotError();
 f = f->Remove(0, 9); // get rid of 'ceiling= '
 fe->setCeil((float) System::Double::Parse(f->Substring(0, f->IndexOf(",")))); // x.xxxxxx
 f = f->Remove(0, f->IndexOf(",") + 2); // remove x.xxxxxx, leave y, z
 fe->setCeilDLL(System::Int32::Parse(f->Substring(0, f->IndexOf(","))));
 f = f->Remove(0, f->IndexOf(",") + 2); // remove y
 fe->setCeilTex(System::Int32::Parse(f));
 return fe;
}

room *hcuReader::getWall(room *r, System::String *f) {
 f = f->Remove(0, 5); // get rid of 'wall='
 float x = (float) System::Double::Parse(f->Substring(0, f->IndexOf(",")));
 f = f->Remove(0, f->IndexOf(",") + 2);
 float y = (float) System::Double::Parse(f->Substring(0, f->IndexOf(",")));
 f = f->Remove(0, f->IndexOf(",") + 2);
 int d = System::Int32::Parse(f->Substring(0, f->IndexOf(",")));
 f = f->Remove(0, f->IndexOf(",") + 2);
 int t = System::Int32::Parse(f);
 wall w(x, y, d, t);
 r->addNode(w);
 return r;
}

feature *hcuReader::getWall(feature *fe, System::String *f) {
 f = f->Remove(0, 5); // get rid of 'wall='
 float x = (float) System::Double::Parse(f->Substring(0, f->IndexOf(",")));
 f = f->Remove(0, f->IndexOf(",") + 2);
 float y = (float) System::Double::Parse(f->Substring(0, f->IndexOf(",")));
 f = f->Remove(0, f->IndexOf(",") + 2);
 int d = System::Int32::Parse(f->Substring(0, f->IndexOf(",")));
 f = f->Remove(0, f->IndexOf(",") + 2);
 int t = System::Int32::Parse(f);
 wall w(x, y, d, t);
 fe->addNode(w);
 return fe;
}

feature *hcuReader::getParent(feature *f, System::String *g) {
 g = g->Remove(0, 7);
 f->setParentRoom(System::Int32::Parse(g));
 return f;
}

element *hcuReader::getSection(element *e, System::String *f) {
 f = f->Remove(0, 8);
 e->setRoomID(System::Int32::Parse(f));
 return e;
}

initpos *hcuReader::getSection(initpos *i, System::String *f) {
 f = f->Remove(0, 8);
 i->setRoomID(System::Int32::Parse(f));
 return i;
}

element *hcuReader::getPosition(element *e, System::String *f) {
 f = f->Remove(0, 10);
 e->setX((float) System::Double::Parse(f->Substring(0, f->IndexOf(","))));
 f = f->Remove(0, f->IndexOf(",") + 2);
 e->setY((float) System::Double::Parse(f->Substring(0, f->IndexOf(","))));
 f = f->Remove(0, f->IndexOf(",") + 2);
 e->setZ((float) System::Double::Parse(f));
 return e;
}

initpos *hcuReader::getPosition(initpos *i, System::String *f) {
 f = f->Remove(0, 10);
 i->setX((float) System::Double::Parse(f->Substring(0, f->IndexOf(","))));
 f = f->Remove(0, f->IndexOf(",") + 2);
 i->setY((float) System::Double::Parse(f->Substring(0, f->IndexOf(","))));
 f = f->Remove(0, f->IndexOf(",") + 2);
 i->setZ((float) System::Double::Parse(f));
 return i;
}


element *hcuReader::getOrientation(element *e, System::String *f) {
 f = f->Remove(0, 13);
 e->setAngle((float) System::Double::Parse(f));
 return e;
}

initpos *hcuReader::getOrientation(initpos *i, System::String *f) {
 f = f->Remove(0, 13);
 i->setAngle((float) System::Double::Parse(f));
 return i;
}

element *hcuReader::getType(element *e, System::String *f) {
 f = f->Remove(0, 14);
 e->setDll(System::Int32::Parse(f->Substring(0, f->IndexOf(","))));
 f = f->Remove(0, f->IndexOf(",") + 2);
 e->setType(System::Int32::Parse(f));
 return e;
}

initpos *hcuReader::getTeam(initpos *i, System::String *f) {
 f = f->Remove(0, 6);
 i->setPlayer(System::Int32::Parse(f));
 return i;
}

conn *hcuReader::getConnection(System::String *f) {
 unsigned long aa, ba;
 int ab, bb;
 aa = (unsigned long) System::Int64::Parse(f->Substring(0, f->IndexOf(",")));
 f = f->Remove(0, f->IndexOf(",") + 2);
 ab = System::Int32::Parse(f->Substring(0, f->IndexOf(",")));
 f = f->Remove(0, f->IndexOf(",") + 2);
 ba = (unsigned long) System::Int64::Parse(f->Substring(0, f->IndexOf(",")));
 f = f->Remove(0, f->IndexOf(",") + 2);
 bb = System::Int32::Parse(f);
 conn *c = new conn(aa, ab, ba, bb);
 return c;
}
*/
