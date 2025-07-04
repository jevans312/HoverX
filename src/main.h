//define all uniersal data and such in this include
#ifndef MAIN_H
#define MAIN_H

//#include "SDL2/SDL.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <string>

// Version and game constants
#define VERSION "7.4.25"
#define MAXLVL 7
#define MAXLAPS 64
#define MAXPLAYERS 7    //max of 8 players
#define MAXCLIENTS 7
#define MAXCHANNELS 2
#define MAXENTITIES 63
#define MAXMSGS 15
#define MAXMSGLENTH 255

using namespace std;


// Storage for individual messages
class MSGClass {
public:
    int ClientAddress;  //who this message is from
    int Type;
    char Payload[MAXMSGLENTH];
    unsigned int PayloadSize;    //in bytes

    void Clear() {
        ClientAddress = -1;
        Type = 0;
        std::memset(Payload, '\0', sizeof(Payload));
        PayloadSize = 0;
    }

    MSGClass() {
        Clear();
    }
};

// Holds all keyboard status
class KeyState {
public:
    bool accel;
    bool brake;
    bool left;
    bool right;
    bool jump;

    KeyState() : accel(false), brake(false), left(false), right(false), jump(false) {}
};

class KeyValue {
public:
    std::string Key;
    std::string sValue;
    int iValue;
    float fValue;

    KeyValue() : Key(""), sValue(""), iValue(-1), fValue(-1.0f) {}
};

class TypeKeyValue {
public:
    std::string Type;
    KeyValue KV[31];    //max of 32 key/value pairs

    TypeKeyValue() : Type("") {}
};

// Helper functions
std::string IntToIpAddress(unsigned int ip);
std::string BoolToStr(const bool b);
void skipLine(std::istream& is);
bool skipCommentLine(std::istream& is);
bool FileExists(const char* FileName);    //returns true if file exist
int FindKeyByName(KeyValue *kv, std::string keytofind);     //return the location of the data we are looking for or -1 on fail
void ParseTKV(std::string worldstr, TypeKeyValue *TKV);     //turn world string into type key and value
template <typename ROFL>

std::string IntToStr(ROFL tmp)  {
    std::ostringstream out;
    out << tmp;
    return out.str();
}
#endif
