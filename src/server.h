#ifndef server_h
#define server_h
#include "main.h"
#include "level.h"
#include "enet/enet.h"
#include "string"
#include "iostream"

using namespace std;

//Info on each one of our clients to be replecated across the network
class ClientList {
    public:
        string Name;                        //How the client will be known
        KeyState Keys;                      //which keys are down/up
        bool isConnected;                   //is this slot filled?
        bool hasJoinedRoom;                 //has fully joined a game room
        int RoomAddress;                    //clients room -1 is lobby

        int PeerNumber;                     //location of where this client is in the client index
        ENetPeer *Peer;                     //access to the ENet peer
        string PrettyIP;                    //127.0.0.1 format
        MSGClass MessageBuffer[MAXMSGS];    //storage for data from this client
        uint32_t LastTimeStamp;             //last time client time received, used to keep packets in order
        uint32_t LastKeepAliveTime;         //time when we last heard a keep alive responce from the client

    void Clear() {  //reset all the data
        Peer = 0;
        PrettyIP.clear();
        Name = "unamed";
        PeerNumber = -1;
        LastTimeStamp = 0;
        LastKeepAliveTime = 0;
        isConnected = false;
        hasJoinedRoom = false;
        RoomAddress = -1;
    }

    ClientList() {
        Clear();
    };

    ~ClientList() {
        Clear();
    };
};

class ServerClass {
public:
    level *lvl;
    bool isRunning;
    bool isAcceptingRemoteClients;  //private?
    ENetAddress Address;
    ENetHost *Host;
    ENetPeer *Peer;
    MSGClass MessageBuffer[127];    //store all commands to be executed; should have a defined value?
    ClientList Clients[MAXCLIENTS];

    //functions
    bool Start(bool AllowRemoteConnections, const string ip = "127.0.0.1");
    void Stop();
    void Update(); //reseive and send data and update world
    void Clear();

    //private?
    void CheckNetEvents();
    void CheckKeepAlive();
    int AddClientTextMessage(string newtextmessage, int clientaddress);   //for adding messages to clients
    int AddServerTextMessage(string newtextmessage, int clientaddress);  //for adding messages to the server
    void BroadcastTxtMessage(string localmsg);
    void HandleDataString(string datastr, int clientaddress);
    void ProcessKeyState(char *packetbuffer, int clientaddress);
    void HandleTextMessage(int messageaddress);
    void EntityAddRemove();
    void BroadcastRoomInfo();
    void BroadcastEntityData(int clientaddress);
    void AddKeepAlive(int clientaddress);
    int CreateNewRoom(int clientaddress, string mapname);      //returns the room number that was just created
    void CloseRoom(int clientaddress);
    void JoinRoom(int clientaddress, int roomtojoin);
    bool ExecuteCommand(const string &command, const string &arg, int clientaddress);
    void HandleNewConnection(ENetEvent localevent);
    void DisconnectClient(int clientaddress);
    void HandleNewPacket(ENetEvent localevent);

    void ReadMessages();
    void ProcessMessages();
    void SendMessages();

    ServerClass() {
        Clear();
    };

    ~ServerClass() {
        Clear();
    };
};

#endif
