#include <iostream>

#include "entity.h"
#include "server.h"
#include "client.h"

extern ServerClass hxServer;
extern LocalClient LC;

using namespace std;

//Add an entity into the world using predefined data
//returns true on success
bool Entity::Add(int entaddress) {
    if(entaddress != -1) {
        ID = entaddress;
    }
    else {
        cout << "Entity::Add: over writing an entity with and ID already assigned" << '\n';
    }

    string modelfile = "models/" + ModelFile + ".obj";
    string texturefile = "models/" + TextureFile + ".png";

    //check if files exist
    if(FileExists(modelfile.c_str()) == false) {
        cout << "Entity::Add: model's model not found: \"" << modelfile.c_str() << '\n';
        Clear();
        return false;
    }
    if(FileExists(texturefile.c_str()) == false) {
        cout << "Entity::Add: model's texture not found \"" << texturefile.c_str() << "\"" << '\n';
        Clear();
        return false;
    }

    //load model data
    if(LC.isConsoleMode() == false)
        Model.Load(modelfile, texturefile);

    //flag replecation to clients
    if(hxServer.isAcceptingRemoteClients) ReplicateAdd = true;

    isUsed = true;
    return true;
}

void Entity::Remove() {
    //free model memory
    Model.Unload();

    int tempid = ID;    //hold the id after clear, to be reset to -1 by the server once handled

    Clear();
    if(hxServer.isAcceptingRemoteClients) {
        ID = tempid;
        ReplicateRemove = true;
        cout << "entity being set to be removed" << '\n';
    }
}

void Entity::Clear() {
    isUsed = false;
    isNetworked = true;
    Pitch = 0.0f;
    Yaw = 0.0f;
    Roll = 0.0f;
    ClientAddress = -1;
    ID = -1;

    //model
    ModelFile = "";
    TextureFile = "";

    //physics
    onground = true;
    jumpingvel = 0;
    height = 1;

    //networking
    ReplicateAdd = false;
    ReplicateRemove = false;

    //game
    //TODO: This should be Ent[i].ResetGameState() that can be called from clear()
    SectorType = "";
    SectorValue = "";
    FinishedRace = false;
    lastcheckpoint = -1;
    currentcheckpoint = -1;
    CurrentLapNumber = 1;
    BestLapTime = 0;
    LastFinishGameTime = 0;
    for(int i = 0; i < MAXLAPS; i++) LapTime[i] = 0;
    StartpointAddress = -1;
}

void Entity::ApplyForces(float divstep) {
    const float accel       = 0.003f;
    const float drag        = 0.992f;
    const float jumppower   = 0.050f;
    const float steerspeed  = 0.025f;
    const float gravity     = 0.001f;
    const float minzvel     = -0.040f;

    //only apply controls to clients
    if(ClientAddress < 0) return;

    KeyState& keys = hxServer.Clients.at(static_cast<size_t>(ClientAddress)).Keys;

    //save old z position for collision checks
    pos.o.z = pos.c.z;

    //gliding physics
    pos.dophys(drag, 1);

    //apply gravity
    jumpingvel -= gravity/divstep;
    if(jumpingvel < minzvel) {
        jumpingvel = minzvel;
    }

    //apply up velocity if "onground"
    if(keys.jump && onground) {
        jumpingvel = jumppower;
        keys.jump = false;
    }
    pos.c.z += jumpingvel/divstep;

    //brake
    if(keys.brake) {
        vector2d tv = pos.c-pos.o;
        tv.scalef(0.1f);
        tv.divf(divstep);
        pos.c -=  tv;
    }

    //accelerate
    if(keys.accel) {
        vector2d v (sinf(Yaw), cosf(Yaw)); //convert yaw to a vector
        v.scalef(accel);
        v.divf(divstep);
        pos.c+=v;
    }

    //turn
    if(keys.right)  Yaw += steerspeed/divstep;
    if(keys.left)   Yaw -= steerspeed/divstep;
}

void Entity::EntityCollision(Entity &otherent) {
    float zdis = pos.c.z - otherent.pos.c.z; //how high a craft is?
    zdis*=zdis;
    if(zdis < 1) {
        vector2d disvec = pos.c - otherent.pos.c;
        float td = disvec.magsqr();
        float rad = 50/20;
        if(td<rad*rad) {
            td=sqrtf(td);
            disvec.divf(td);
            disvec.scalef(rad/100);
            pos.c+=disvec;
            otherent.pos.c-=disvec;
        }
    }
}
