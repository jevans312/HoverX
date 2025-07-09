#pragma once

#include "tinyxml.h"

class xmlfile {
public:
    TiXmlDocument* xmlDoc = nullptr;

    TiXmlElement* getxmlfirstelement(const char* xmlfile);
    TiXmlElement* getelement(TiXmlElement* from, const char* name);
    int countchildren(TiXmlElement* of);
    void endxml();
};
