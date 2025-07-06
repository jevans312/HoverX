#pragma once

#include <sstream>
#include <cstring>
#include <string>

// Version and game constants
constexpr const char* VERSION = "7.4.25";
constexpr int MAXLVL = 7;
constexpr int MAXLAPS = 64;
constexpr int MAXPLAYERS = 7;    // max of 8 players
constexpr int MAXCLIENTS = 7;
constexpr int MAXCHANNELS = 2;
constexpr int MAXENTITIES = 63;
constexpr int MAXMSGS = 15;
constexpr int MAXMSGLENTH = 255;
constexpr int TICK_RATE = 10; // 100 ticks per second

// Storage for individual messages
class MSGClass {
public:
    int ClientAddress;  // who this message is from
    int Type;
    char Payload[MAXMSGLENTH];
    unsigned int PayloadSize;    // in bytes

    void Clear() {
        ClientAddress = -1;
        Type = 0;
        std::memset(Payload, '\0', sizeof(Payload));
        PayloadSize = 0;
    }

    MSGClass() { Clear(); }
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
    KeyValue KV[31];    // max of 32 key/value pairs

    TypeKeyValue() : Type("") {}
};

// Helper functions
std::string IntToIpAddress(unsigned int ip);
std::string BoolToStr(bool b);
void skipLine(std::istream& is);
bool skipCommentLine(std::istream& is);
bool FileExists(const char* FileName);
int FindKeyByName(KeyValue* kv, std::string keytofind);
void ParseTKV(std::string worldstr, TypeKeyValue* TKV);

// Template function for integer to string conversion
template <typename T>
std::string IntToStr(T tmp) {
    std::ostringstream out;
    out << tmp;
    return out.str();
}
