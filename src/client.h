#pragma once

#include <string>
#include <cstdint>

#include "SDL2/SDL.h"
#include "enet/enet.h"

#include "level.h"

// Represents a single client in the local client list.
class LocalClientList {
public:
    std::string Name;
    int LastUpdate;

    LocalClientList() { Clear(); }
    void Clear() {
        Name = "unknown";
        LastUpdate = -1;
    }
};

// Main client class for handling network and game state.
class LocalClient {
    private:
        

    public:
        level lvl;  // Client level data

        // Settings
        std::string Username;

        // States
        bool MSGmode = false;                  // Typing a message or command
        bool isConnected = false;              // Connected to a server
        bool isConnectedToRemoteServer = false;
        bool DrawWorld = false;
        KeyState Keys;

        // UI and display
        SDL_Window *window;
        SDL_GLContext glContext;
        unsigned int window_width = 640;
        unsigned int window_height = 480;
        unsigned int window_fullscreen = 0;
        uint64_t frameTime = 0;

        // Network and data buffers
        signed int EntityAddress = -1;
        unsigned int KeyFlags = 0;
        ENetPeer* ServerPeer = nullptr;
        ENetHost* ClientHost = nullptr;
        std::string MSGDrawBuffer0, MSGDrawBuffer1, MSGDrawBuffer2, MSGDrawBuffer3;
        std::string MSGSendBuffer;
        MSGClass MessageBuffer[MAXMSGS];
        LocalClientList Clients[MAXCLIENTS];
        uint64_t LastTimestamp = 0;

        // Functions
        void Update();
        void ProcessMessages();
        bool NetServerConnect(const std::string& IPAddressString);
        void RemoteDisconnect();
        void CheckNetEvents();
        void HandleDataString(const std::string& worldstr);
        void ProcessEntityData(char* packetbuffer);
        void HandleTextMessage(int messageaddress);
        bool ExecuteCommand(const std::string& command, const std::string& arg);
        void BroadcastTimer();
        void AddTextMessage(const std::string& newtextmessage);
        void SendKeyState();
        void BroadcastKeyState();
        void SendClientInfo();
        void SendKeepAlive();
        void SendMessages();
        void HandleNewPacket(ENetEvent localevent);

        LocalClient() = default;
};
