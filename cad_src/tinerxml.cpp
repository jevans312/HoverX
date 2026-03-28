#include "tinerxml.h"

#include <iostream>
using namespace std;

//tinyxml2::XMLDocument     *xmlDoc;

void xmlfile::endxml()
{
    delete xmlDoc;
}

tinyxml2::XMLElement* xmlfile::getxmlfirstelement(char* xmlfile)
{
    xmlDoc = new tinyxml2::XMLDocument();
    if (xmlDoc->LoadFile(xmlfile) != tinyxml2::XML_SUCCESS)
    {
    	cout << "couldnt find file " << xmlfile << endl;
        delete xmlDoc;
        return 0;
    }
    tinyxml2::XMLElement *xGame = xmlDoc->FirstChildElement("root");
    if (!xGame)
    {
    	cout << "couldnt find root(must be called root) element in " << xmlfile << endl;
        endxml();
        return 0;
    }
    return xGame;
}

tinyxml2::XMLElement* xmlfile::getelement(tinyxml2::XMLElement* from, char* name)
{
    tinyxml2::XMLElement *element = from->FirstChildElement(name);
    if (!element)
    {
        cout << "error! xml element " << name << " didnt load!" << endl;
        endxml();
        return 0;
    }
    return element;

}

int xmlfile::countchildren(tinyxml2::XMLElement* of)
{
    int numchildren = 0;
    tinyxml2::XMLElement *child = of->FirstChildElement();
    while( child )
    {
        numchildren++;
        child = child->NextSiblingElement();
    }
    return numchildren;
}
