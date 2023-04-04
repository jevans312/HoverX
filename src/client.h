#ifndef client_h
#define client_h

#include "main.h"
#include "level.h"
#include "ui.h"
#include "enet/enet.h"
#include "iostream"

class LocalClientList {
public:
    string Name;
    int LastUpdate;

    void Clear() {
        Name = "unknown";
        LastUpdate = -1;
    }

    LocalClientList() {
        Clear();
    };
};

class LocalClient {
public:
    level lvl;  //client level data

    //settings
    string Username;

    //states
    bool MSGmode;      //state for typing a message or command
    bool isConnected;    //connected to a server
    bool isConnectedToRemoteServer;
    bool DrawWorld;
    KeyState Keys;

    //various data buffers and address
    ENetPeer *ServerPeer;
    ENetHost *ClientHost;
    unsigned int KeyFlags;
    string MSGDrawBuffer0;  //text messages drawn to the screen; prob should be part of the ui code
    string MSGDrawBuffer1;
    string MSGDrawBuffer2;
    string MSGDrawBuffer3;
    string MSGSendBuffer;
    MSGClass MessageBuffer[MAXMSGS];
    LocalClientList Clients[MAXCLIENTS];
    int EntityAddress;
    uint64_t LastTimestamp;                               //this should prob be reset someplace ;p

    //functions
    void Update();
    void ProcessMessages();

    //private?
    bool NetServerConnect(string IPAddressString);
    void RemoteDisconnect();
    void CheckNetEvents();
    void HandleDataString(string worldstr);
    void ProcessEntityData(char *packetbuffer);
    void HandleTextMessage(int messageaddress);
    bool ExecuteCommand(const string &command, const string &arg);
    void BroadcastTimer();
    void AddTextMessage(string newtextmessage);
    void SendKeyState();
    void BroadcastKeyState();
    void SendClientInfo();
    void SendKeepAlive();
    void SendMessages();
    void HandleNewPacket(ENetEvent localevent);

    LocalClient() {
        Username = "unnamed";
        MSGmode = false;
        isConnected = false;
        isConnectedToRemoteServer = false;
        DrawWorld = false;
        ServerPeer = nullptr;
        ClientHost = nullptr;
        KeyFlags = 0;
        MSGDrawBuffer0 = "";
        MSGDrawBuffer1 = "";
        MSGDrawBuffer2 = "";
        MSGDrawBuffer3 = "";
        MSGSendBuffer = "";
        EntityAddress = -1;
        LastTimestamp = 0;
    };
};

#endif
