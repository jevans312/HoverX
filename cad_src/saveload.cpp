#include "hovercad.h"

////////////////////////////////////
///// LOADING///////////////
/////////////////////////////////
int loadxmlfile(char*filename)
{
    xmlfile hoverlvl;
    TiXmlElement *xLevel = hoverlvl.getxmlfirstelement(filename);
    {//these braces indicate an xml file is open


    TiXmlElement *xTextures = hoverlvl.getelement(xLevel, "textures");//TODO editor texture pallete thing!

        TiXmlElement *xTexi = xTextures->FirstChildElement();
        ClearTextureList();
        char texturename[100];
        while( xTexi ) {
            sprintf(texturename, "%s\0", xTexi->Attribute("texture"));
            AddTexture(texturename);
            xTexi = xTexi->NextSiblingElement();
            cout << "loaded " << texturename << endl;
        }


        /*
        TiXmlElement *xStarts = hoverlvl.getelement(xLevel, "starts");
        TiXmlElement *xChecks = hoverlvl.getelement(xLevel, "checkpoints");
        TiXmlElement *xFinishes = hoverlvl.getelement(xLevel, "finish");
        */

        TiXmlElement *xVerts = hoverlvl.getelement(xLevel, "verts");
        TiXmlElement *xLines = hoverlvl.getelement(xLevel, "lines");
        TiXmlElement *xPolys = hoverlvl.getelement(xLevel, "polys");

        vert_count = hoverlvl.countchildren(xVerts);
        line_count = hoverlvl.countchildren(xLines);
        poly_count = hoverlvl.countchildren(xPolys);
        TiXmlElement *xi;
        TiXmlElement *xii;

        //should say verts = new vertclass[vert_count]; here
        vert tempvert;
        vert_count = 0;
        xi = xVerts->FirstChildElement();
        while( xi ) {
                vert tempvert;
                verts.push_back(tempvert);
                verts[vert_count].x = atof(xi->Attribute("x"));
                verts[vert_count].y = atof(xi->Attribute("y"));
                verts[vert_count].type = (vert_type)atoi(xi->Attribute("type"));
                if( verts[vert_count].type == START_POINT ) //only load this if we have a start point
                    verts[vert_count].VertDirection = atof(xi->Attribute("dir"));
                vert_count++;

                xi = xi->NextSiblingElement();
        }

        //should say lines = new lineclass[line_count]; here
        line_count = 0;
        xi = xLines->FirstChildElement();
        line templine;
        while( xi )
        {
            lines.push_back(templine);
            lines[line_count].a = atoi(xi->Attribute("v1"));
            lines[line_count].b = atoi(xi->Attribute("v2"));
            line_count++;
            xi = xi->NextSiblingElement();
        }

        //should say polys = new polyclass[poly_count]; here
        poly_count = 0;
        xi = xPolys->FirstChildElement();
        while( xi )
        {

            poly temppoly;
            polys.push_back(temppoly);
            polys[poly_count].sidecount =  hoverlvl.countchildren(xi);
            //alloc poly sides p = new int[sidenum] yaknow

            polys[poly_count].floortex = atoi(xi->Attribute("ftex"));
            polys[poly_count].rooftex = atoi(xi->Attribute("rtex"));
            polys[poly_count].floor = atof(xi->Attribute("floor"));
            polys[poly_count].roof = atof(xi->Attribute("roof"));
            polys[poly_count].sidecount = 0;
            xii = xi->FirstChildElement();
            while( xii )
            {
                polys[poly_count].p[polys[poly_count].sidecount] = atoi(xii->Attribute("l"));
                polys[poly_count].tex[polys[poly_count].sidecount] = atoi(xii->Attribute("tex"));

                /*cout << "read for file poly#" << poly_count
                << " side #" << polys[poly_count].sidecount << " texture#"
                << polys[poly_count].tex[polys[poly_count].sidecount] << endl;*/

                polys[poly_count].sidecount++;
                xii = xii->NextSiblingElement();
            }
            polys[poly_count].sidecount+=-1;
            polys[poly_count].complete = 1;


            poly_count++;
            xi = xi->NextSiblingElement();

        }
        /*
        xi = xStarts->FirstChildElement();
        while( xi )
        {
            verts.push_back(tempvert);

            verts[vert_count].x = atoi(xi->Attribute("x"));
            verts[vert_count].y = atoi(xi->Attribute("y"));
            verts[vert_count].type = START_POINT;

            cout << "loaded vert #" << vert_count << " type " << verts[vert_count].type << endl;
            vert_count++;
            xi = xi->NextSiblingElement();
        }
        xi = xChecks->FirstChildElement();
        while( xi )
        {
            verts.push_back(tempvert);
            verts[vert_count].x = atoi(xi->Attribute("x"));
            verts[vert_count].y = atoi(xi->Attribute("y"));
            verts[vert_count].type = CHECK_POINT;
            //checks.push_back(tempvert);
            //verts.push_back(tempvert);
            vert_count++;
            xi = xi->NextSiblingElement();
        }
        xi = xFinishes->FirstChildElement();
        while( xi )
        {
            verts.push_back(tempvert);
            verts[vert_count].x = atoi(xi->Attribute("x"));
            verts[vert_count].y = atoi(xi->Attribute("y"));
            verts[vert_count].type = START_FINISH;
            //finishes.push_back(tempvert);
            //verts.push_back(tempvert);
            vert_count++;
            xi = xi->NextSiblingElement();
        }
        */

    }
    hoverlvl.endxml();
}

////////////////////////////////////
///// SAVING//////////////////
/////////////////////////////////
int savexmlfile(char*filename)
{
    char chartowrite[100];

    TiXmlDocument* xmlDoc = new TiXmlDocument(filename);
    TiXmlElement* rootnode = new TiXmlElement("root");
    xmlDoc->LinkEndChild( rootnode );


    TiXmlElement *xLvl   = new TiXmlElement("lvl");
    xLvl->SetAttribute( "name",  "Test Level!");
    rootnode->LinkEndChild( xLvl );


    TiXmlElement *xTextures = new TiXmlElement("textures");
    xTextures->SetAttribute( "skytex",  "img/skyline.jpg");
    rootnode->LinkEndChild( xTextures);


    TiXmlElement *xTexi;

    //save textures
    for(int i = 0; i < TextureListCount; i++) {
        xTexi = new TiXmlElement("add");
        xTexi->SetAttribute( "texture", TextureList[i].c_str());
        //cout << "wrote texture #" << i << " called " << TextureList[i] << endl;
        xTextures->LinkEndChild( xTexi);
    }
    /*
    TiXmlElement *xStarts = new TiXmlElement("starts");
    TiXmlElement *xChecks = new TiXmlElement("checkpoints");
    TiXmlElement *xFinishs = new TiXmlElement("finish");

    rootnode->LinkEndChild( xStarts );
    rootnode->LinkEndChild( xChecks );
    rootnode->LinkEndChild( xFinishs);
    TiXmlElement *xSCFi;

    for(int i = 0; i < vert_count; i++) {
        if(verts[i].type == 1) {
            xSCFi   = new TiXmlElement("add");
            sprintf(chartowrite, "%f\0",verts[i].x);
            xSCFi->SetAttribute( "x",  chartowrite);
            sprintf(chartowrite, "%f\0",verts[i].y);
            xSCFi->SetAttribute( "y", chartowrite );
            sprintf(chartowrite, "%f\0",1.f);
            xSCFi->SetAttribute( "z",  chartowrite);
            sprintf(chartowrite, "%f\0",0.f);
            xSCFi->SetAttribute( "dir",  chartowrite);
            xStarts->LinkEndChild( xSCFi );
     }
    }

    for(int i = 0; i < vert_count; i++) {
     if(verts[i].type == 2) {
        xSCFi   = new TiXmlElement("add");
        sprintf(chartowrite, "%f\0",verts[i].x);
        xSCFi->SetAttribute( "x",  chartowrite);
        sprintf(chartowrite, "%f\0",verts[i].y);
        xSCFi->SetAttribute( "y", chartowrite );
        sprintf(chartowrite, "%f\0",1.f);
        xSCFi->SetAttribute( "z",  chartowrite);
        xChecks->LinkEndChild( xSCFi );
     }
    }

    for(int i = 0; i < vert_count; i++) {
     if(verts[i].type == 3) {
        xSCFi   = new TiXmlElement("add");
        sprintf(chartowrite, "%f\0", verts[i].x);
        xSCFi->SetAttribute( "x",  chartowrite);
        sprintf(chartowrite, "%f\0", verts[i].y);
        xSCFi->SetAttribute( "y", chartowrite );
        sprintf(chartowrite, "%f\0",1.f);
        xSCFi->SetAttribute( "z",  chartowrite);
        xFinishs->LinkEndChild( xSCFi );
     }
    }

    TiXmlElement *xFuelzones = new TiXmlElement("fuelzones");
    rootnode->LinkEndChild( xFuelzones);
    TiXmlElement *xFuelzonei;
    {
        xFuelzonei = new TiXmlElement("add");
        sprintf(chartowrite, "%f\0",-30.f);
        xFuelzonei->SetAttribute( "x",  chartowrite);
        sprintf(chartowrite, "%f\0",0.f);
        xFuelzonei->SetAttribute( "y",  chartowrite);
        sprintf(chartowrite, "%f\0",1.f);
        xFuelzonei->SetAttribute( "z",  chartowrite);
        xFuelzones->LinkEndChild( xFuelzonei);
    }

    TiXmlElement *xSpeedz = new TiXmlElement("speedzones");
    rootnode->LinkEndChild( xSpeedz);
    {}
    */

    TiXmlElement *xVerts = new TiXmlElement("verts");
    TiXmlElement *xLines = new TiXmlElement("lines");
    TiXmlElement *xPolys = new TiXmlElement("polys");
    rootnode->LinkEndChild( xVerts );
    rootnode->LinkEndChild( xLines );
    rootnode->LinkEndChild( xPolys );

    //load the vector data
    TiXmlElement *xVerti;
    for(int i = 0; i < vert_count;i++) {
            xVerti   = new TiXmlElement("v");
            sprintf(chartowrite, "%f\0",verts[i].x);
            xVerti->SetAttribute( "x",  chartowrite);
            sprintf(chartowrite, "%f\0",verts[i].y);
            xVerti->SetAttribute( "y", chartowrite );
            xVerti->SetAttribute( "type", verts[i].type);
            if(verts[i].type == START_POINT) {  //write spawn direction if the verts is a spawn
                sprintf(chartowrite, "%f\0", verts[i].VertDirection);
                xVerti->SetAttribute( "dir", chartowrite);
            }
            xVerts->LinkEndChild( xVerti );
    }


    TiXmlElement *xLinei;
    for(int i = 0; i < line_count;i++)
    {
        xLinei   = new TiXmlElement("l");
        sprintf(chartowrite, "%i\0",lines[i].a);
        xLinei->SetAttribute( "v1", chartowrite );
        sprintf(chartowrite, "%i\0",lines[i].b);
        xLinei->SetAttribute( "v2", chartowrite );
        xLines->LinkEndChild( xLinei );
    }


    TiXmlElement *xPolyi;
    TiXmlElement *xPolyii;
    for(int i = 0; i < poly_count;i++)
    {
        xPolyi   = new TiXmlElement("p");
        sprintf(chartowrite, "%f\0",polys[i].roof);
        xPolyi->SetAttribute( "roof", chartowrite );
        sprintf(chartowrite, "%f\0",polys[i].floor);
        xPolyi->SetAttribute( "floor", chartowrite );
        sprintf(chartowrite, "%i\0",polys[i].floortex);
        xPolyi->SetAttribute( "ftex", chartowrite );
        sprintf(chartowrite, "%i\0",polys[i].rooftex);
        xPolyi->SetAttribute( "rtex", chartowrite );
        for(int r = 0; r < polys[i].sidecount+1;r++)
        {
            xPolyii   = new TiXmlElement("add");
            sprintf(chartowrite, "%i\0",polys[i].p[r]);
            xPolyii->SetAttribute( "l", chartowrite );
            sprintf(chartowrite, "%i\0",polys[i].tex[r]);
            //cout << "writing file poly#" << i << " with tex " << polys[i].tex[r] << endl;
            xPolyii->SetAttribute( "tex", chartowrite );
            xPolyi->LinkEndChild( xPolyii );
        }

        xPolys->LinkEndChild( xPolyi );
    }

    xmlDoc->SaveFile();

    xmlDoc->Clear();
    delete xmlDoc;
}
