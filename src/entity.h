#ifndef entity_h
#define entity_h

#include "physpoint.h"
#include "obj.h"
#include "main.h"

class Entity {
public:
    bool isUsed;                //is there an entity here
    bool isNetworked;           //for objects not handled by server
    physpoint pos;              //position in the world
    float Pitch;                //angular rotation
    float Yaw;
    float Roll;
    int ID;                     //this entities place in the data array
    int ClientAddress;          //the client who controls this entity

    //model
    string ModelFile;           //the .obj file that is the model
    string TextureFile;         //links to a texture to be used on this model
    objModel Model;             //model to display as this entity

    //physics
    float height;               //of the entity; radius should also be defined here
    float jumpingvel;           //vertical velocity
    bool onground;              //used to determine if we can jump
    bool antionground;

    //networking
    bool ReplicateAdd;
    bool ReplicateRemove;

    //game; should be moved to a game code defined structure?
    string SectorType;
    string SectorValue;
    bool FinishedRace;
    int lastcheckpoint;
    int currentcheckpoint;
    int CurrentLapNumber;
    int BestLapTime;
    int LastFinishGameTime;
    int LapTime[MAXLAPS];
    int StartpointAddress;

    //functions
    bool Add(int entaddress);                   //add to the world
    void Remove();                              //remove from the world
    void Clear();
    void ApplyForces(float divstep);            //used to control an entities movement
    void EntityCollision(Entity &otherent);     //test for collitons agains other entities

    Entity() {
        Clear();
    };

    ~Entity() {};
};

#endif
