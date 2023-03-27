//define all uniersal data and such in this include
#ifndef main_h
#define main_h

//#include "SDL2/SDL.h"
#include "iostream"
#include <sstream>

#define ENGINEVERSION "build_6"     //going by build number untill we have a version 0.5 or something like that
#define MAXLVL 7
#define MAXLAPS 64
#define MAXPLAYERS 7    //max of 8 players
#define MAXCLIENTS 7
#define MAXCHANNELS 2
#define MAXENTITIES 63
#define MAXMSGS 15
#define MAXMSGLENTH 255

using namespace std;


//storage for individual messages
class MSGClass {
public:
    int ClientAddress;  //who this message is from
    int Type;
    char Payload[MAXMSGLENTH];
    unsigned int PayloadSize;    //in bytes

    void Clear() {
        ClientAddress = -1;
        Type = 0;
        for( int i = 0 ; i < MAXMSGLENTH ; ++i) Payload[i] = '\0';
        PayloadSize = 0;
    }

    MSGClass() {
        ClientAddress = -1;
        Type = 0;
        for( int i = 0 ; i < MAXMSGLENTH ; ++i) Payload[i] = '\0';
        PayloadSize = 0;
    };
};

//holds all keyboard status
class KeyState {
public:
    bool accel;
    bool brake;
    bool left;
    bool right;
    bool jump;

    KeyState() {
        accel = false;
        brake = false;
        left = false;
        right = false;
        jump = false;
    };
};

class KeyValue {
public:
    string Key;
    string sValue;
    int iValue;
    float fValue;

    KeyValue() {
        Key = "";
        sValue = "";
        iValue = -1;
        fValue = -1.0;
    };
};

class TypeKeyValue {
public:
    string Type;
    KeyValue KV[31];    //max of 32 key/value pairs

    TypeKeyValue() {
        Type = "";
    };
};

//helper functions

bool FileExists( const char* FileName );    //returns true if file exist
int FindKeyByName(KeyValue *kv, string keytofind);     //return the location of the data we are looking for or -1 on fail
void ParseTKV(string worldstr, TypeKeyValue *TKV);     //turn world string into type key and value
template <typename ROFL>

std::string IntToStr(ROFL tmp)  {
    std::ostringstream out;
    out << tmp;
    return out.str();
}

#endif
