#include <SDL2/SDL.h>
#include <sstream>
#include "stdio.h"
#include "string.h"
#include "client.h"
#include "server.h"
#include "level.h"

extern ButtonArray Dropdownmenus[MAX_DROPDOWNS];
extern UIButton ButtonList[MAX_BUTTONS];

extern bool done;
extern ServerClass hxServer;
//extern level mylvl;    //the one true world

void LocalClient::Update() {
    //get net data here
    if(isConnectedToRemoteServer) CheckNetEvents();

    //this may add client side prediction
    if(isConnectedToRemoteServer && lvl.Loaded) {
        //cout << "is running this code" << endl;
        //lvl.Ent[EntityAddress].ApplyForces(1);  //gravity and input
        //if(Keys.right)  lvl.Ent[EntityAddress].Yaw += 0.025;
        //if(Keys.left)   lvl.Ent[EntityAddress].Yaw -= 0.025;
        //lvl.Ent[EntityAddress].onground = 0; lvl.Ent[EntityAddress].antionground = 0;   //reset ground info

        const float accel = 0.003;
        //float jumppower = 0.05;
        //float steerspeed = 0.025;
        //float gravity = 0.001;
        //float minzvel = -0.04;

        //only apply controls to clients
        //if(ClientAddress == -1) return;

        lvl.Ent[EntityAddress].pos.o.z = lvl.Ent[EntityAddress].pos.c.z;

        //gliding physics
        lvl.Ent[EntityAddress].pos.dophys(0.992, 1);

        //get a vector of heading and speed
        vector2d v (sinf(lvl.Ent[EntityAddress].Yaw), cosf(lvl.Ent[EntityAddress].Yaw));
        //vector2d h = v.getperp();

        //xy the heading vector
        v.scalef(accel);
        v.divf(1);

        if(Keys.accel) lvl.Ent[EntityAddress].pos.c+=v;

        if(Keys.right)  lvl.Ent[EntityAddress].Yaw += 0.025;
        if(Keys.left)   lvl.Ent[EntityAddress].Yaw -= 0.025;
    }

    //handle recieved messages
    ProcessMessages();

    //send new messages
    if(isConnectedToRemoteServer) SendMessages();
}

void LocalClient::AddTextMessage(string newtextmessage) {
    int emptymsgaddress = -1;

    //find a empty message slot
    for(int i = 0; i < MAXMSGS; i++) {
        if(MessageBuffer[i].Type == 0) {
            emptymsgaddress = i;
            i = MAXMSGS + 1;
        }
    }

    //no empty spot found
    if(emptymsgaddress == -1) {
        cout << "client: cant add new message: \"" << newtextmessage << "\"" << "; buffer full" << endl;
        return;
    }

    MessageBuffer[emptymsgaddress].Clear();    //extra safe clear of current address

    if(newtextmessage != "") {
        MessageBuffer[emptymsgaddress].Type = 1;
        MessageBuffer[emptymsgaddress].PayloadSize = strlen(newtextmessage.c_str());
        memcpy(&MessageBuffer[emptymsgaddress].Payload + 0, newtextmessage.c_str(), MessageBuffer[emptymsgaddress].PayloadSize);

        //cout << "client added type: " << MessageBuffer[emptymsgaddress].Type << " that is " << MessageBuffer[emptymsgaddress].PayloadSize << " bytes with a payload of \"" << MessageBuffer[emptymsgaddress].Payload << "\"" << endl;
    }
    return;
}

void LocalClient::BroadcastKeyState() {
    unsigned int bufpos = 0;
    uint8_t binarydatatype = 4;                             //type 4 is key state
    uint16_t datasize = 0;                                  //how much data in bytes we are sending
    uint64_t timestamp = SDL_GetTicks64();                  //SDL ticks used as a way of ordering packets
    char packetbuffer[255];                                 //enough for 64 entities and packet over head
    memset(packetbuffer, '\0', sizeof(packetbuffer));

    //start assembling the packet
    memcpy(packetbuffer + bufpos, &binarydatatype, sizeof(binarydatatype));
    bufpos += sizeof(binarydatatype);
    memcpy(packetbuffer + bufpos, &datasize, sizeof(datasize)); //this will always be zero here
    bufpos += sizeof(datasize);
    memcpy(packetbuffer + bufpos, &timestamp, sizeof(timestamp));
    bufpos += sizeof(timestamp);

    //key data
    memcpy(packetbuffer + bufpos, &Keys.accel, sizeof(Keys.accel));
    bufpos += sizeof(Keys.accel);
    memcpy(packetbuffer + bufpos, &Keys.brake, sizeof(Keys.brake));
    bufpos += sizeof(Keys.brake);
    memcpy(packetbuffer + bufpos, &Keys.left, sizeof(Keys.right));
    bufpos += sizeof(Keys.left);
    memcpy(packetbuffer + bufpos, &Keys.right, sizeof(Keys.right));
    bufpos += sizeof(Keys.right);
    memcpy(packetbuffer + bufpos, &Keys.jump, sizeof(Keys.jump));
    bufpos += sizeof(Keys.jump);

    if(bufpos <= 255) {
        datasize = bufpos;
        //cout << "datasize: " << datasize << endl;
        memcpy(packetbuffer + 1, &datasize, sizeof(datasize)); //size of all data
    }
    else {
        cout << "LocalClient::BroadcastKeyState packet too big; not sent" << '\n';
        return;
    }

    ENetPacket * packet = enet_packet_create (packetbuffer, bufpos+1, 0);
    if(packet != NULL) {
        int goodsend = -1;
        goodsend = enet_peer_send(ServerPeer, 0, packet);
        if(goodsend < 0) cout << "ServerClass::SendMessages: couldnt send a packet to server; error #" << goodsend << '\n';
        enet_host_flush (ClientHost); //do we need to send everytime?
    }
    else cout << "ServerClass::SendMessages: bad packet construction: data not sent" << '\n';
}

void LocalClient::SendClientInfo() {
    string datastr = "";

    if(!isConnectedToRemoteServer) return;

    datastr = "/clstr /user n=";
    datastr += Username;

    /*
    datastr += " v=";
    datastr += ENGINEVERSION;
    */

    //cout << "LocalClient::SendClientInfo: " << datastr << endl;
    AddTextMessage(datastr);
}

bool LocalClient::ExecuteCommand(const string &command, const string &arg) {
    if (command == "connectionaccepted" ) {
        UI_AddMSG("connected: " + arg);
        //isConnected = true; should always be connected do i need this in LC?
    }
    else if (command == "roomkick" || command == "leaveroom") {
        if(command == "roomkick") {
            UI_AddMSG("kicked from room");
        } else {
            UI_AddMSG("left room");
        }

        //client should never unload the level in a local game, set the server do it
        //JAE 3/31/23 - Sure about that?
        if(lvl.Loaded) {
            //if(isConnectedToRemoteServer) {
                cout << "LocalClient::ExecuteCommand: Unloading level with local server, idea bad?" << '\n';
                lvl.Unload();
                AddTextMessage("/leftroom");
                EntityAddress = -1;
                DrawWorld = false;
            //}
        }
    }
    else if (command == "disconnected" ) {  //needs to implemented on the server
        UI_AddMSG("signaled to disconnect, ignored");
    }
    else if (command == "datastr" ) {
        //cout << "client received datastr: " << arg << endl;
        //if( isConnectedToRemoteServer ) HandleDataString(arg); //if we are connected to a local server we can just look at the local data
        HandleDataString(arg);
    }
    else if (command == "host" ) {
        if(arg == "1") {
            UI_AddMSG("accepting connections");
            hxServer.isAcceptingRemoteClients = true;
        }
        if(arg == "0") {
            UI_AddMSG("not accepting connections");
            hxServer.isAcceptingRemoteClients = false;
        }
    }
    else if (command == "connect" ) {
        if(!isConnected) {
            //attempt network connection if not trying for local
            if(arg != "local") NetServerConnect(arg);
            else isConnectedToRemoteServer = false;    //set type of connection; this should prob be a bool isConnectedToRemoteServer and used to turn server on and off

            //if the socket connection worked formally ask for an offical connection and stop the local server
            if(isConnectedToRemoteServer) {
                hxServer.Stop();
                isConnected = true;
                SendClientInfo();   //broadcast info about the client to the server
                ButtonList[Dropdownmenus[1].Buttons[0]].label = "Go Offline";   //change out button to reflect the fact that we are connected
            }
            else UI_AddMSG("connection attempt failed");
        }
        else {
            AddTextMessage("/disconnect");
        }
    }
    else if (command == "txtmsg" ) {
        UI_AddMSG(arg);
    }
    else if (command == "name" ) {
        if(arg != "unnamed") {
            Username = arg;
            UI_AddMSG("your name is " + Username);
            SendClientInfo();
        }
        else UI_AddMSG("try another name");
    }
    else if (command == "exit" || command == "quit") {
        if(isConnectedToRemoteServer)  AddTextMessage("/disconnect");
        done = true;
    }
    else if (command == "mapmenu" ) {   //turn the map menu on or off
        if(Dropdownmenus[0].Draw == true) { //close all dropdowns
            DisplayMapDropdown(false);
        }
        else {
            DisplaySectorsListDropdown(false);
            DisplayMapDropdown(true);
        }
    }
    else if (command == "mainmenu" ) {   //turn the map menu on or off
        if(Dropdownmenus[1].Draw == true) { //close all dropdowns
            DisplayMainMenuDropdown(false);
            DisplayMapDropdown(false);
            DisplaySectorsDropdown(false);
            DisplaySectorsListDropdown(false);
            DisplayMapDropdown(false);
        }
        else DisplayMainMenuDropdown(true);
    }
    else if (command == "sectormenu" ) {   //turn the map menu on or off
        if(Dropdownmenus[2].Draw == true) {
            DisplayMapDropdown(false);
            DisplaySectorsListDropdown(false);
            DisplaySectorsDropdown(false);
        }
        else {
            DisplayMapDropdown(false);
            DisplaySectorsListDropdown(false);
            DisplaySectorsDropdown(true);
        }
    }
    else if (command == "openrooms" ) {   //turn the map menu on or off
        if(Dropdownmenus[3].Draw == true) DisplaySectorsListDropdown(false);
        else {
            DisplayMapDropdown(false);  //close the other drop down in this section
            DisplaySectorsListDropdown(true);
        }
    }
    else return false;

    //happens if the command was found
    return true;
}

void LocalClient::SendKeepAlive() {
    int emptymsgaddress = -1;
    const unsigned char fiftyfour = 54;


    //find a empty message slot
    for(int i = 0; i < MAXMSGS; i++) {
        if(MessageBuffer[i].Type == 0) {
            emptymsgaddress = i;
            i = MAXMSGS + 1;
        }
    }

    //no empty spot found
    if(emptymsgaddress == -1) {
        cout << "client: message buffer full" << endl;
        return;
    }

    MessageBuffer[emptymsgaddress].Type = 2;
    MessageBuffer[emptymsgaddress].PayloadSize = 1;
    //some random byte just to have a payload; 54
    memmove(&MessageBuffer[emptymsgaddress].Payload, &fiftyfour, MessageBuffer[emptymsgaddress].PayloadSize);

}

static int LastKeepAliveFromServer = 0;
static int LastKeepAliveBroadcast = 0;
//static int LastKeyBroadcast = 0;
void LocalClient::BroadcastTimer() {    //this should prob be called timers
    int currenttime = SDL_GetTicks64();

    if(isConnectedToRemoteServer) {
        //key info X times a sec; 1000ms == 1 sec
        //if( 10 <= (currenttime - LastKeyBroadcast) ) {
        if(DrawWorld) BroadcastKeyState();
            //LastKeyBroadcast = currenttime;
        //}

        //send keep alives every half sec
        if( 500 <= (currenttime - LastKeepAliveBroadcast) ) {
            //cout << "sending keepy alive" << endl;
            SendKeepAlive();
            LastKeepAliveBroadcast = currenttime;
        }

        //check to make sure we have gotten a keep alive with in 2 seconds
        if( 2000 <= (currenttime - LastKeepAliveBroadcast) ) {
            RemoteDisconnect();
        }
    }

    //look to see when we last heard from the clients on our list; clear after 2 seconds
    for(int i = 0; i < MAXCLIENTS; i++) {
        //cout << (currenttime - Clients[i].LastUpdate) << " :currenttime - Clients[i].LastUpdate" << endl;
        if(  2000 <= (currenttime - Clients[i].LastUpdate) ) Clients[i].Clear();
    }
}

void LocalClient::ProcessMessages() {
    MSGClass localmessage;

    BroadcastTimer();

    //read each message
    for(int i = 0; i < MAXMSGS; i++) {
        switch(MessageBuffer[i].Type) {
            case 0:
                //do nothing
                break;
            case 1:
                HandleTextMessage(i);
                break;
        }
    }
}

void LocalClient::HandleDataString(string worldstr) {
    TypeKeyValue TKV[63];
    //turn the world string into something we can use
    ParseTKV(worldstr, TKV);
//cout << "DataString: " << worldstr << endl;
    //process the new data
    for(int i = 0; i < 63; i++) {
        if(TKV[i].Type == ""); //do nothing; empty var
        else if(TKV[i].Type == "lvl") {
            int mapaddress = FindKeyByName(TKV[i].KV, "map");

            if(mapaddress != -1) {
                if(lvl.MapName != TKV[i].KV[mapaddress].sValue) {
                    lvl.Load(TKV[i].KV[mapaddress].sValue);
                    DrawWorld = true; //enable world drawing; prob should go else where
                    AddTextMessage("/hasjoined");
                }
            }
        }
        else if( TKV[i].Type == "clients" ) {     //read client list
            int clientnameaddress[MAXCLIENTS];

            //get addresses for client data
            for(int w = 0; w < MAXCLIENTS; w++) {
                string clientnum;
                stringstream convert (stringstream::in | stringstream::out);
                //itoa(w, roomnum, 10);   //convert w to a string

                convert << w;
                convert >> clientnum;

                clientnameaddress[w] = FindKeyByName( TKV[i].KV, clientnum );
            }

            //assign data to client list
            for(int w = 0; w < MAXCLIENTS; w++) {
                if(clientnameaddress[w] != -1) {
                    //cout << "client #" << w << "'s name is " << TKV[i].KV[clientnameaddress[w]].sValue << endl;
                    Clients[w].Name = TKV[i].KV[clientnameaddress[w]].sValue;
                    Clients[w].LastUpdate = SDL_GetTicks64();
                }
                else Clients[w].Name = "unnamed";
            }
        }
        else if( TKV[i].Type == "room" ) {     //open room data

            int roomaddress[MAXLVL];

            //get addresses for room data
            for(int w = 0; w < MAXLVL; w++) {
                string roomnum;
                stringstream convert (stringstream::in | stringstream::out);
                //itoa(w, roomnum, 10);   //convert w to a string

                convert << w;
                convert >> roomnum;

                roomaddress[w] = FindKeyByName( TKV[i].KV, roomnum );
            }

            //assign data to room list
            for(int w = 0; w < MAXLVL; w++) {
                if(roomaddress[w] != -1) {
                    ButtonList[Dropdownmenus[3].Buttons[w]].label = TKV[i].KV[roomaddress[w]].sValue;
                    //cout << "room #" << w << " is " << TKV[i].KV[roomaddress[w]].sValue << endl;
                }
                else {
                    ButtonList[Dropdownmenus[3].Buttons[w]].label = "(none)";
                    //cout << "room #" << w << " is (none)" << endl;
                }
            }

        }
        else if( TKV[i].Type == "e" && lvl.Loaded ) {     //entity data: check for level; cuz we cant reference entities if there is no world!
            int entaddress = FindKeyByName(TKV[i].KV, "id");
            //int xaddress = FindKeyByName(TKV[i].KV, "x");
            //int yaddress = FindKeyByName(TKV[i].KV, "y");
            //int zaddress = FindKeyByName(TKV[i].KV, "z");
            //int yawaddress = FindKeyByName(TKV[i].KV, "yaw");
            int modeladdress = FindKeyByName(TKV[i].KV, "md");
            int textureaddress = FindKeyByName(TKV[i].KV, "tx");
            int ismeaddress = FindKeyByName(TKV[i].KV, "isme");
            int removeentaddress = FindKeyByName(TKV[i].KV, "rm");

            //move the entaddress from the location in the key/value array to the location in the ent array
            entaddress = atoi( TKV[i].KV[entaddress].sValue.c_str() );

            if(entaddress != -1) {
                if(lvl.Ent[entaddress].isUsed) {
                    //update positions - out moded
                    /*
                    if(yawaddress != -1) lvl.Ent[entaddress].Yaw = atof( TKV[i].KV[yawaddress].sValue.c_str() );
                    if(xaddress != -1 && yaddress != -1)
                        lvl.Ent[entaddress].pos.teleport( atof(TKV[i].KV[xaddress].sValue.c_str()), atof( TKV[i].KV[yaddress].sValue.c_str()) );
                    if(zaddress != -1) lvl.Ent[entaddress].pos.c.z = atof( TKV[i].KV[zaddress].sValue.c_str() );
                    */
                }
                else {
                    if(modeladdress != -1 && textureaddress != -1) {
                        cout << "adding new ent: " << entaddress << endl;
                        //model stuff
                        lvl.Ent[entaddress].ModelFile = TKV[i].KV[modeladdress].sValue;
                        lvl.Ent[entaddress].TextureFile = TKV[i].KV[textureaddress].sValue;

                        //position stuff - out moded
                        /*
                        if(yawaddress != -1) lvl.Ent[entaddress].Yaw = atof( TKV[i].KV[yawaddress].sValue.c_str() );
                        if(xaddress != -1 && yaddress != -1)
                            lvl.Ent[entaddress].pos.o.x = lvl.Ent[entaddress].pos.c.x;
                            lvl.Ent[entaddress].pos.o.y = lvl.Ent[entaddress].pos.c.y;
                            lvl.Ent[entaddress].pos.teleport( atof(TKV[i].KV[xaddress].sValue.c_str()), atof( TKV[i].KV[yaddress].sValue.c_str()) );
                        if(zaddress != -1) {
                            lvl.Ent[entaddress].pos.o.z = lvl.Ent[entaddress].pos.c.z;
                            lvl.Ent[entaddress].pos.c.z = atof( TKV[i].KV[zaddress].sValue.c_str() );
                        }
                        */

                        //how we will know if this is the entity to follow with the camera; aka me
                        if(ismeaddress != -1) {
                            int ismevalue = atoi( TKV[i].KV[ismeaddress].sValue.c_str() );
                            if(ismevalue == 1) EntityAddress = entaddress;
                        }

                        if(! lvl.Ent[entaddress].Add(entaddress) ) {
                            cout << "HandleDataString: add entity failed" << endl;
                        }
                    }
                    else cout << "HandleDataString: unknown entity referenced, no info to add" << endl;
                }

                //ent flagged to be removed
                if(removeentaddress != -1) {
                    if( atoi( TKV[i].KV[removeentaddress].sValue.c_str() ) == 1 ) lvl.Ent[entaddress].Remove();
                }
            }
        }
        else cout << "HandleDataString: got unknown type \"" <<  TKV[i].Type << "\"" << endl;
    }
}

void LocalClient::ProcessEntityData(char *packetbuffer) {
    if(!lvl.Loaded) return;

    int bufpos = 0;
    uint8_t binarydatatype = 0;
    uint16_t datasize = 0;               //size of the data to process
    uint64_t timestamp = 0;             //SDL ticks used as a way of ordering packets
    uint16_t entcount = 0;              //how many entities data are in this packet
    uint16_t entaddress = 0;
    float yaw = 0;                        //entities rotation
    float x = 0; float y = 0; float z = 0;  //position in the world

    //cout << "size: " << strlen(packetbuffer) << " packetbuffer: " << packetbuffer << endl;

    memcpy(&binarydatatype, packetbuffer + bufpos, sizeof(binarydatatype)); //cout << "type: " << (int)binarydatatype;
    bufpos += sizeof(binarydatatype);
    memcpy(&datasize, packetbuffer + bufpos, sizeof(datasize)); //cout << "type: " << (int)datasize;
    bufpos += sizeof(datasize);
    memcpy(&timestamp, packetbuffer + bufpos, sizeof(timestamp)); //cout << " timestamp: " << (uint32_t)timestamp;
    bufpos += sizeof(timestamp);
    memcpy(&entcount, packetbuffer + bufpos, sizeof(entcount)); //cout << " # of ents: " << (int)entcount << endl;
    bufpos += sizeof(entcount);

    if(timestamp < LastTimestamp)  return; //check to see if this isnt an old packet
    LastTimestamp = timestamp;

    for(int i = 0; i < entcount; i++) {
        if(bufpos < datasize) {    //prevent the buffer from overflowing; 17 bytes in ent data
            memcpy(&entaddress, packetbuffer + bufpos, sizeof(entaddress)); //cout << "ent add: " << entaddress;
            bufpos += sizeof(entaddress);
            memcpy(&yaw, packetbuffer + bufpos, sizeof(yaw)); //cout << " yaw: " << (float)yaw;
            bufpos += sizeof(yaw);
            memcpy(&x, packetbuffer + bufpos, sizeof(x)); //cout << " x " << (float)x;
            bufpos += sizeof(x);
            memcpy(&y, packetbuffer + bufpos, sizeof(y)); //cout << " y: " << (float)y;
            bufpos += sizeof(y);
            memcpy(&z, packetbuffer + bufpos, sizeof(z)); //cout << " z: " << (float)z << endl;
            bufpos += sizeof(z);

            if(lvl.Ent[entaddress].isUsed) {    //check to see if this ent is actually there
                lvl.Ent[entaddress].Yaw = yaw;
                lvl.Ent[entaddress].pos.c.x = x;
                lvl.Ent[entaddress].pos.c.y = y;
                lvl.Ent[entaddress].pos.c.z = z;
            }
        }
        else {
            cout << "LocalClient::ProcessEntityData packet too big ignoring" << endl;
            return;
        }
    }
}

void LocalClient::HandleTextMessage(int messageaddress) {
    string localstring = "";
    string found_arg = "";
    string found_command = "";
    size_t space_position = 0;

    localstring = MessageBuffer[messageaddress].Payload;
    if(localstring == "") {
        cout << "LocalClient::HandleTextMessage: empty string" << endl;
        return;
    }

    //cout << "HandleTextMessage: proccessing string \"" << localstring << "\"" << endl;

    //look for a / at the beginning of the string then pull out the command
    if(localstring.at(0) == '/') {
        space_position = localstring.find(' ');
        if( space_position != string::npos ) {
            //found space pull out substring
            found_command = localstring.substr(1, space_position - 1);
            found_arg = localstring.substr( space_position + 1, localstring.size() );

        } else {
            //pull out substring
            found_command = localstring.substr(1, localstring.size());
        }

        //attempt to execute command if succesful clear the msg from buffer; other wise let it go to the server
        if(ExecuteCommand(found_command, found_arg)) MessageBuffer[messageaddress].Clear();
    }
    else {  //add /txtmsg to command and let it go on to the server to be sent back
        localstring = "/txtmsg " + localstring;    //8
        MessageBuffer[messageaddress].PayloadSize = strlen(localstring.c_str());
        memcpy(&MessageBuffer[messageaddress].Payload + 0, localstring.c_str(), MessageBuffer[messageaddress].PayloadSize);

        //cout << "new payload \"" << MessageBuffer[messageaddress].Payload << "\"" << endl;
    }
}

bool LocalClient::NetServerConnect(string IPAddressString) {
    ENetAddress AddresToConnect;
    ENetEvent event;

    //setup address to connect to
    enet_address_set_host (& AddresToConnect, IPAddressString.c_str());
    AddresToConnect.port = 41414;

    //Initiate the connection, allocating the two channels 0 and 1.
    ServerPeer = enet_host_connect (ClientHost, & AddresToConnect, MAXCHANNELS, 2);

    if (ServerPeer == NULL)   {
        cout << "LocalClient: could not connect to host: no server" << endl;
        return false;
    }

    // Wait up to 1 second for the connection attempt to succeed.
    if (enet_host_service (ClientHost, & event, 1000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)  {
        cout << "LocalClient: connected to host" << endl;
        isConnectedToRemoteServer = true; //the type of networking we are using
        return true;
    } else {
        enet_peer_reset (ServerPeer);

        cout << "LocalClient::NetServerConnect: could not connect to host: no responce" << endl;
        return false;
    }

}

void LocalClient::RemoteDisconnect() {
    //let the client know
    AddTextMessage("lost connection");    //shouldnt this just be a ui msg?

    //reset local world data
    if(lvl.Loaded) {
        EntityAddress = -1;
        DrawWorld = false;
        lvl.Unload();
    }

    //stop checking for network activity
    isConnectedToRemoteServer = false;
    isConnected = false;

    //turn on local server; not accepting remote connections
    if(!hxServer.isRunning) hxServer.Start(0);

    //change the connect button to go online from go offline
    ButtonList[Dropdownmenus[1].Buttons[0]].label = "Go Online";
}

void LocalClient::CheckNetEvents() {
    ENetEvent event;

    /* Wait up to 1 millisecond for an event. */
    while (enet_host_service (ClientHost, & event, 1) > 0)  {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                cout << "LocalClient::CheckNetEvents(): "
                     << "client connected address.host: " << event.peer -> address.host << ":"
                     << event.peer -> address.port << " "
                     << "data: " << event.peer -> data << endl;
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                HandleNewPacket(event);

                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy (event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                RemoteDisconnect();

                /* Reset the peer's client information. */
                event.peer -> data = NULL;
                break;

            case ENET_EVENT_TYPE_NONE:
               cout     << "LocalClient::CheckNetEvents(): "
                        << "Mysterious event type none: "
                        << "from ip:" <<   event.peer -> address.host << ":" << event.peer -> address.port << " "
                        << "data: " << event.peer -> data << endl;
                break;
        }
    }
}

void LocalClient::HandleNewPacket(ENetEvent localevent) {
    bool baddata = false;
    uint16_t textmessagesize = 0;
    uint8_t messagetype = 0;
    int bufpos = 0; //where we will read from in the packetbuffer
    char packetbuffer[1080];    //1kB of buffer
    char textmessagedata[MAXMSGLENTH];

    //clear arrays
    memset(packetbuffer, '\0', sizeof(packetbuffer));
    memset(textmessagedata, '\0', sizeof(textmessagedata));

    memmove(packetbuffer, localevent.packet->data, localevent.packet->dataLength);
    //cout << "client: processing \"" << packetbuffer << "\"" << endl;

    //seperate and process the messages
    while(packetbuffer[bufpos] != '\0') {
        //copy data into buffer
        memmove(&messagetype, packetbuffer + bufpos, sizeof(messagetype));
        bufpos += sizeof(messagetype);
        //cout << "client: processing packet type: " << (int)messagetype;

        memmove(&textmessagesize, packetbuffer + bufpos, sizeof(textmessagesize));
        bufpos += sizeof(textmessagesize);
        //cout << " size: " << (int)textmessagesize;

        memmove(textmessagedata, packetbuffer + bufpos, textmessagesize);
        bufpos += textmessagesize;
        //cout << " data: \"" << textmessagedata << "\"" << endl;

        //update buffer pos
        //bufpos = bufpos + (textmessagesize + 3);

        //examine the data
        //JAE 3/23/23: a uint8_t like messagetype is by definition 0 to 255
        //if(messagetype <= 0 || messagetype > 255) baddata = true;

        //put new message in our message buffer
        if(!baddata) {
            if(messagetype == 1) {
                string msgstring = textmessagedata;
                AddTextMessage(msgstring);
            }
            else if(messagetype == 2) {
                LastKeepAliveFromServer = SDL_GetTicks();
            }
            else if (messagetype == 3) {
                ProcessEntityData(packetbuffer);
            }
            else cout << "client: received and ignored type: " << (int)messagetype << endl;
        }

        //clear buffers
        textmessagesize = 0;
        messagetype = 0;
        // JAE 3/23/23 - using memset to clear should be a whole buncha fasters!
        //for( int tempi = 0 ; tempi < MAXMSGLENTH; tempi++) textmessagedata[tempi] = '\0';
        memset(textmessagedata, '\0', sizeof(textmessagedata));
    }
}

void LocalClient::SendMessages() {
    int msgcount = 0;
    int bufpos = 0; //where we will wright to in the packetbuffer
    char packetbuffer[1023];    //1kB of buffer
    memset(packetbuffer,  '\0', sizeof(packetbuffer));
    char textmessagedata[MAXMSGLENTH];
    uint16_t textmessagesize;
    uint8_t messagetype;

    for(int j = 0; j < MAXMSGS; j++) {
        //move data from client list to packet buffer
        if(MessageBuffer[j].Type != 0) {
            memset(textmessagedata,  '\0', sizeof(textmessagedata));
            textmessagesize = 0;
            messagetype = 0;

            messagetype = MessageBuffer[j].Type; //message type
            textmessagesize = MessageBuffer[j].PayloadSize;  //message size

            //message data
            if(MessageBuffer[j].PayloadSize <= MAXMSGLENTH && MessageBuffer[j].PayloadSize > 0) {   //check for valid size
                memcpy(&textmessagedata, &MessageBuffer[j].Payload, MessageBuffer[j].PayloadSize);
            }
            else {
                textmessagesize = 0;  //bad length
                cout << "ReadCommands: bad message length" << endl; //should return here from another function
            }

            //move command data to the servers buffer in to the packet buffer; 254 so we know we dont over wright the null char
            if(textmessagesize != 0 && 1022 > (textmessagesize+3) ) {   //check for valid size and that we arnt going to over wright the buffer
                cout << "client: sending packet type:" << (int)messagetype;
                memmove(packetbuffer + bufpos, &messagetype, sizeof(messagetype));   //copy type into buffer
                bufpos += sizeof(messagetype);

                cout << " size: " << textmessagesize;
                memmove(packetbuffer + bufpos, &textmessagesize, sizeof(textmessagesize));   //copy data size into buffer
                bufpos += sizeof(textmessagesize);

                cout << " data: \"" << textmessagedata << "\"" << endl;
                memmove(packetbuffer + bufpos, textmessagedata, textmessagesize);
                bufpos = bufpos + textmessagesize;

                msgcount++;
            }

            //message sent therefor no longer need
            MessageBuffer[j].Clear();
        }
    }

    //send packet if we have anything to send
    if(msgcount > 0) {
        //the +1 in the length will include the null char to terminate the string
        //ENetPacket * packet = enet_packet_create (packetbuffer, strlen(packetbuffer)+1, ENET_PACKET_FLAG_RELIABLE);
        ENetPacket * packet = enet_packet_create (packetbuffer, bufpos+2, ENET_PACKET_FLAG_RELIABLE);
        if(packet != NULL) {
            int goodsend = -1;
            goodsend = enet_peer_send(ServerPeer, 0, packet);
            if(goodsend < 0) cout << "client: couldnt send a packet to server" << endl;
                enet_host_flush (ClientHost); //do we need to send everytime?
            }
            else cout << "bad packet construction: data not sent" << endl;
    }

    //reset the position of message writes
    //msgcount = 0;
}
