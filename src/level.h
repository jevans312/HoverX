#ifndef level_h
#define level_h

#include <stdlib.h>
#include <stdio.h>
#include "iostream"
#include "main.h"
#include "entity.h"
#include "line2d.h"
#include "poly2d.h"

//helper fuctions
string PrettyPrintTime(int milliseconds);

class StartPoints {
    public:
        short unsigned int  Count;                      //number of start points loaded
        vector2d            Position[MAXPLAYERS];       //max of 8 players
        float               Direction[MAXPLAYERS];      //the way the ship is pointed
        int                 isUsed[MAXPLAYERS];         

        StartPoints() { 
            Clear();
        }

        ~StartPoints() {
            
        }

        void Clear() {
            Count = 0;
            for(int i = 0; i < MAXPLAYERS; i++) {
                isUsed[i] = -1;
            }
        }
};

class GameClass {
public:
    int GameType;           //0 is limbo; 1 is race 2; 3 is sumo
    int RaceLaps;           //how many lap in a race
    int CurrentLap;         //what lap the player is on
    int RaceWinner;         //what ent won the race
    int CheckpointCount;    //number of checkpoints in this level
    uint64_t StartGameOffset;    //when the timer was turned on
    uint64_t StartGameTime;      //when the game is suppost to start
    uint64_t StartGameTimer;     //how long till the race starts
    uint64_t GameTimeOffset;     //subtract this from SDL_GetTicks(); to get the game time


    GameClass() {
        Clear();
    }

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
    string MapName; //just for referance
    bool Loaded = false;
    int gridtexid;  //sky texture
    int vertnum, linenum, polynum;
    int linestage, tlili;   //for adding lines
    int OwnerAddress = -1;   //which client ownes this room
    int EntityCount = 0;
    short unsigned int CheckPointCount;
    vector2d *lvlvert;
    line2d *lvlline;
    poly2d *lvlpoly;
    vector2d *tlvlvert;
    line2d *tlvlline;
    Entity *Ent;
    GameClass Game; //game data
    StartPoints SpawnPoint;
    GLuint  DisplayList;    //address where the levels drawing list will be

    //functions
    int GetGameTime();
    bool StartGame(uint8_t GameType);
    void AddPlayerEntity(int clientaddress);
    void Add3dobject(vector2d pos, string modelfile, string texturefile);
    void FinishedLap(Entity &ent);
    void CheckTimerEvents();
    bool Load(string LevelFilename);
    void Unload();
    void Update();
    void GenerateDisplayList();

    level() {
        Loaded = false;
        MapName = "";
        OwnerAddress = -1;
        EntityCount = 0;
    }
};

//level *lvl; //how the level will be referenced through out the engine
#endif
