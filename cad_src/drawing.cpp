#include "hovercad.h"

void glCircle(float x, float y, float radius = 1, int type = GL_LINE_LOOP, int numsides)
{
    glPushMatrix();
    //glTranslatef(x,y,0);
    glBegin(type);
    for(int i = 0;i<numsides;i++)
    {
        float angle = (PI*2.f/numsides)*(float)i;
        float xp = cosf(angle)*radius+x;
        float yp = sinf(angle)*radius+y;
        glVertex3f(xp,yp,0);
    }
    glEnd();
    glPopMatrix();
}


void drawvertexs()  {
    float radius = 3;
    int i = 0;
    while(i < vert_count)   {
        if(verts[i].selected == true)   {   //turn selected verts blue
            glLineWidth(3);
            glColor3f(0,0,1);
            radius = 0.2*(flvertsize->value()+1);
        }
        else if(verts[i].red == true) { //turn the starting vert in poly creation red
            glLineWidth(3);
            glColor3f(1,0,0);
            radius = 0.2*(flvertsize->value()+3);
        } else {    //normal vert
            glLineWidth(1);
            glColor3f(0,0,0);
            radius = 0.2*flvertsize->value();
        }

        //make check points yellow
        if(verts[i].type == CHECK_POINT) glColor3f(1, 1, 0);

        //draw an arrow for starts for direction of spawn
        if(verts[i].type == START_POINT) {
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glTranslatef(verts[i].x, verts[i].y, 0);
            glRotatef( -(verts[i].VertDirection * (180/3.14159265) ), 0.0, 0.0, 1.0  );
            glBegin(GL_TRIANGLES);
                glVertex3f( -1.5f, 0.0f, 0.0f);
                glVertex3f( 1.5f, 0.0f, 0.0f);
                glVertex3f( 0.0f, 2.5f, 0.0f);
            glEnd();
            glPopMatrix();
        }

        glCircle(verts[i].x,verts[i].y,radius);
        i++;
    }

    glLineWidth(1);
}

/*This isnt used
void drawSCF()
{
    float radius = 3;//*cam.z;
    int i = 0;
    while(i < starts.size())    {
        if(starts[i].selected == true)
            glColor3f(0,0,255);
        else
            glColor3f(0,0,0);

        glCircle(starts[i].x,starts[i].y,radius);
        i++;
    }
    i = 0;
    while(i < checks.size())
    {
        if(checks[i].selected == true)
            glColor3f(0,0,255);
        else
            glColor3f(0,0,0);

        glCircle(checks[i].x,checks[i].y,radius);
        i++;
    }
    i = 0;
    while(i < finishes.size())
    {
        if(finishes[i].selected == true)
            glColor3f(0,0,255);
        else
            glColor3f(0,0,0);

        glCircle(finishes[i].x,finishes[i].y,radius);
        i++;
    }
}
*/

void DrawGrid() {
    glColor3f(1,1,1);
    glLineWidth(1);

    for(int nextline = -1000; nextline < 1000; nextline = nextline + 5){
        glBegin(GL_LINES);
        glVertex3f(-1000,nextline,5);
        glVertex3f(1000, nextline,5);
        glEnd();

        glBegin(GL_LINES);
        glVertex3f(nextline, -1000,5);
        glVertex3f(nextline, 1000,5);
        glEnd();

    }
}

void drawlines()
{
    int i = 0;
    float dxa,dya,dxb,dyb;

    glColor3f(0,0,0);
    while(i < line_count)
    {
        //complete line
        if(lines[i].a != -1 && lines[i].b != -1)
        {
            dxa = verts[lines[i].a].x;
            dya = verts[lines[i].a].y;
            dxb = verts[lines[i].b].x;
            dyb = verts[lines[i].b].y;
            glColor3f(0,0,0);
            glLineWidth(3);

            if(lines[i].selected)   {
                glColor3f(0,0,1);
                glLineWidth(3);
            }
            if(lines[i].red) {
                glColor3f(1,0,0);
                glLineWidth(3);
            }

            glBegin(GL_LINES);
            glVertex3f(dxa,dya,0);
            glVertex3f(dxb,dyb,0);
            glEnd();

            glCircle((dxa+dxb)/2,(dya+dyb)/2, 1.5);//flvertsize->value()*cam.z/2);
        }
        i++;
    }
    i = 0;
    glLineWidth(1);
}

void drawpolys()
{
    int i = 1;
    int p = 0;

    while(p < poly_count)
    {
        if(polys[p].complete)
        {
            //light green
            if(!polys[p].selected)
            {
                glColor3ub(180, 255, 180);
            }
            else
            {
                glColor3ub(0, 255, 0);
            }
            float cx = 0;
            float cy = 0;
            glBegin(GL_POLYGON);

            if(lines[polys[p].p[i]].a == lines[polys[p].p[i-1]].a ||
                    lines[polys[p].p[i]].a == lines[polys[p].p[i-1]].b)
            {
                glVertex3f(verts[ lines[polys[p].p[i]].a ].x, verts[ lines[polys[p].p[i]].a ].y,0);
            }
            else
            {
                glVertex3f(verts[ lines[polys[p].p[i]].b ].x, verts[ lines[polys[p].p[i]].b ].y,0);
            }

            while(i < polys[p].sidecount+1)
            {
                if(lines[polys[p].p[i]].a == lines[polys[p].p[i-1]].a ||
                        lines[polys[p].p[i]].a == lines[polys[p].p[i-1]].b)
                {
                    glVertex3f(verts[ lines[polys[p].p[i]].b ].x, verts[ lines[polys[p].p[i]].b ].y,0);
                }
                else
                {
                    glVertex3f(verts[ lines[polys[p].p[i]].a ].x, verts[ lines[polys[p].p[i]].a ].y,0);
                }
                i++;
            }
            glEnd();

            i=0;
            while(i < polys[p].sidecount+1)
            {
                cx+=verts[ lines[polys[p].p[i]].a ].x;
                cy+= verts[ lines[polys[p].p[i]].a ].y;
                cx+=verts[ lines[polys[p].p[i]].b ].x;
                cy+= verts[ lines[polys[p].p[i]].b ].y;
                i++;
            }
            i = 1;
            cx/=(polys[p].sidecount+1)*2;
            cy/=(polys[p].sidecount+1)*2;
            glColor3ub(0, 0, 180);
            glCircle(cx,cy,flvertsize->value()*cam.z/2);

        }
        p++;
    }
}


void ExtDraw()
{
    //cout << " drawing" << endl;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    float tempscalex = 2.f/(float)DrawingBox->w()/cam.z;
    float tempscaley = 2.f/(float)DrawingBox->h()/cam.z;
    glScalef(tempscalex,tempscaley,0.01f);
    /*gluLookAt(cam.x,cam.y,cam.z,
              0,0,0,//cam.x,cam.y,0,
              0,0,1);*/
    glTranslatef(cam.x,cam.y,cam.z);

    glColor3f(1.f,1.f,1.f);
    DrawGrid();
    drawpolys();
    drawlines();
    drawvertexs();
    //drawSCF();

    /*
        glTranslatef(mouse.x,mouse.y,0);
        glScalef(cam.z,cam.z,cam.z);
        glColor3f(1.f,1.f,1.f);
        glBegin(GL_QUADS);
        glVertex3f(-3.0,   0.0,    0.0);
        glVertex3f(0.0,    3.0,    0.0);
        glVertex3f(3.0,    0.0,    0.0);
        glVertex3f(0.0,    -3.0,    0.0);
        glEnd();
    */

    static int oldw = 0;
    static int oldh = 0;
    if (DrawingBox->w() != oldw || DrawingBox->h() != oldh)
    {
        DrawingBox->valid(0);
        cout << "resizing w is " << 1.f/(float)DrawingBox->h() << endl;
        oldw = DrawingBox->w();
        oldh = DrawingBox->h();
    }

}
