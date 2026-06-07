#include "tinerxml.h"

#include <iostream>

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
        std::cout << "couldnt find file " << _xmlfile << std::endl;
        delete xmlDoc;
        return 0;
    }
    TiXmlElement *xGame = xmlDoc->FirstChildElement("root");
    if (!xGame)
    {
        std::cout << "couldnt find root(must be called root) element in " << _xmlfile << std::endl;
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
        std::cout << "error! xml element " << name << " didnt load!" << std::endl;
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
