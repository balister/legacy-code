/*******************************************************************************

Copyright 2004, 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE Parser.

OSSIE Parser is free software; you can redistribute it and/or modify
it under the terms of the Lesser GNU General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

OSSIE Parser is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Lesser GNU General Public License for more details.

You should have received a copy of the Lesser GNU General Public License
along with OSSIE Parser; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Even though all code is original, the architecture of the OSSIE Parser is based
on the architecture of the CRC's SCA Reference Implementation (SCARI)
see: http://www.crc.ca/en/html/rmsc/home/sdr/projects/scari

*********************************************************************************/

#include "ossie/SPDAuthor.h"

SPDAuthor::SPDAuthor(TiXmlElement *elem)
{
    parseElement(elem);
}

#if 0 ///\todo Delete this code
SPDAuthor::SPDAuthor():
authorElement(NULL),
authorCompany(NULL),
authorWebPage(NULL),
authorName(NULL)
{}


SPDAuthor::SPDAuthor (const SPDAuthor & _spda):
authorElement(_spda.authorElement),
authorCompany(NULL),
authorWebPage(NULL),
authorName(NULL)
{
	this->parseElement();
}
#endif

SPDAuthor::~SPDAuthor()
{
}


const char* SPDAuthor::getAuthorName()
{
    return authorName.c_str();
}


const char* SPDAuthor::getAuthorCompany()
{
    return authorCompany.c_str();
}


const char* SPDAuthor::getAuthorWebPage()
{
    return authorWebPage.c_str();
}


void SPDAuthor::parseElement(TiXmlElement *elem)
{
    parseSoftPkgAuthor(elem);
}


void SPDAuthor::parseSoftPkgAuthor (TiXmlElement *elem)
{
    TiXmlElement *node = elem->FirstChildElement();
    
    for (; node; node = node->NextSiblingElement()) {
	std::string eName = node->ValueStr();

	if (eName == "name")
	    authorName = node->GetText();
	else if (eName == "company")
	    authorCompany = node->GetText();
	else if (eName == "webpage")
	    authorWebPage = node->GetText();
    }
}
