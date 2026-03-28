//#include "tinybind.h"
#include "tinyxml2.h"
/*
void endxml();

TiXmlElement* getxmlfirstelement(char* xmlfile);

TiXmlElement* getelement(TiXmlElement* from, char* name);

int countchildren(TiXmlElement* of);
*/
class xmlfile
{
    public:
//xmlfile
tinyxml2::XMLDocument     *xmlDoc;

tinyxml2::XMLElement* getxmlfirstelement(char* xmlfile);

tinyxml2::XMLElement* getelement(tinyxml2::XMLElement* from, char* name);

int countchildren(tinyxml2::XMLElement* of);

void endxml();
};
