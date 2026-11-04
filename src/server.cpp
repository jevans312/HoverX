#include <print>

#include "main.h"
#include "server.h"
#include "client.h"

extern LocalClient LC;

ServerClass::ServerClass() {
    lvl = nullptr;
    isRunning = false;
    isAcceptingRemoteClients = false;
    Host = nullptr;
    Peer = nullptr;
    Address.host = ENET_HOST_ANY;
    Address.port = 41414;
    for (size_t i = 0; i < MessageBuffer.size(); ++i) {
        MessageBuffer[i].Clear();
    }
    for (size_t i = 0; i < Clients.size(); ++i) {
        Clients[i].Clear();
    }
}

ServerClass::~ServerClass() {
    /*
    if (lvl) {
        delete[] lvl;
        lvl = nullptr;
    }
    */
}

bool ServerClass::Start(bool AllowRemoteConnections, const std::string& ip) {
    if(isRunning) {
        Stop();
    }

    isAcceptingRemoteClients = AllowRemoteConnections;

    enet_address_set_host(&Address, ip.c_str()); // Address.host = ENET_HOST_ANY;
    Address.port = 41414;

    std::print("\n");
    std::println("====== Server ======\nAddress: {}:{}\nConnections Accepted: {}",
        ip, Address.port, isAcceptingRemoteClients ? "Remote" : "Local");

    Host = enet_host_create (& Address      /* the address to bind the server host to */,
                            MAXCLIENTS      /* allow up to MAXCLIENTS clients and/or outgoing connections */,
                            MAXCHANNELS     /* allow up to 2 channels to be used, 0 and 1 */,
                            0               /* assume any amount of incoming bandwidth */,
                            0               /* assume any amount of outgoing bandwidth */);

    //cant return false other wise the server wont start; this could cause problems
    if (Host == NULL) {
        std::println("Host is NULL!\nRunning: False");
        return false;
    } else {
        //create lvl memory
        lvl = new level[MAXLVL];

        //this bit acts as a replacement for an actual local client log in
        Clients[0].isConnected = true;
        Clients[0].Name = LC.Username;

        isRunning = true;
        std::println("Running: True");
        return true;
    }
}

void ServerClass::Stop() {
    if(!isRunning) return;

    //destroy levels
    for(int i = 0; i < MAXLVL; i++) {
        if( lvl[i].Loaded ) lvl[i].Unload();
    }

    delete[] lvl;

    //reset the server status
    isRunning = false;
    isAcceptingRemoteClients = false;

    std::println("Server: Stopped");
}

void ServerClass::Clear() {
    isRunning = false;
    isAcceptingRemoteClients = false;
    Address.host = ENET_HOST_ANY;
    Address.port = 41414;
    Host = ENET_HOST_ANY;
    Peer = 0;
}

//messages that will go out to the clients
int ServerClass::AddClientTextMessage(const std::string& newtextmessage, int clientaddress) {
    if(newtextmessage.empty() || newtextmessage.size() > static_cast<std::string::size_type>(MAXMSGLENTH)) {
        std::println("server: cant add new message: \"{}\"; invalid size", newtextmessage);
        return -1;
    }

    //find a empty message slot
    int emptymsgaddress = -1;
    for(int i = 0; i <= MAXMSGS; i++) {
        if(Clients[clientaddress].MessageBuffer[i].Type == 0) {     //TODO: crashbug: if the server is in a room that is closed the program will crash here
            emptymsgaddress = i;
            i = MAXMSGS + 1;
        }
    }

    //no empty spot found
    if(emptymsgaddress == -1) {
        std::println("server: message buffer full");
        return -1;
    }

    Clients[clientaddress].MessageBuffer[emptymsgaddress].Type = 1;
    Clients[clientaddress].MessageBuffer[emptymsgaddress].PayloadSize = static_cast<unsigned int>(newtextmessage.size());
    memcpy(&Clients[clientaddress].MessageBuffer[emptymsgaddress].Payload + 0, newtextmessage.c_str(), Clients[clientaddress].MessageBuffer[emptymsgaddress].PayloadSize);

    //std::println("server position: {} added type: {} that is {} bytes with a payload of \"{}\"",
    //    emptymsgaddress, Clients[clientaddress].MessageBuffer[emptymsgaddress].Type,
    //    Clients[clientaddress].MessageBuffer[emptymsgaddress].PayloadSize,
    //    Clients[clientaddress].MessageBuffer[emptymsgaddress].Payload);
    return emptymsgaddress;
}

//messages that go to the server
int ServerClass::AddServerTextMessage(const std::string& newtextmessage, int clientaddress) {
    if(newtextmessage.empty() || newtextmessage.size() > static_cast<std::string::size_type>(MAXMSGLENTH)) {
        std::println("server: cant add new message: \"{}\"; invalid size", newtextmessage);
        return -1;
    }

    //find a empty message slot
    int emptymsgaddress = -1;
    for(int i = 0; i < MAXMSGS; i++) {
        if(MessageBuffer[i].Type == 0) {
            emptymsgaddress = i;
            i = MAXMSGS + 1;
        }
    }

    //no empty spot found
    if(emptymsgaddress == -1) {
        std::println("server: message buffer full, intended message: {}", newtextmessage);
        return -1;
    }

    MessageBuffer[emptymsgaddress].ClientAddress = clientaddress;
    MessageBuffer[emptymsgaddress].Type = 1;
    MessageBuffer[emptymsgaddress].PayloadSize = static_cast<unsigned int>(newtextmessage.size());
    memcpy(&MessageBuffer[emptymsgaddress].Payload + 0, newtextmessage.c_str(), MessageBuffer[emptymsgaddress].PayloadSize);

    //std::println("server position: {} added type: {} that is {} bytes with a payload of \"{}\"",
    //    emptymsgaddress, Clients[clientaddress].MessageBuffer[emptymsgaddress].Type,
    //    Clients[clientaddress].MessageBuffer[emptymsgaddress].PayloadSize,
    //    Clients[clientaddress].MessageBuffer[emptymsgaddress].Payload);
    return emptymsgaddress;
}

void ServerClass::BroadcastTxtMessage(const std::string& localmsg) {
    //find all connected and add text message to them
    for(int i = 0; i < MAXCLIENTS; i++) {
        if(Clients[i].isConnected) {  //no connection here
            //std::println("broadcasting txt msg: {} to client: {}", localmsg, i);
            AddClientTextMessage(localmsg, i);
        }
    }
}

void ServerClass::BroadcastRoomInfo() {
    std::string datastr = "";
    char chartowrite[127];
    memset(chartowrite, '\0', sizeof(chartowrite));

    datastr = "/datastr /room";

    for(int i = 0; i < MAXLVL; i++) {
        if( lvl[i].Loaded ) {
            sprintf(chartowrite, " %i=", i);
            datastr += chartowrite;
            datastr += lvl[i].MapName;
        }
    }

    datastr += " /clients";

    for(int i = 1; i < MAXCLIENTS; i++) {   //skip client 0; thats the local servers name
        if(Clients[i].Name != "unamed") {
            sprintf(chartowrite, " %i=", i);
            datastr += chartowrite;
            datastr += Clients[i].Name;
        }
    }
    datastr += " "; //this is to avoid a bug where a lack of a space after a '/object' results in a crash from ParseTKV

    if(datastr != "/datastr /room /clients") {
        BroadcastTxtMessage(datastr);
        //std::println("ServerClass::BroadcastRoomInfo: sending->{}", datastr);
    }
}


void ServerClass::EntityAddRemove() {
    std::string datastr = "";
    char chartowrite[127];
    memset(chartowrite, '\0', sizeof(chartowrite));

    for(int j = 0; j < MAXLVL; j++) {
        datastr = "/datastr ";

        //entity details
        for(int i = 0; i < MAXENTITIES; i++) {
            if( lvl[j].Loaded) {
                if(lvl[j].Ent[i].ReplicateAdd) {
                    //start new entity description
                    datastr += "/e";

                    //address
                    sprintf(chartowrite, " id=%i", lvl[j].Ent[i].ID);
                    datastr += chartowrite;

                    //model
                    datastr += " md=" + lvl[j].Ent[i].ModelFile;
                    datastr += " tx=" + lvl[j].Ent[i].TextureFile;

                    lvl[j].Ent[i].ReplicateAdd = false;

                }

                if(lvl[j].Ent[i].ReplicateRemove) { //tell the client to remove this ent
                    //std::println("how can this not work?");
                    sprintf(chartowrite, " /e id=%i rm=1", lvl[j].Ent[i].ID);
                    datastr += chartowrite;
                    lvl[j].Ent[i].ID = -1;
                    lvl[j].Ent[i].ReplicateRemove = false;
                }
            }
        }

        if(datastr != "" && datastr != "/datastr ") {
            for(int i = 0; i < MAXCLIENTS; i++) {
                //send only to those clients that have confirmed there joinning the room
                //std::println("server sending datastr: {}", datastr);
                if( Clients[i].hasJoinedRoom && Clients[i].RoomAddress == j ) AddClientTextMessage(datastr, i);
            }
        }
    }
}

void ServerClass::BroadcastEntityData(int clientaddress) {
    char packetbuffer[1080];    //enough for 64 entities and packet over head
    uint8_t binarydatatype = 3; //type 3 is entity data so the client knows what to do with it
    uint16_t datasize = 0;      //
    uint64_t timestamp = 0;     //SDL ticks used as a way of ordering packets
    uint16_t entcount = 0;      //how many entities data are in this packet
    uint16_t entaddress = 0;    //what entity this data is for
    uint16_t bufpos = 0;

    memset(packetbuffer, '\0', sizeof(packetbuffer));

    if(!Clients[clientaddress].hasJoinedRoom) return;

    //figure out which room the client is in
    int clientroom = Clients[clientaddress].RoomAddress;
    if(clientroom == -1) return;    //no room no need to broadcast entity info

    timestamp = SDL_GetTicks64();

    //count entities to be sent; if zero no point in sending a packet
    for(int i = 0; i < MAXENTITIES; i++) {
        if(lvl[clientroom].Ent[i].isUsed && lvl[clientroom].Ent[i].isNetworked) entcount++;
    }
    if(entcount == 0) return;


    //start assembling the packet
    memcpy(packetbuffer + bufpos, &binarydatatype, sizeof(binarydatatype));
    bufpos += sizeof(binarydatatype);
    memcpy(packetbuffer + bufpos, &datasize, sizeof(datasize)); //this will always be zero here
    bufpos += sizeof(datasize);
    memcpy(packetbuffer + bufpos, &timestamp, sizeof(timestamp));
    bufpos += sizeof(timestamp);
    memcpy(packetbuffer + bufpos, &entcount, sizeof(entcount));
    bufpos += sizeof(entcount);


    //add on each entity in this level to the packet
    for(entaddress = 0; entaddress < MAXENTITIES; entaddress++) {
        if(lvl[clientroom].Ent[entaddress].isUsed && lvl[clientroom].Ent[entaddress].isNetworked ) {  //found ent to add to the packet

            memcpy(packetbuffer + bufpos, &entaddress, sizeof(entaddress));
            bufpos += sizeof(entaddress);
            memcpy(packetbuffer + bufpos, &lvl[clientroom].Ent[entaddress].Yaw, sizeof(lvl[clientroom].Ent[entaddress].Yaw));
            bufpos += sizeof(lvl[clientroom].Ent[entaddress].Yaw);
            memcpy(packetbuffer + bufpos, &lvl[clientroom].Ent[entaddress].pos.c.x, sizeof(lvl[clientroom].Ent[entaddress].pos.c.x));
            bufpos += sizeof(lvl[clientroom].Ent[entaddress].pos.c.x);
            memcpy(packetbuffer + bufpos, &lvl[clientroom].Ent[entaddress].pos.c.y, sizeof(lvl[clientroom].Ent[entaddress].pos.c.y));
            bufpos += sizeof(lvl[clientroom].Ent[entaddress].pos.c.y);
            memcpy(packetbuffer + bufpos, &lvl[clientroom].Ent[entaddress].pos.c.z, sizeof(lvl[clientroom].Ent[entaddress].pos.c.z));
            bufpos += sizeof(lvl[clientroom].Ent[entaddress].pos.c.z);

        }
    }

    if(bufpos <= 255) {
        datasize = bufpos;
        memcpy(packetbuffer + 1, &datasize, sizeof(datasize)); //size of all data
    }
    else {
        std::println("ServerClass::BroadcastEntityData packet too big; not sent");
        return;
    }

    ENetPacket * packet = enet_packet_create (packetbuffer, bufpos+1, 0);
    if(packet != NULL) {
        int goodsend = -1;
        goodsend = enet_peer_send(Clients[clientaddress].Peer, 0, packet);
        if(goodsend < 0) std::println("ServerClass::SendMessages: couldnt send a packet to client #{} error: {}", clientaddress, goodsend);
        enet_host_flush (Host); //do we need to send everytime?
    }
    else std::println("ServerClass::SendMessages: bad packet construction: data not sent");
}


void ServerClass::AddKeepAlive(int clientaddress) {
    int emptymsgaddress = -1;
    uint8_t fiftyfour = 54;

    //find a empty message slot
    for(int i = 0; i < MAXMSGS; i++) {
        if(Clients[clientaddress].MessageBuffer[i].Type == 0) {
            emptymsgaddress = i;
            i = MAXMSGS + 1;
        }
    }

    //no empty spot found
    if(emptymsgaddress == -1) {
        std::println("server: message buffer full");
        return;
    }

    //make sure the client is connected
    if(!Clients[clientaddress].isConnected) {
        std::println("server: atempting to send keep alive to non connected client");
        return;
    }

    Clients[clientaddress].MessageBuffer[emptymsgaddress].Type = 2;
    Clients[clientaddress].MessageBuffer[emptymsgaddress].PayloadSize = 1;
    //some random byte just to have a payload; 54
    memmove(&Clients[clientaddress].MessageBuffer[emptymsgaddress].Payload, &fiftyfour,
            Clients[clientaddress].MessageBuffer[emptymsgaddress].PayloadSize);
}


//let a client create a new world
int ServerClass::CreateNewRoom(int clientaddress, const std::string& mapname) {
    int emptylvl = -1;

    std::println("CreateNewRoom clientaddress: {}", clientaddress);

    //check to see if you already a in a room or own a room
    if(Clients[clientaddress].RoomAddress != -1) {
        const std::string tmpstr = "Server shows you in room #"
                        + IntToStr(Clients[clientaddress].RoomAddress)
                        + " /leaveroom first";
        AddClientTextMessage(tmpstr, clientaddress);
        return -1;
    }

    //Does user already have a room open
    for(int i = 0; i < MAXLVL; i++) {
        if(lvl[i].OwnerAddress == clientaddress) {
            const std::string tmpstr = "Already owner of room #" + IntToStr((i+1));
            AddClientTextMessage(tmpstr, clientaddress);
            return -1;
        }
    }

    //find space to create lvl
    for(int i = 0; i < MAXLVL; i++) {
        if(lvl[i].Loaded == false) {
            emptylvl = i;
            i = MAXLVL;
        }
    }

    //fail if no empty room
    if(emptylvl == -1) {
        AddClientTextMessage("No more space on the server for more rooms", clientaddress);
        return -1;
    }

    if(lvl[emptylvl].Load(mapname.c_str()))    {
        lvl[emptylvl].OwnerAddress = clientaddress;  //put creater in control
        const std::string tmpstr = "Room #" + IntToStr(emptylvl) + " ready!";
        AddClientTextMessage(tmpstr, clientaddress);
        return emptylvl;
    }
    else {
        AddClientTextMessage("Level XML file could not load.", clientaddress);
        return -1;
    }
}

void ServerClass::CloseRoom(int clientaddress) {
    if(clientaddress < 0 || clientaddress > MAXCLIENTS) {
        std::println("ServerClass::CloseRoom: invalid client address: {}", clientaddress);
        return;
    }
    
    const int clientsRoom = Clients[clientaddress].RoomAddress;
    if(clientsRoom == -1) {
        const std::string tmpstr = "You are not in a room";
        AddClientTextMessage(tmpstr, clientaddress);
        return;
    }

    //See if we are the room owner
    if(lvl[clientsRoom].OwnerAddress != clientaddress) {
        const std::string tmpstr = "Room #" + IntToStr(clientsRoom) + " is owned by "
                            + Clients[lvl[clientsRoom].OwnerAddress].Name;
        AddClientTextMessage(tmpstr, clientaddress);
        return;
    }

    //kick all clients in room
    for(int i = 0; i < MAXENTITIES; i++) {
        if(lvl[clientsRoom].Ent[i].ClientAddress != -1) {
            int targetclient = lvl[clientsRoom].Ent[i].ClientAddress;
            std::println("ServerClass::CloseRoom: Kicking client #{} from room #{}", targetclient, clientsRoom);
            //client should respond to this but if not its the servers problem
            AddClientTextMessage("/roomkick", targetclient);
            Clients[targetclient].RoomAddress = -1;
            lvl[clientsRoom].Ent[i].ClientAddress = -1;
            lvl[clientsRoom].Ent[i].Remove();
            lvl[clientsRoom].Ent[i].Clear();
        }
    }

    //unload level
    if(lvl[clientsRoom].Loaded) {
        lvl[clientsRoom].Unload();
        const std::string tmpstr = "Room #" + IntToStr(clientsRoom) + " closed";
        AddClientTextMessage(tmpstr, clientaddress);
    }
    else {
        const std::string tmpstr = "Room #" + IntToStr(clientsRoom) + " was not loaded";
        AddClientTextMessage(tmpstr, clientaddress);
    }

    return;
}

void ServerClass::JoinRoom(int clientaddress, int roomtojoin) {
    //std::println("ServerClass::JoinRoom: got join room #{}", roomtojoin);

    //make sure room to join is in range
    if(roomtojoin < 0 || roomtojoin > MAXLVL) {
        std::string tmpstr = "Room #" + IntToStr(roomtojoin) + " is not valid";
        AddClientTextMessage(tmpstr, clientaddress);
        return;
    }

    //need a level to join
    if(!lvl[roomtojoin].Loaded) {
        std::string tmpstr = "Room #" + IntToStr(roomtojoin) + " has not been loaded";
        AddClientTextMessage(tmpstr, clientaddress);
        return;
    }

    //check to make this client isnt already in a room
    if(Clients[clientaddress].RoomAddress != -1) {
        std::string tmpstr = "You are already in room " + IntToStr(Clients[clientaddress].RoomAddress);
        AddClientTextMessage(tmpstr, clientaddress);
        return;
    }

    //add the joining player
    lvl[roomtojoin].AddPlayerEntity(clientaddress);
    Clients[clientaddress].RoomAddress = roomtojoin;

    if(clientaddress == 0) {    //local client
        LC.lvl = lvl[roomtojoin];

        //find the client entity
        int entaddress = -1;
        for(int i = 0; i < MAXENTITIES; i++) {
            if(LC.lvl.Ent[i].ClientAddress == 0) {
                entaddress = i;
                i = MAXENTITIES + 1;
            }
        }

        if(entaddress == -1) LC.EntityAddress = 0;
        else LC.EntityAddress = entaddress;

        LC.DrawWorld = true; //tell the client to start rendering
        return;
    }
    else { //remote client
        //net client; generate a world string and send it to client trigger it to start rendering
        std::string datastr = "/datastr ";

        //level details
        datastr += "/lvl map=" + lvl[roomtojoin].MapName;

        //entity details
        for(int i = 0; i < MAXENTITIES; i++) {
            if(lvl[roomtojoin].Ent[i].isUsed && lvl[roomtojoin].Ent[i].isNetworked) {
                //start new entity description
                datastr += " /e";

                //id
                datastr += " id=" + IntToStr(lvl[roomtojoin].Ent[i].ID);
                if(lvl[roomtojoin].Ent[i].ClientAddress == clientaddress) {
                    datastr += " isme=1";
                }
                
                //position
                datastr += " x=";
                datastr += IntToStr(lvl[roomtojoin].Ent[i].pos.c.x);
                datastr += " y=";
                datastr += IntToStr(lvl[roomtojoin].Ent[i].pos.c.y);

                //model
                datastr += " md=" + lvl[roomtojoin].Ent[i].ModelFile;
                datastr += " tx=" + lvl[roomtojoin].Ent[i].TextureFile;
            }
        }

        AddClientTextMessage(datastr, clientaddress);

        return;
    }
}

bool ServerClass::ExecuteCommand(const std::string &command, const std::string &arg, int clientaddress) {
    if (command == "xyz" ) {
        //do nothing
    }
    else if (command == "clstr") {
        //std::println("server: data string: {}", arg);
        HandleDataString(arg, clientaddress);
    }
    else if (command == "createroom" ) {
        if(arg != "") {
            int createdroom = CreateNewRoom(clientaddress, arg);    //make the room

            if(createdroom != -1) {
                JoinRoom(clientaddress, createdroom);   //join the client to the room they created
                AddClientTextMessage("type /startrace numberoflaps to begin", clientaddress);
            }
        }
    }
    else if (command == "closeroom" ) {
        CloseRoom(clientaddress);
    }
    else if (command == "leftroom") {
        int entaddress = -1;
        int currentroom = Clients[clientaddress].RoomAddress;

        //find the entity to delete from the room
        for(int i = 0; i < MAXPLAYERS; i++ ) {
            if(currentroom != -1) {
                if(lvl[currentroom].Ent[i].ClientAddress == clientaddress) entaddress = i;
            }
        }

        //delete the entity and reset the players room number
        if(entaddress != -1) {
            int spawntoclear = lvl[currentroom].Ent[entaddress].StartpointAddress;
            lvl[currentroom].SpawnPoint.isUsed[spawntoclear] = -1; //clear the start point
            lvl[currentroom].Ent[entaddress].Remove();  //remove from the world
            lvl[currentroom].Ent[entaddress].Clear();   //clear all ent data
        }

        if( lvl[currentroom].OwnerAddress == clientaddress )    //close the room if the owner is leaving
            CloseRoom(Clients[clientaddress].RoomAddress);

        Clients[clientaddress].RoomAddress = -1;
    }
    else if (command == "join") {
        int roomtojoin = -1;
        roomtojoin = atoi( arg.c_str() );
        if(roomtojoin != -1) JoinRoom(clientaddress, roomtojoin);
    }
    else if (command == "hasjoined") {
        Clients[clientaddress].hasJoinedRoom = true;
    }
    else if (command == "disconnect" ) {
        std::println("requesting disconnect for client: {}", clientaddress);
        DisconnectClient( clientaddress );
    }
    else if (command == "startrace" ) {
        int targetlvl = Clients[clientaddress].RoomAddress;

        //only the room owner can start a race
        if(lvl[targetlvl].OwnerAddress != clientaddress) {
            AddClientTextMessage("only the room owner can do that", clientaddress);
            return false;
        }

        int laps = atoi( arg.c_str() );
        if(laps < 1 || laps > MAXLAPS) {
            AddClientTextMessage("Laps must be between 1 and " + IntToStr(MAXLAPS), clientaddress);
            return false;
        }

        lvl[targetlvl].Game.RaceLaps = laps;

        //set up timers
        lvl[targetlvl].Game.StartGameOffset = SDL_GetTicks64();
        lvl[targetlvl].Game.StartGameTime = SDL_GetTicks64() + 5000;
        lvl[targetlvl].Game.StartGameTimer = 5;
    }
    else if (command == "txtmsg" ) {
        std::string messagetobroadcast = "";

        //only add client name to non server messages
        if(clientaddress != 0) messagetobroadcast = "/txtmsg " + Clients[clientaddress].Name + ": " + arg;
        else messagetobroadcast = "/txtmsg " + arg;

        BroadcastTxtMessage(messagetobroadcast);
        std::println("ServerClass::ExecuteCommand: server got a message from client: ?; {}", arg);
    }
    else {
        AddClientTextMessage("no such command", clientaddress);
        std::println("server: command \"{}\" not recognized", command);
        return false;
    }

    return true;
}

//networking
void ServerClass::CheckNetEvents() {
    ENetEvent event;

    // Wait up to 1 millisecond for an event.
    while (enet_host_service (Host, & event, 1) > 0)  {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                HandleNewConnection(event);
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                HandleNewPacket(event);
                // Clean up the packet now that we're done using it.
                enet_packet_destroy (event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                std::println("peer disconnected: {:p}", event.peer->data);
                // Reset the peer's client information.
                event.peer -> data = NULL;
                break;
            case ENET_EVENT_TYPE_NONE:
                std::println("ServerClass::CheckNetEvents(): Mysterious event type none: from ip:{}:{} data: {:p}",
                    event.peer->address.host, event.peer->address.port, event.peer->data);
                break;
        }
    }

}

void ServerClass::HandleDataString(const std::string& datastr, int clientaddress) {
    TypeKeyValue TKV[63];
    //if(!Clients[clientaddress].hasJoinedRoom) return;

    ParseTKV(datastr, TKV);

    //process the new data
    for(int i = 0; i < 63; i++) {
        if(TKV[i].Type == ""); //do nothing; empty var
        else if( TKV[i].Type == "user" ) {
            int nameaddress = FindKeyByName(TKV[i].KV, "n");
            if(nameaddress != -1) {
                if(TKV[i].KV[nameaddress].sValue != "unnamed") Clients[clientaddress].Name = TKV[i].KV[nameaddress].sValue;
                else Clients[clientaddress].Name = "n00b";
            }

            int versionaddress = FindKeyByName(TKV[i].KV, "v");
            std::println("ServerClass::HandleDataString: versionaddress: {}", versionaddress);

            /* cant get this to work!
            if(versionaddress != -1) {
                char version[23];
                strcpy(version, TKV[i].KV[versionaddress].sValue.c_str());

                if(version != VERSION) {
                    std::println("ServerClass::HandleDataString: old client c={} s={}", version, VERSION);
                    DisconnectClient(clientaddress);
                }
            }
            */
        }
        /*else if( TKV[i].Type == "keys" ) {
            int acceladdress = FindKeyByName(TKV[i].KV, "a");
            int brakeaddress = FindKeyByName(TKV[i].KV, "b");
            int leftaddress = FindKeyByName(TKV[i].KV, "l");
            int rightaddress = FindKeyByName(TKV[i].KV, "r");
            int jumpaddress = FindKeyByName(TKV[i].KV, "j");

            if(acceladdress != -1) {
                int doaccel = atoi( TKV[i].KV[acceladdress].sValue.c_str() );
                if(doaccel == 1) Clients[clientaddress].Keys.accel = true;
                else Clients[clientaddress].Keys.accel = false;
            }
            if(brakeaddress != -1) {
                int dobrake = atoi( TKV[i].KV[brakeaddress].sValue.c_str() );
                if(dobrake == 1) Clients[clientaddress].Keys.brake = true;
                else Clients[clientaddress].Keys.brake = false;
            }
            if(leftaddress != -1) {
                int doleft = atoi( TKV[i].KV[leftaddress].sValue.c_str() );
                if(doleft == 1) Clients[clientaddress].Keys.left = true;
                else Clients[clientaddress].Keys.left = false;
            }
            if(rightaddress != -1) {
                int doright = atoi( TKV[i].KV[rightaddress].sValue.c_str() );
                if(doright== 1) Clients[clientaddress].Keys.right = true;
                else Clients[clientaddress].Keys.right = false;
            }
            if(jumpaddress != -1) {
                int dojump = atoi( TKV[i].KV[jumpaddress].sValue.c_str() );
                if(dojump == 1) Clients[clientaddress].Keys.jump = true;
                else Clients[clientaddress].Keys.jump = false;
            }
        }*/
        else std::println("HandleWorldString: got unknown type \"{}\"", TKV[i].Type);
    }
}

void ServerClass::ProcessKeyState(char *packetbuffer, int clientaddress) {
    int bufpos = 0;
    uint8_t binarydatatype = 0;
    uint16_t datasize = 0;               //size of the data to process
    uint32_t timestamp = 0;             //SDL ticks used as a way of ordering packet

    memcpy(&binarydatatype, packetbuffer + bufpos, sizeof(binarydatatype)); //std::print("type: {}", static_cast<int>(binarydatatype));
    bufpos += sizeof(binarydatatype);
    memcpy(&datasize, packetbuffer + bufpos, sizeof(datasize)); //std::print("type: {}", static_cast<int>(datasize));
    bufpos += sizeof(datasize);
    memcpy(&timestamp, packetbuffer + bufpos, sizeof(timestamp)); //std::print(" timestamp: {}", static_cast<uint32_t>(timestamp));
    bufpos += sizeof(timestamp);

    //std::println("timestamp from client: {} last client timestamp: {}", timestamp, Clients[clientaddress].LastTimeStamp);
    if(timestamp <= Clients[clientaddress].LastTimeStamp)   //check to see if this isnt an old packet
        return;
    Clients[clientaddress].LastTimeStamp = timestamp;

    if(datasize <= 255) {
        memcpy(&Clients[clientaddress].Keys.accel, packetbuffer + bufpos, 1);
        bufpos += 1;
        memcpy(&Clients[clientaddress].Keys.brake, packetbuffer + bufpos, 1);
        bufpos += 1;
        memcpy(&Clients[clientaddress].Keys.left, packetbuffer + bufpos, 1);
        bufpos += 1;
        memcpy(&Clients[clientaddress].Keys.right, packetbuffer + bufpos, 1);
        bufpos += 1;
        memcpy(&Clients[clientaddress].Keys.jump, packetbuffer + bufpos, 1);
        bufpos += 1;
    }
    else {
        std::println("ServerClass::ProcessKeyState packet too big ignoring");
        return;
    }
}

void ServerClass::HandleTextMessage(int messageaddress) {
    std::string localstring = "";
    std::string found_arg = "";
    std::string found_command = "";
    size_t space_position = 0;

    localstring = MessageBuffer[messageaddress].Payload;
    //std::println("handling \"{}\"", localstring);

    //look for a / at the beginning of the string then pull out the command
    if(localstring.at(0) == '/') {
        space_position = localstring.find(' ');
        if( space_position != std::string::npos ) {
            //found space pull out substring
            found_command = localstring.substr(1, space_position - 1);
            found_arg = localstring.substr( space_position + 1, localstring.size() );

        } else {
            //pull out substring
            found_command = localstring.substr(1, localstring.size());
        }

        //got our command now run it; if command not found delete it so its not sent the the client
        if(!ExecuteCommand(found_command, found_arg, MessageBuffer[messageaddress].ClientAddress))    MessageBuffer[messageaddress].Clear();
    }
    else  { //not a command so broadcast it to the clients
        BroadcastTxtMessage(localstring);
    }
}

void ServerClass::HandleNewConnection(ENetEvent localevent)  {
    int emptyclient = -1;

    //find and empty spot for our new client; start at 1 cuz LC is always connected
    for(int i = 1; i < MAXCLIENTS; i++) {
        if(!Clients[i].isConnected) {  //no connection here
            emptyclient = i;
            i = MAXCLIENTS + 1;
        }
    }

    if(emptyclient != -1) {
        //check address and make sure that someone is not already connected from that address

        //save peer in the peer list
        Clients[emptyclient].Peer = localevent.peer;

        //the place our client is in the index
        Clients[emptyclient].PeerNumber = emptyclient;

        //the name everyone will know this client as
        //NetClients[emptypeer].Name =

        //fill in the pretty ip
        char prettyip[16];
        for( int i = 0 ; i < 16 ; ++i) prettyip[i] = 0; //clear string

        enet_address_get_host_ip(&Clients[emptyclient].Peer->address, prettyip, 15);
        Clients[emptyclient].PrettyIP.assign(prettyip);

        Clients[emptyclient].isConnected = true; //server will now check this client for new data
        AddClientTextMessage("/connectionaccepted welcome to hoverland", emptyclient); //tell the client it

        //print out connection info
        std::println("server: assigned client to position: {} connected from: {} data: {:p}",
            emptyclient, Clients[emptyclient].PrettyIP, localevent.peer->data);

        //first keep alive
        AddKeepAlive(emptyclient);
        Clients[emptyclient].LastKeepAliveTime = SDL_GetTicks64();

        //tell everyone someone connected
        std::string tmpstring = "/txtmsg client connected";
        BroadcastTxtMessage(tmpstring); //let all the clients know we have a new client
    }
    else {
        enet_peer_disconnect_now ( localevent.peer, 0);
    }

}

void ServerClass::DisconnectClient(int clientaddress) {
    if(clientaddress < 1 || clientaddress > MAXCLIENTS) {
        std::println("server: asked to disconnect and impossable client#, #{}", clientaddress);
        return;
    }

    if(!Clients[clientaddress].isConnected) return;

    //check to see if there are any entities linked to this client
    int lvlclientsin = Clients[clientaddress].RoomAddress;
    if(lvlclientsin != -1) {
        for(int i = 0; i < MAXENTITIES; i++) {
            if(lvl[lvlclientsin].Ent[i].ClientAddress == clientaddress) {
                std::println("ServerClass::DisconnectClient: disconnecting client with ent attached; removing ent #{}", i);
                lvl[lvlclientsin].Ent[i].Remove();
            }
        }
    }

    //tell the client he is disconnected
    //ENetPacket * packet = enet_packet_create (disconnectstring.c_str(), 11, 0);
    //enet_peer_send(Clients[clientaddress].Peer, 0, packet);

    //send out the packet
    //enet_host_flush (Host);

    //release the client in enet
    enet_peer_disconnect_now( Clients[clientaddress].Peer , 0);
    enet_peer_reset (Clients[clientaddress].Peer);

    //reset all client data
    Clients[clientaddress].Clear();   //this causes a crash for some reason

    //tell the local client ie '0' that he we disconnected a client
    AddClientTextMessage("client was disconnected", 0);
}

/*
message types
0 NULL does nothing used to mark message as nothing
1 text message/command
2 keep alive from server to client back to server
3 entity data
4 key state
*/

void ServerClass::HandleNewPacket(ENetEvent localevent) {
    bool baddata = false;
    int peernumber = -1;
    uint16_t textmessagesize = 0;
    uint8_t messagetype = 0;
    int bufpos = 0; //where we will read from in the packetbuffer
    char packetbuffer[1023];    //1kB of buffer
    char textmessagedata[MAXMSGLENTH];
    char charprettyip[16];
    memset(charprettyip, '\0', sizeof(charprettyip));
    std::string stringprettyip = "";

    //figure out who this packet is from
    enet_address_get_host_ip(&localevent.peer->address, charprettyip, 15);
    stringprettyip = charprettyip;

    //skip client 0 cuz a net client shouldnt be client 0
    for(int i = 1; i < MAXCLIENTS; i++) {
        if(stringprettyip == Clients[i].PrettyIP) {
            peernumber = i;
            i = MAXCLIENTS +1;
        }
    }

    //process the new packet
    if(peernumber != -1) {
        //clear arrays
        memset(packetbuffer, '\0', sizeof(packetbuffer));
        memset(textmessagedata, '\0', sizeof(textmessagedata));

        memmove(packetbuffer, localevent.packet->data, localevent.packet->dataLength);
        //std::println("server: processing packet \"{}\"", packetbuffer);

        //seperate and process the messages
        while(packetbuffer[bufpos] != '\0') {
            //copy data into buffer
            memmove(&messagetype, packetbuffer + bufpos, sizeof(messagetype));
            bufpos += sizeof(messagetype);
            memmove(&textmessagesize, packetbuffer + bufpos, sizeof(textmessagesize));
            bufpos += sizeof(textmessagesize);

            //std::println("got packet type: {} size: {}", messagetype, textmessagesize);

            if(textmessagesize > 0 && textmessagesize <= 1023) {
                //std::println("got a message size of: {}", textmessagesize);
                memmove(textmessagedata, packetbuffer + bufpos, textmessagesize);
                bufpos += textmessagesize;
            }
            else baddata = true;

            //examine the data JAE - 3/24/23 uint8_t is 0 to 255
            //if(messagetype <= 0 || messagetype > 255) baddata = true;

            //put new message in our message buffer
            if(!baddata) {
                if(messagetype == 1) {
                    std::string msgstring = textmessagedata;
                    //add message to the servers message buffer
                    AddServerTextMessage(msgstring, peernumber);
                }
                else if(messagetype == 2)   Clients[peernumber].LastKeepAliveTime = SDL_GetTicks64();
                else if(messagetype == 4)   ProcessKeyState(packetbuffer, peernumber);
                else std::println("server: received and ignored type: {} message",
                    static_cast<char>(messagetype));
            }

            //clear buffers
            textmessagesize = 0;
            messagetype = 0;
            for( int tempi = 0 ; tempi < MAXMSGLENTH; tempi++) textmessagedata[tempi] = '\0'; //clear char array
        }
    }
    else std::println("server: got a packet from a unknown client");
}

void ServerClass::ReadMessages() {
    MSGClass localmessage;
    int msgcount = 0;

    //get local clients key stat
    Clients[0].Keys = LC.Keys;

    //read messages from local client
    for(int i = 0; i < MAXMSGS; i++) {
        if(LC.MessageBuffer[i].Type != 0)   { //put the data into the server buffers
            MessageBuffer[msgcount].ClientAddress = 0;     //all commands from client 0 are not limited
            MessageBuffer[msgcount].Type = LC.MessageBuffer[i].Type;

            //verify correct message size
            if(LC.MessageBuffer[i].PayloadSize <= MAXMSGLENTH && LC.MessageBuffer[i].PayloadSize > 0) {
                MessageBuffer[msgcount].PayloadSize = LC.MessageBuffer[i].PayloadSize;
            }
            else {
                MessageBuffer[msgcount].PayloadSize = -1;  //bad length
                std::println("ReadCommands: bad message length");
            }

            //move command data to the servers buffer to be read with all the other commandse
            memcpy(&MessageBuffer[msgcount].Payload, &LC.MessageBuffer[i].Payload, MessageBuffer[msgcount].PayloadSize);
            //std::println("server: reading message type: {} size: {} bytes with a payload of \"{}\"",
            //    MessageBuffer[msgcount].Type, MessageBuffer[msgcount].PayloadSize, MessageBuffer[msgcount].Payload);
            msgcount++;

            LC.MessageBuffer[i].Clear();
        }
    }

    //get data from remote clients
    if( isRunning && isAcceptingRemoteClients ) {
        CheckNetEvents();
        //read messages
    }
}

static uint64_t LastKeepAliveBroadcast = 0;
//static uint64_t LastWorldDataBroadcast = 0;
//static uint64_t LastEntDataBroadcast = 0;
void ServerClass::CheckKeepAlive() {
    uint64_t currenttime = SDL_GetTicks64();

    //send this out every server tick
    for(int i = 1; i < MAXCLIENTS; i++) {   //skip 0; dont send to server
        BroadcastEntityData(i);
    }

    EntityAddRemove();

    //keep alives every 1 sec
    if( 1000 < (currenttime - LastKeepAliveBroadcast) ) {
        for(int i = 1; i < MAXCLIENTS; i++) {
            if(Clients[i].isConnected) {
                //std::println("current time: {} client #{} last keep alive: {}", currenttime, i, Clients[i].LastKeepAliveTime);
                if( 10000 < (currenttime - Clients[i].LastKeepAliveTime) ) {
                    std::println("ServerClass::CheckKeepAlive: disconnecting client #{}", i);
                    DisconnectClient(i);
                }

                AddKeepAlive(i);
            }
        }
        BroadcastRoomInfo();

        LastKeepAliveBroadcast = currenttime;
    }
}

void ServerClass::ProcessMessages() {
    CheckKeepAlive();   //reimplement as timed events or something

    //read each message
    for(int i = 0; i < 127; i++) {
        //std::print("; {} = {}", i, hxServer.MessageBuffer[i].Type);
        switch(MessageBuffer[i].Type) {
            case 0:
                //do nothing
                break;
            case 1:
                HandleTextMessage(i);
                break;
        }

        //command used; there for removed
        MessageBuffer[i].Clear();
    }
}

void ServerClass::SendMessages() {
    int msgcount = 0;
    int bufpos = 0; //where we will wright to in the packetbuffer
    char packetbuffer[1023];    //1kB of buffer
    char textmessagedata[MAXMSGLENTH];
    uint16_t textmessagesize;
    uint8_t messagetype;

    //find all connected clients
    for(int i = 0; i < MAXCLIENTS; i++) {
        if(Clients[i].isConnected) {
            if(i == 0) {    //local client
                //find a message
                for(int j = 0; j < MAXMSGS; j++) {
                    //move data from client list to local client
                    if(Clients[i].MessageBuffer[j].Type != 0) {
                        LC.MessageBuffer[msgcount].ClientAddress = i;     //client position the message is from
                        LC.MessageBuffer[msgcount].Type = Clients[i].MessageBuffer[j].Type;

                        //verify correct message size
                        if(Clients[i].MessageBuffer[j].PayloadSize <= MAXMSGLENTH && Clients[i].MessageBuffer[j].PayloadSize > 0)
                            LC.MessageBuffer[msgcount].PayloadSize = Clients[i].MessageBuffer[j].PayloadSize;
                        else {
                            LC.MessageBuffer[msgcount].PayloadSize = -1;  //bad length
                            std::println("ServerClass::SendMessages(): bad message length");
                        }

                        //move command data to the servers buffer to be read with all the other commands
                        memcpy(&LC.MessageBuffer[msgcount].Payload, &Clients[i].MessageBuffer[j].Payload, Clients[i].MessageBuffer[j].PayloadSize);
                        //std::println("server: buffer output; message address {} type: {} that is {} bytes with a payload of \"{}\"",
                        //    msgcount, LC.MessageBuffer[msgcount].Type, LC.MessageBuffer[msgcount].PayloadSize,
                        //    LC.MessageBuffer[msgcount].Payload);
                        msgcount++;

                        //message sent therefor no longer need
                        MessageBuffer[msgcount].Clear();
                        Clients[i].MessageBuffer[j].Clear();
                    }
                }

                //reset the position of message writes
                msgcount = 0;
            }
            else {  //this is a network client and the data need to be serialized
                memset(packetbuffer, '\0', sizeof(packetbuffer)); //clear array
                bufpos = 0;

                for(int j = 0; j < MAXMSGS; j++) {
                    //move data from client list to packet buffer
                    if(Clients[i].MessageBuffer[j].Type != 0) {
                        //buffer format: type 1 byte | size of data in 1 byte | data

                        memset(textmessagedata, '\0', sizeof(textmessagedata));
                        textmessagesize = 0;
                        messagetype = 0;

                        messagetype = static_cast<uint8_t>(Clients[i].MessageBuffer[j].Type); //message type
                        textmessagesize = static_cast<uint16_t>(Clients[i].MessageBuffer[j].PayloadSize);  //message size

                        //message data
                        if(Clients[i].MessageBuffer[j].PayloadSize <= MAXMSGLENTH && Clients[i].MessageBuffer[j].PayloadSize > 0) {   //check for valid size
                            memcpy(&textmessagedata, &Clients[i].MessageBuffer[j].Payload, Clients[i].MessageBuffer[j].PayloadSize);
                        }
                        else {
                            textmessagesize = 0;  //bad length
                            std::println("ServerClass::SendMessages: bad message length"); //should return here from another function
                        }

                        //move command data to the servers buffer in to the packet buffer; 254 so we know we dont over wright the null char
                        if(textmessagesize != 0 && 1022 > (textmessagesize+3) ) {   //check for valid size and that we arnt going to over wright the buffer
                            //std::print("server: sending packet type:{}", messagetype);
                            memmove(packetbuffer + bufpos, &messagetype, sizeof(messagetype));   //copy type into buffer
                            bufpos += sizeof(messagetype);
                            //std::print(" size: {}", textmessagesize);

                            memmove(packetbuffer + bufpos, &textmessagesize, sizeof(textmessagesize));   //copy data size into buffer
                            bufpos += sizeof(textmessagesize);
                            //std::println(" data: \"{}\"", textmessagedata);

                            memmove(packetbuffer + bufpos, textmessagedata, textmessagesize);
                            bufpos += textmessagesize;

                            msgcount++;
                        }

                        //message sent therefor no longer need
                        MessageBuffer[msgcount].Clear();
                        Clients[i].MessageBuffer[j].Clear();
                    }
                }

                //send packet if we have anything to send
                if(msgcount > 0) {
                    //the +1 in the length will include the null char to terminate the string
                    //ENetPacket * packet = enet_packet_create (packetbuffer, strlen(packetbuffer)+1, ENET_PACKET_FLAG_RELIABLE);
                    ENetPacket * packet = enet_packet_create (packetbuffer, bufpos+2, ENET_PACKET_FLAG_RELIABLE);
                    if(packet != NULL) {
                        int goodsend = -1;
                        goodsend = enet_peer_send(Clients[i].Peer , 0, packet);
                        if(goodsend < 0) std::println("ServerClass::SendMessages: couldnt send a packet to client #{} error: {}", i, goodsend);

                        enet_host_flush (Host); //do we need to send everytime?
                    }
                    else std::println("ServerClass::SendMessages: bad packet construction: data not sent");
                }

                //reset the position of message writes
                msgcount = 0;
            }
        }
    }
}

void ServerClass::Update() {
	if(isRunning == false)
        return;

	ReadMessages();
	ProcessMessages();

	for(int i = 0; i < MAXLVL; i++) {
		lvl[i].Update();
	}

	SendMessages();
}

void ClientList::Clear() {
    Name = "unnamed";
    isConnected = false;
    hasJoinedRoom = false;
    RoomAddress = -1;
    PeerNumber = -1;
    Peer = nullptr;
    PrettyIP = "";
    LastTimeStamp = 0;
    LastKeepAliveTime = 0;
    Keys.Clear();
    for(int i = 0; i < MAXMSGS; ++i) {
        MessageBuffer[i].Clear();
    }
}

ClientList::ClientList() {
    Clear();
}

ClientList::~ClientList() {
    Clear();
}
