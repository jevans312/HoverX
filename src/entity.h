#pragma once

#include <string>

#include "physpoint.h"
#include "obj.h"
#include "main.h"

class Entity {
public:
    // State
    bool isUsed = false;                // Is there an entity here
    bool isNetworked = false;           // For objects not handled by server
    physpoint pos;                      // Position in the world
    float Pitch = 0.0f;                 // Angular rotation
    float Yaw = 0.0f;
    float Roll = 0.0f;
    int ID = -1;                        // This entity's place in the data array
    int ClientAddress = -1;             // The client who controls this entity

    // Model
    std::string ModelFile;              // The .obj file that is the model
    std::string TextureFile;            // Texture to be used on this model
    objModel Model;                     // Model to display as this entity

    // Physics
    float height = 0.0f;                // Height of the entity
    float jumpingvel = 0.0f;            // Vertical velocity
    bool onground = false;              // Used to determine if we can jump
    bool antionground = false;

    // Networking
    bool ReplicateAdd = false;
    bool ReplicateRemove = false;

    // Game state
    std::string SectorType;
    std::string SectorValue;
    bool FinishedRace = false;
    int lastcheckpoint = -1;
    int currentcheckpoint = -1;
    int CurrentLapNumber = 0;
    int BestLapTime = 0;
    int LastFinishGameTime = 0;
    int LapTime[MAXLAPS] = {0};
    int StartpointAddress = -1;

    // Functions
    Entity() { Clear(); }
    ~Entity() = default;

    bool Add(int entaddress);                   // Add to the world
    void Remove();                              // Remove from the world
    void Clear();
    void ApplyForces(float divstep);            // Used to control an entity's movement
    void EntityCollision(Entity& otherent);     // Test for collisions against other entities
};
