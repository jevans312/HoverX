#pragma once

#include "tinyxml2.h"

class xmlfile {
public:
    tinyxml2::XMLDocument* xmlDoc = nullptr;

    tinyxml2::XMLElement* getxmlfirstelement(const char* xmlfile);
    tinyxml2::XMLElement* getelement(tinyxml2::XMLElement* from, const char* name);
    int countchildren(tinyxml2::XMLElement* of);
    void endxml();
};
