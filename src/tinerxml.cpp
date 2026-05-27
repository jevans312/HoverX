#include "tinerxml.h"

#include <iostream>
using namespace std;

//TiXmlDocument     *xmlDoc;

void xmlfile::endxml()
{
    xmlDoc->Clear();
    delete xmlDoc;
}

TiXmlElement* xmlfile::getxmlfirstelement(const char* _xmlfile)
{
    xmlDoc = new TiXmlDocument(_xmlfile);
    if (!xmlDoc->LoadFile())
    {
    	cout << "couldnt find file " << _xmlfile << endl;
        delete xmlDoc;
        return 0;
    }
    TiXmlElement *xGame = xmlDoc->FirstChildElement("root");
    if (!xGame)
    {
    	cout << "couldnt find root(must be called root) element in " << _xmlfile << endl;
        endxml();
        return 0;
    }
    return xGame;
}

TiXmlElement* xmlfile::getelement(TiXmlElement* from, const char* name)
{
    TiXmlElement *element = from->FirstChildElement(name);
    if (!element)
    {
        cout << "error! xml element " << name << " didnt load!" << endl;
        endxml();
        return 0;
    }
    return element;

}

int xmlfile::countchildren(TiXmlElement* of)
{
    int numchildren = 0;
    TiXmlElement *child = of->FirstChildElement();
    while( child )
    {
        numchildren++;
        child = child->NextSiblingElement();
    }
    return numchildren;
}
