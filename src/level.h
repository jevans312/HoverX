#pragma once

#include <string>

#include "poly2d.h"

// Helper function
std::string PrettyPrintTime(int milliseconds);

class StartPoints {
public:
    short unsigned int Count = 0;                      // Number of start points loaded
    vector2d Position[MAXPLAYERS];                     // Max of 8 players
    float Direction[MAXPLAYERS];                       // The way the ship is pointed
    int isUsed[MAXPLAYERS];                            // Usage flags

    StartPoints() { Clear(); }
    void Clear() {
        Count = 0;
        for (int i = 0; i < MAXPLAYERS; i++) {
            isUsed[i] = -1;
        }
    }
};

class GameClass {
public:
    int GameType = -1;           // 0: limbo, 1: race, 2: sumo
    int RaceLaps = 5;            // Number of laps in a race
    int CurrentLap = 1;          // Current lap
    int RaceWinner = -1;         // Entity that won the race
    int CheckpointCount = -1;    // Number of checkpoints in this level
    uint64_t StartGameOffset = -1;
    uint64_t StartGameTime = -1;
    uint64_t StartGameTimer = -1;
    uint64_t GameTimeOffset = 0;

    GameClass() { Clear(); }
    void Clear() {
        GameType = -1;
        RaceLaps = 5;
        CurrentLap = 1;
        RaceWinner = -1;
        StartGameOffset = -1;
        StartGameTime = -1;
        StartGameTimer = -1;
        CheckpointCount = -1;
        GameTimeOffset = 0;
    }
};

class level {
public:
    std::string MapName;             // For reference
    bool Loaded = false;
    int gridtexid = 0;               // Sky texture
    int vertnum = 0, linenum = 0, polynum = 0;
    int linestage = 0, tlili = 0;    // For adding lines
    int OwnerAddress = -1;           // Which client owns this room
    int EntityCount = 0;
    short unsigned int CheckPointCount = 0;
    vector2d* lvlvert = nullptr;
    line2d* lvlline = nullptr;
    poly2d* lvlpoly = nullptr;
    vector2d* tlvlvert = nullptr;
    line2d* tlvlline = nullptr;
    Entity* Ent = nullptr;
    GameClass Game;
    StartPoints SpawnPoint;
    GLuint DisplayList = 0;          // Address of the level's drawing list

    // Functions
    int GetGameTime();
    bool StartGame(uint8_t GameType);
    void AddPlayerEntity(int clientaddress);
    void Add3dobject(vector2d pos, const std::string& modelfile, const std::string& texturefile);
    void FinishedLap(Entity& ent);
    void CheckTimerEvents();
    bool Load(const std::string& LevelFilename);
    void Unload();
    void Update();
    void GenerateDisplayList();

    level() = default;
};
