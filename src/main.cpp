#include <signal.h>

#include "main.h"
#include "sdl.h"
#include "renderer.h"
#include "server.h"
#include "client.h"

#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

using namespace std;

//level mylvl; //clients level
ServerClass hxServer;
LocalClient LC; //local client data

//game states
//TODO: Move is stuff to client/server and MSGmode to UI
bool isConnected = false;   //net flag
bool isHost = false;        //net flag
bool MSGmode = false;       //game state for typing a message or command
bool done;                  //main loop control

//!Warning this gives you the time difference since you lasted call this function!
uint64_t static TimeofLastUpdate = 0;
int64_t getDeltaTime() {
    int64_t returnvalue = 0;
    uint64_t timenow = SDL_GetTicks64();

    //get the time between the last tick and now
    returnvalue = timenow - TimeofLastUpdate;

    //set the time of last update to now as now is when we will tick
    TimeofLastUpdate = timenow;

    return returnvalue;
}

uint64_t static tickcount = 0;
void Tick() {
    //Update ever 1 second
    if(tickcount == 1000/TICK_RATE) {
        LC.frameTime = LC.afterDrawTime - LC.beforeDrawTime;
        cout << "Frame time: " << LC.frameTime << "ms" << '\n';
        tickcount = 0;
    }
    tickcount++;

    LC.Update();
    hxServer.Update();
}

uint64_t static accumulator = 0;
void Ticker() {
     //accumulate the time since the last tick
    accumulator += getDeltaTime();

    //tick how ever many times we missed
    bool sleepThisTime = true;
    while(accumulator >= TICK_RATE) {
        Tick();
        accumulator = accumulator - TICK_RATE;
        sleepThisTime = false; //we ticked so we want to draw asap
    }
    
    //JAE: 7/4/25 - Fixed sleep time between updates for maximum precision
    if(sleepThisTime) {
        SDL_Delay(1);
    }
}

//signal handler for POSIX signals
//TODO: Handle pause and resume
void signalHandler(int signum) {
    if(signum == 1 || signum == 2 || signum == 9 || signum == 10 || signum == 15) {
        cout << "HoverX: Received POSIX terminate signal type #" << signum << '\n';
    } else if(signum >= 3 && signum <= 8) {
        cout << "HoverX: Received POSIX Dump signal type #" << signum << '\n';
    } else if(signum == 18) { //Continue
        cout << "HoverX: Received POSIX Continue signal type #" << signum << '\n';
    } else if(signum == 19) { //Pause
        cout << "HoverX: Received POSIX Stop signal type #" << signum << '\n';
    } else {
        cout << "HoverX: Received unkonwn POSIX signal type #" << '\n';
    }

    done = 1; // Exit the main loop
}

int main(int argc, char *argv[]) {
    //Listen for Ctrl+C to exit gracefully
    signal(SIGINT, signalHandler);

    //get commandline options
    bool consoleArgStr = false;
    for(int i = 1; i < argc; i++) {
        string argstr = argv[i];

        if(argstr == "console") {
            consoleArgStr = true;
        }
    }

    //Setup client and create if not in cosole mode
    LC.InitLocalClient(consoleArgStr);

    //turn the server on; allow remote connections if in console mode
    hxServer.Start(consoleArgStr);

    done = 0;
    while ( ! done ) {
        //JAE 7/5/25 - Move input and draw to client update functions
        GetSDLInput();
        Ticker(); //Update the client and server
        DrawScene();
    }

    hxServer.Stop();
    SDL_Quit();
    return 0;
}

//helper functions
string IntToIpAddress(unsigned int ip) {
    char ips[20];
    unsigned char bytes[4];
    bytes[0] = ip & 0xFF;
    bytes[1] = (ip >> 8) & 0xFF;
    bytes[2] = (ip >> 16) & 0xFF;
    bytes[3] = (ip >> 24) & 0xFF;
    sprintf(ips, "%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0]);
    return ips;
}

string BoolToStr(const bool b) {
    if(b) {
        return "True";
    } else {
        return "False";
    }
}

void skipLine(istream& is)  {
    char next;
	is >> std::noskipws;
    while( (is >> next) && ('\n' != next) );
}

//	skip a comment line
bool skipCommentLine(istream& is)   {
	char next;
	while( is >> std::skipws >> next )
    {
		is.putback(next);
		if ('#' == next)
			skipLine(is);
		else
			return true;
    }
    return false;
}

bool FileExists( const char* FileName ) {    //returns true if file exist
    FILE* fp = NULL;

    fp = fopen( FileName, "rb" );
    if( fp != NULL )    {
        fclose( fp );
        return true;
    }

    return false;
}

int FindKeyByName(KeyValue *kv, string keytofind) {    //return the location of the data we are looking for or -1 on fail
    for(int i = 0; i < 31; i++) {
        if(kv[i].Key== keytofind) {
            return i;
        }
    }

    return -1;
}

void ParseTKV(string worldstr, TypeKeyValue *TKV) {     //turn data string into type key and value
    size_t slash1 = 0;
    size_t slash2 = 0;
    size_t spacepos = 0;
    size_t equalpos = 0;
    string datapoint = "";
    bool endofkv = false;
    bool endoftype = false;
    int i = 0;
    int j = 0;

    endoftype = false;
    while ( !endoftype && i < 63) {
        //find the '/'
        slash1 = worldstr.find('/', slash1);
        slash2 = worldstr.find('/', slash1 + 1);
        //cout << "slash1: " << slash1 << " slash2: " << slash2 << '\n';

        //use the end of the file as the end if there are no more '/'
        if(slash2 == string::npos) {
            slash2 = worldstr.length();
            endoftype = true;
        }

        if(slash1 != string::npos && slash2 != string::npos) {
            //seperate the type class from the rest of the string
            datapoint = worldstr.substr(slash1 + 1, slash2 - 1);
            //cout << "datapoint: " << datapoint << '\n';

            //remove datapoin from the world string so it doesnt get proccessed again
            worldstr = worldstr.substr(slash2, worldstr.length());

            //get the type
            spacepos = datapoint.find(' ');
            TKV[i].Type = datapoint.substr(0, spacepos);
            //cout << "type: " << TKV[i].Type << "->";

            //remove type from datapoint
            datapoint = datapoint.substr(spacepos, datapoint.length());

            j = 0;
            endofkv = false;
            while( !endofkv && j < 31 ) {
                //cout << '\n' << "what to process: \"" << datapoint << "\"" << '\n';

                //remove space from begining of string; potentail bug only removes 1 leading ' '
                if(datapoint.length() != 0) {
                    if(datapoint.at(0) == ' ') datapoint.erase(0, 1);
                }


                if(datapoint.length() != 0) {
                    //find the next space
                    size_t strend = datapoint.find(' ');

                    //use the end of the file as the end if there is no space
                    if(strend == string::npos) strend = datapoint.length();

                    //find a '=' between the beginning and the next space/end of string; if not no reason to continue
                    equalpos = datapoint.find('=');

                    if(equalpos != string::npos) {
                        string keyvaluestr = datapoint.substr(0, strend);
                        //cout << "keyvaluestr: \"" << keyvaluestr << "\"" << '\n';

                        datapoint = datapoint.substr(strend, datapoint.length());
                        //cout << "new datapoint: \"" << datapoint << "\"" << '\n';

                        //now pull out the left and right side of the '='
                        TKV[i].KV[j].Key = keyvaluestr.substr(0, equalpos);
                        TKV[i].KV[j].sValue = keyvaluestr.substr(equalpos+1, strend);

                        //cout << "\"" << TKV[i].KV[j].Key << "\"" << "=" << "\"" << TKV[i].KV[j].sValue << "\"" << '\n';
                    }
                }
                else endofkv = true;

                j++;
            }
            if(j >= 31) cout << "HandleWorldString: more than 32 key/values in type" << '\n';
        }

        i++;
    }
    if(i >= 63) cout << "HandleWorldString: more than 64 types in string" << '\n';

}
