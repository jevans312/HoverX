#pragma once

#include <string>
#include <iostream>
#include "main.h"
#include "level.h"
#include "enet/enet.h"

// Forward declarations
class level;

// Info on each client to be replicated across the network
class ClientList {
public:
    ClientList();
    ~ClientList();

    void Clear();

    std::string Name;                        // How the client will be known
    KeyState Keys;                           // Which keys are down/up
    bool isConnected;                        // Is this slot filled?
    bool hasJoinedRoom;                      // Has fully joined a game room
    int RoomAddress;                         // Client's room; -1 is lobby

    int PeerNumber;                          // Location of this client in the client index
    ENetPeer *Peer;                          // Access to the ENet peer
    std::string PrettyIP;                    // 127.0.0.1 format
    MSGClass MessageBuffer[MAXMSGS];         // Storage for data from this client
    uint32_t LastTimeStamp;                  // Last time client time received, used to keep packets in order
    uint32_t LastKeepAliveTime;              // Time when we last heard a keep alive response from the client
};

// TODO: JAE - 7/4/25 Most of the public variables should be private, accessible only through functions
class ServerClass {
public:
    ServerClass();
    ~ServerClass();

    bool Start(bool AllowRemoteConnections, const std::string& ip = "127.0.0.1");
    void Stop();
    void Update(); // Receive and send data and update world
    void Clear();

    void CheckNetEvents();
    void CheckKeepAlive();
    int AddClientTextMessage(const std::string& newtextmessage, int clientaddress);
    int AddServerTextMessage(const std::string& newtextmessage, int clientaddress);
    void BroadcastTxtMessage(const std::string& localmsg);
    void HandleDataString(const std::string& datastr, int clientaddress);
    void ProcessKeyState(char *packetbuffer, int clientaddress);
    void HandleTextMessage(int messageaddress);
    void EntityAddRemove();
    void BroadcastRoomInfo();
    void BroadcastEntityData(int clientaddress);
    void AddKeepAlive(int clientaddress);
    int CreateNewRoom(int clientaddress, const std::string& mapname);
    void CloseRoom(int clientaddress);
    void JoinRoom(int clientaddress, int roomtojoin);
    bool ExecuteCommand(const std::string &command, const std::string &arg, int clientaddress);
    void HandleNewConnection(ENetEvent localevent);
    void DisconnectClient(int clientaddress);
    void HandleNewPacket(ENetEvent localevent);

    void ReadMessages();
    void ProcessMessages();
    void SendMessages();

    level *lvl;
    bool isRunning;
    bool isAcceptingRemoteClients;
    ENetAddress Address;
    ENetHost *Host;
    ENetPeer *Peer;
    MSGClass MessageBuffer[127];
    ClientList Clients[MAXCLIENTS];
};
