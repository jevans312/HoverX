#include <sstream>
#include "level.h"
#include "main.h"
#include "line2d.h"
#include "tinerxml.h"
#include "glex.h"
#include "font.h"
#include "SDL2/SDL.h"
#include "client.h"
#include "server.h"
#include "iostream"

using namespace std;
float globaltime = 0;
extern bool isConsole;
extern ServerClass hxServer;
extern LocalClient LC;

//return 01:23.321 on input of ms time from engine
string PrettyPrintTime(int milliseconds) {
    string returnstr = "";
    int minutes = 0;
    int seconds = 0;
    //int thousandths = 0;

    while(milliseconds > 60000) {
        milliseconds = milliseconds - 60000;
        minutes++;
    }

    while(milliseconds > 1000) {
        milliseconds = milliseconds - 1000;
        seconds++;
    }
    returnstr = IntToStr(minutes) + ":" + IntToStr(seconds) + "." + IntToStr(milliseconds);
    return returnstr;
}


bool level::Load(string LevelFilename)   {
    //add the dir location to the string
    string filewithlocal = "levels/" + LevelFilename;

    //check to see if the file exist or not
    if( ! FileExists(filewithlocal.c_str()) ) return false;

    //clear the memory of any loaded maps
    if(Loaded) Unload();

    //save the name of the map
    MapName = LevelFilename;

    Ent = new Entity[MAXENTITIES];  //allocate all the entities for this level

    const char *temp;
    int texnum = 0;
    int* texlist;
    float WHYSCALE = 1;         //named such, cos i cant remember why im scaling everything up...(and now im not scaling, i may make this load from the level though, or perhaps a user option)
    vector2d tCheckPoints[23];   //Maxium of 24 check points, i think that should be plenty
    vector2d tStartFinish;


    EntityCount = 0;
    CheckPointCount = 0;
    SpawnPoint.Count = 0;
    SpawnPoint.Clear();

    xmlfile hoverlvl;
    TiXmlElement *xLevel = hoverlvl.getxmlfirstelement((char*)filewithlocal.c_str());
    {
        //this has to be defined for poly code
        TiXmlElement *xTextures = hoverlvl.getelement(xLevel, (char*)"textures");
        texnum = hoverlvl.countchildren(xTextures);
        texlist = new int[texnum];

        //only load textures if we are going  LoadGLTexture(xTextures->Attribute("skytex"));to actually use them
        if(!isConsole) {
            //load sky
            temp =  xTextures->Attribute("skytex");
            gridtexid = LoadGLTexture((char*)temp);

            //load texture list
            texnum = hoverlvl.countchildren(xTextures);
            texlist = new int[texnum];  //this has to be defined
            texnum = 0;
            TiXmlElement *xTexturei = xTextures->FirstChildElement();
            while( xTexturei )  {
                //lvltexture[texturenum].floor = atof(xTexturei->Attribute("floor"))*WHYSCALE;
                //lvltexture[texturenum].roof = atof(xTexturei->Attribute("roof"))*WHYSCALE;
                temp = xTexturei->Attribute("texture");
                texlist[texnum] = LoadGLTexture((char*)temp);
                texnum++;
                xTexturei = xTexturei->NextSiblingElement();
            }
        }

        //load verts
        TiXmlElement *xVerts = hoverlvl.getelement(xLevel, (char*)"verts");
        vertnum = hoverlvl.countchildren(xVerts);
        lvlvert = new vector2d[vertnum];
        vertnum = 0;

        TiXmlElement *xVerti = xVerts->FirstChildElement();
        while( xVerti ) {
            if ( atoi(xVerti->Attribute("type")) == 0 ) {   //normal vert
                lvlvert[vertnum].setpos(atof(xVerti->Attribute("x"))*WHYSCALE, atof(xVerti->Attribute("y"))*WHYSCALE);
                vertnum++;
            }
            else if ( atoi(xVerti->Attribute("type")) == 1 )    {   //starts
                if ( SpawnPoint.Count + 1 < MAXPLAYERS ) {
                    SpawnPoint.Position[SpawnPoint.Count].x = atof(xVerti->Attribute("x"));
                    SpawnPoint.Position[SpawnPoint.Count].y = atof(xVerti->Attribute("y"));
                    SpawnPoint.Direction[SpawnPoint.Count] = atof(xVerti->Attribute("dir"));
                    //cout << "dir " << SpawnPoint.Direction[SpawnPoint.Count] << endl;
                    SpawnPoint.Count++;
                    vertnum++;
                }
                else cout << "level::Load: too many start points, stoping at 8" << endl;
            }
            else if ( atoi(xVerti->Attribute("type")) == 2 ) {  //checkpoints
                if (CheckPointCount + 1 < 23)  {
                    tCheckPoints[CheckPointCount].x = atof(xVerti->Attribute("x"));
                    tCheckPoints[CheckPointCount].y = atof(xVerti->Attribute("y"));
                    CheckPointCount++;
                    vertnum++;
                }
            }
            else if ( atoi(xVerti->Attribute("type")) == 3 )    {   //StartFinish
                tStartFinish.x = atof(xVerti->Attribute("x"));
                tStartFinish.y = atof(xVerti->Attribute("y"));
                vertnum++;
            }
            else if ( atoi(xVerti->Attribute("type")) == 4 )    {   //3d obj
                vector2d pos;
                pos.x = atof(xVerti->Attribute("x"));
                pos.y = atof(xVerti->Attribute("y"));

                Add3dobject(pos, "3dobject", "3dobject");
                vertnum++;

                cout << "adding 3d object " << pos.x  << " " << pos.y << endl;
            }
            else {
                cout << "vert without type not added" << endl;
            }
            xVerti = xVerti->NextSiblingElement();
        }

        //load line data
        TiXmlElement *xLines = hoverlvl.getelement(xLevel, (char*)"lines");
        linenum = hoverlvl.countchildren(xLines);
        lvlline = new line2d[linenum];
        linenum = 0;

        TiXmlElement *xLinei = xLines->FirstChildElement();
        while( xLinei ) {
            //lvlline[linenum].p1 = &lvlvert[atoi(xLinei->Attribute("v1"))];
            //lvlline[linenum].p2 = &lvlvert[atoi(xLinei->Attribute("v2"))];
            lvlline[linenum].setup(lvlvert[atoi(xLinei->Attribute("v1"))],lvlvert[atoi(xLinei->Attribute("v2"))]);
            linenum++;
            xLinei = xLinei->NextSiblingElement();
        }

        //load polys
        TiXmlElement *xPolys = hoverlvl.getelement(xLevel, (char*)"polys");
        polynum = hoverlvl.countchildren(xPolys);
        lvlpoly = new poly2d[polynum];
        polynum = 0;
        TiXmlElement *xPolyi = xPolys->FirstChildElement();
        int checktex = 0;
        while( xPolyi ) {
            lvlpoly[polynum].lnum = hoverlvl.countchildren(xPolyi);
            lvlpoly[polynum].l = new lc[lvlpoly[polynum].lnum];
            lvlpoly[polynum].floor = atof(xPolyi->Attribute("floor"))*WHYSCALE;
            lvlpoly[polynum].roof = atof(xPolyi->Attribute("roof"))*WHYSCALE;
            checktex = atoi(xPolyi->Attribute("ftex"));
            if(checktex >= 0)
                lvlpoly[polynum].floortex = texlist[checktex];
            else
                lvlpoly[polynum].floortex = -1;

            checktex = atoi(xPolyi->Attribute("rtex"));
            if(checktex >= 0)
                lvlpoly[polynum].rooftex = texlist[checktex];
            else
                lvlpoly[polynum].rooftex = -1;

            TiXmlElement *xPLinei = xPolyi->FirstChildElement();
            for(int i = 0;i<lvlpoly[polynum].lnum;i++)
            {
                lvlpoly[polynum].l[i].l = &lvlline[atoi(xPLinei->Attribute("l"))];
                checktex = atoi(xPLinei->Attribute("tex"));
                if(checktex >= 0)
                    lvlpoly[polynum].l[i].ltexid = texlist[checktex];
                else
                    lvlpoly[polynum].l[i].ltexid = -1;
                xPLinei = xPLinei->NextSiblingElement();
            }
            polynum++;
            xPolyi = xPolyi->NextSiblingElement();
        }

        //assign poly status check/finish etc
        for (int pid = 0; pid < polynum;pid++)   {
            if(lvlpoly[pid].pnpoly(tStartFinish.x, tStartFinish.y))
                lvlpoly[pid].isfinish = 1;

            for(int i = 0; i < CheckPointCount; i++)   {
                if(lvlpoly[pid].pnpoly(tCheckPoints[i].x, tCheckPoints[i].y))   {
                    //lvlpoly[pid].ischeckpoint = 1;
                    //lvlpoly[pid].checkpointid = i;
                    stringstream convert (stringstream::in | stringstream::out);
                    string sectorval = "";

                    convert << i;
                    convert >> sectorval;

                    lvlpoly[pid].sectortype = "checkpoint";
                    lvlpoly[pid].sectorvalue = sectorval;
                    //cout << "level::Load assign checkpoint sectorvalue: " << lvlpoly[pid].sectorvalue << endl;
                }
            }
        }

        //save checkpoint count
        if(CheckPointCount != 0)    Game.CheckpointCount = CheckPointCount;
        else Game.CheckpointCount = -1;

        cout << "level::load: Game.CheckpointCount: " << Game.CheckpointCount << endl;

        delete[] texlist;
    }
    hoverlvl.endxml();

    //Set the height of the floor and ceiling
    for (int pid = 0; pid <polynum;pid++)
    {
        //wallrangeT initialrange;
        //initialrange.low = lvlpoly[pid].floor;
        //initialrange.high = lvlpoly[pid].roof;
        for (int lid = 0; lid <lvlpoly[pid].lnum;lid++)
        {
            lvlpoly[pid].l[lid].low = lvlpoly[pid].floor;
            lvlpoly[pid].l[lid].high = lvlpoly[pid].roof;
            //    lvlpoly[pid].l[lid].range.push_back(initialrange);
        }
    }


    for (int pid = 0; pid <polynum;pid++)
    {
        for (int lid = 0; lid <lvlpoly[pid].lnum;lid++)
        {
            lvlpoly[pid].l[lid].alone = 1;
        }
    }

    for (int pid = 0; pid <polynum;pid++)
    {
        lvlpoly[pid].getcentre();
        lvlpoly[pid].sortflipped();
    }

    for (int pid = 0; pid <polynum;pid++)
    {
        for (int dip = 0; dip <polynum;dip++)
        {
            if(dip!=pid)
                for (int lid = 0; lid <lvlpoly[pid].lnum;lid++)
                {
                    for (int dil = 0; dil <lvlpoly[dip].lnum;dil++)
                    {
                        lvlpoly[pid].l[lid].checkagainst(lvlpoly[dip].l[dil]);
                    }
                }
        }
    }

    for (int pid = 0; pid <polynum;pid++)
    {
        wallrangeT initialrange;
        initialrange.low = lvlpoly[pid].floor;
        initialrange.high = lvlpoly[pid].roof;
        for (int lid = 0; lid <lvlpoly[pid].lnum;lid++)
        {
            //lvlpoly[pid].l[lid].low = lvlpoly[pid].floor;
            //lvlpoly[pid].l[lid].high = lvlpoly[pid].roof;
            if(lvlpoly[pid].l[lid].alone)
                lvlpoly[pid].l[lid].range.push_back(initialrange);
        }
    }

    //who can control the room besides the server; set to -1 to say no one
    OwnerAddress = -1;

    //store display list for opengl to draw
    GenerateDisplayList();
    Loaded = true;
    return true;
}

void level::GenerateDisplayList() {
    //Start a new list
    DisplayList = glGenLists(1);
	glNewList(DisplayList, GL_COMPILE);

    //draw the world polygon by polygon
    for (int i = 0; i <polynum;i++) {
        lvlpoly[i].draw();
    }

    glEndList();
}

void level::Update() {
    CheckTimerEvents();

    //glide
    //for(int i = 0; i < MAXENTITIES; i++)
    //    if(Ent[i].isUsed) Ent[i].pos.dophys(0.992, 1);

    //entity testing
    for (int i = 0; i < MAXENTITIES; i++) {
        if(Ent[i].isUsed) {
            Ent[i].ApplyForces(1);  //gravity and input
            Ent[i].onground = 0; Ent[i].antionground = 0;   //reset ground info

            //look for collition with each player
            for (int r = 0; r < MAXENTITIES; r++) {
                if(r != i && Ent[r].isUsed) Ent[i].EntityCollision(Ent[r]);  //dont colide with your self
            }

            //floor/ceiling collision poly by poly
            for (int pid = 0; pid < polynum; pid++)   {
                lvlpoly[pid].docolfloor(Ent[i]);
            }

            //wall collision poly by poly
            for (int pid = 0; pid < polynum; pid++)   {
                lvlpoly[pid].docolwall(Ent[i]);
            }
        }
    }

    //update game state
    for(int i = 0; i < MAXENTITIES; i++) {  //look for any ents that are flagged to be in a checkpoint
        if(Ent[i].isUsed && Ent[i].SectorType == "checkpoint" && Game.GameType == 1) {
            stringstream convert (stringstream::in | stringstream::out);
            int checkpointnumber = -1;

            //change string value to an integer
            convert << Ent[i].SectorValue;
            convert >> checkpointnumber;

            if( Ent[i].currentcheckpoint + 1 == checkpointnumber) {     //hit a hit point 1 more than the previous
                Ent[i].currentcheckpoint = checkpointnumber;

                //-1 because 0 1 2 3 is 4 checkpoints
                if(Ent[i].currentcheckpoint == Game.CheckpointCount - 1)    FinishedLap(Ent[i]);
            }
        }
    }
}

void level::Unload() {
    if(!Loaded) return;

    //unload all models
    for(int i = 0; i < MAXENTITIES; i++) {
        if(Ent[i].isUsed) {    //make sure theres an entity before go checking it too deeply
            if( Ent[i].Model.Loaded ) {
                Ent[i].Model.Unload();
            }

            Ent[i].Clear();
        }
    }

    //free memory
    delete[] lvlvert;
    delete[] lvlline;
    delete[] lvlpoly;
    delete[] Ent;

    Loaded = false;
    MapName = "";
    OwnerAddress = -1;
    EntityCount = 0;
}

//*********
//game code
//*********
void level::AddPlayerEntity(int clientaddress) {
    int entaddress = -1;

    if(clientaddress == -1) return;

    //find an unused entity
    for(int i = 0; i < MAXENTITIES; i++) {
        if(!Ent[i].isUsed) {
            entaddress = i;
            i = MAXENTITIES + 1;
        }
    }

    if (entaddress == -1) {
       cout << "AddPlayerEntity: no space for a new entity" << endl;
       return;
    }

    //find an empty spawnpont
    for( int i = 0; i < SpawnPoint.Count; i++) {
        if(SpawnPoint.isUsed[i] == -1) {
            SpawnPoint.isUsed[i] = entaddress;
            Ent[entaddress].StartpointAddress = i;
            i = 100;    //end loop
        }
    }

    if (Ent[entaddress].StartpointAddress == -1) {
        cout << "couldnt find a spawn point for player " << entaddress << ", player not added" << endl;
        return;
    }

    //starting point
    Ent[entaddress].pos.teleport(SpawnPoint.Position[Ent[entaddress].StartpointAddress].x, SpawnPoint.Position[Ent[entaddress].StartpointAddress].y);
    Ent[entaddress].pos.c.z = Ent[entaddress].pos.o.z = 0.1;
    Ent[entaddress].Yaw = SpawnPoint.Direction[Ent[entaddress].StartpointAddress];

    Ent[entaddress].pos.rad = 1.4;  //collision radius
    Ent[entaddress].ModelFile = "bicraft";
    Ent[entaddress].TextureFile = "bicraft-blue";
    Ent[entaddress].ClientAddress = clientaddress;

    if(! Ent[entaddress].Add(entaddress) ) {
        cout << "AddPlayerEntity: add player entity failed" << endl;
        Ent[entaddress].Clear();
    }
}

void level::Add3dobject(vector2d pos , string modelfile, string texturefile) {
    int entaddress = -1;

    //find an unused entity
    for(int i = 0; i < MAXENTITIES; i++) {
        if(!Ent[i].isUsed) {
            entaddress = i;
            i = MAXENTITIES + 1;
        }
    }

    if (entaddress == -1) {
       cout << "Add3dobject: no space for a new entity" << endl;
       return;
    }

    //properties
    Ent[entaddress].pos.teleport(pos.x, pos.y);
    Ent[entaddress].pos.c.z = Ent[entaddress].pos.o.z = 0.1;

    Ent[entaddress].pos.rad = 0.5;  //collision radius
    Ent[entaddress].ModelFile = modelfile;
    Ent[entaddress].TextureFile = texturefile;

    if(! Ent[entaddress].Add(entaddress) ) {
        cout << "Add3dobject: add 3dobject failed" << endl;
        Ent[entaddress].Clear();
    }
}

void level::FinishedLap(Entity &ent) {
    int gametime = GetGameTime();
    ent.currentcheckpoint = 0;   //reset the current checkpoint
    int currentlaptime = gametime - ent.LastFinishGameTime;
    ent.LastFinishGameTime = gametime;
    int currentlapnumber = ent.CurrentLapNumber;
    ent.LapTime[ent.CurrentLapNumber] = currentlaptime;

    string finishmsg = "";

    if(Game.CurrentLap < Game.RaceLaps) {   //finshed lap
        Game.CurrentLap++;
        if(ent.CurrentLapNumber < MAXLAPS) ent.CurrentLapNumber++;
        if(ent.CurrentLapNumber == 1) ent.BestLapTime = currentlaptime;   //first lap is always the fastest!

        //setup message for finishing a lap
        string currentlaptimestring = PrettyPrintTime(currentlaptime);    //convert to string
        string currentlapnumberstring = IntToStr(currentlapnumber);
        finishmsg = LC.Clients[ent.ClientAddress].Name + " lap #" + currentlapnumberstring + " time: " + currentlaptimestring;
    }
    else {  //finished race
        string gametimestring = PrettyPrintTime(GetGameTime());

        if( ! ent.FinishedRace) {
            if(Game.RaceWinner == -1) {
                Game.RaceWinner = ent.ID;
                finishmsg = LC.Clients[ent.ClientAddress].Name + " won the race in " + gametimestring;
            }
            else {
                finishmsg = LC.Clients[ent.ClientAddress].Name + " finished the race in " + gametimestring;
            }

            ent.FinishedRace = true;
        }
    }

    //set new best laptime if its is the best
    int lowestlaptime = 2147483647;
    for(int j = 0; j < MAXLAPS; j++) {
        if(ent.LapTime[j] < lowestlaptime && ent.LapTime[j] != -1) lowestlaptime = ent.LapTime[j];
        //cout << "lap #" << i << " time: " << Ent[i].LapTime[j] << " lowest time " << lowestlaptime << endl;
    }

    ent.BestLapTime = lowestlaptime;

    //tell everyone whats up
    if(finishmsg != "") hxServer.ExecuteCommand("txtmsg", finishmsg, 0);
}

void level::CheckTimerEvents() {
    string msg = "";

    //check for game start timer updates
    if(SDL_GetTicks64() > Game.StartGameTime) {
        //reset timers
        Game.StartGameOffset = -1;
        Game.StartGameTime = -1;
        Game.StartGameTimer = -1;
        StartGame(1);
    }
    else if(SDL_GetTicks64() > Game.StartGameOffset + 1000) {
        Game.StartGameOffset = Game.StartGameOffset + 1000;
        msg = "game will start in " + IntToStr(Game.StartGameTimer);
        hxServer.ExecuteCommand("txtmsg", msg, 0);
        Game.StartGameTimer--;
    }
}

//0 is limbo; 1 is race 2; 3 is sumo
bool level::StartGame(uint8_t gametype) {
    if(gametype > 3) {
        cout << "GameClass::StartGame failed, game type " << gametype << " is over 3!" << endl;
        return false;
    }

    //reset all client ents
    for(int i = 0; i < MAXENTITIES; i++) {
        if( Ent[i].ClientAddress != -1 ) {
            Ent[i].pos.teleport(SpawnPoint.Position[Ent[i].StartpointAddress].x, SpawnPoint.Position[Ent[i].StartpointAddress].y);
            Ent[i].pos.c.z = Ent[i].pos.o.z = 0.1;
            Ent[i].Yaw = SpawnPoint.Direction[Ent[i].StartpointAddress];
            Ent[i].SectorType = "";
            Ent[i].SectorValue = "";
            Ent[i].FinishedRace = false;
            Ent[i].lastcheckpoint = -1;
            Ent[i].currentcheckpoint = -1;
            Ent[i].CurrentLapNumber = 1;
            Ent[i].BestLapTime = 0;
            Ent[i].LastFinishGameTime = 0;
            for(int j = 0; j < MAXLAPS; j++) Ent[i].LapTime[j] = -1;
        }
    }

    Game.GameType = gametype;
    Game.CurrentLap = 1;
    Game.RaceWinner = -1;
    Game.GameTimeOffset = SDL_GetTicks();
    return true;
}

int level::GetGameTime()  {
    int returnval = 0;

    returnval = SDL_GetTicks64() - Game.GameTimeOffset;
    //cout << "GameClass::GetGameTime() returning a GameTime of: " << returnval << endl;
    return returnval;
}
