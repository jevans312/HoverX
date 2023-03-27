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
    ENetPeer *Peer;                                         //access to the ENet peer
    string PrettyIP;                                        //127.0.0.1 format
    string Name;                                            //How the client will be known
    MSGClass MessageBuffer[MAXMSGS];                        //storage for data from this client
    int PeerNumber;                                         //location of where this client is in the client index
    uint32_t LastTimeStamp;                                      //last time client time received, used to keep packets in order
    int LastKeepAliveTime;                                  //time when we last heard a keep alive responce from the client
    bool isConnected;                                       //is this slot filled?
    bool hasJoinedRoom;                                     //has fully joined a game room
    KeyState Keys;                                          //which keys are down/up
    int RoomAddress;                                        //clients room -1 is lobby

    void Clear() {  //reset all the data
        Peer = NULL;
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
        Peer = NULL;
        PrettyIP.clear(); //clear string
        Name = "unamed";
        PeerNumber = -1;
        LastTimeStamp = 0;
        LastKeepAliveTime = 0;
        isConnected = false;
        hasJoinedRoom = false;
        RoomAddress = -1;
    };
    ~ClientList() {
        //destroy
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
    bool Start(bool AllowRemoteConnections);
    void Stop();
    void Update(); //reseive and send data and update world

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
        isRunning = false;
        isAcceptingRemoteClients = false;
        //Address = NULL;
        Host = NULL;
        Peer = NULL;
    };

    ~ServerClass() {};
};

#endif
