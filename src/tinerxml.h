#ifndef tinerxml_h
#define tinerxml_h
#include "tinyxml.h"
/*
void endxml();
TiXmlElement* getxmlfirstelement(char* xmlfile);
TiXmlElement* getelement(TiXmlElement* from, char* name);
int countchildren(TiXmlElement* of);
*/

class xmlfile {
    public:
        TiXmlDocument*  xmlDoc;
        TiXmlElement*   getxmlfirstelement(char* xmlfile);
        TiXmlElement*   getelement(TiXmlElement* from, char* name);

        int countchildren(TiXmlElement* of);
        void endxml();
};


#endif
