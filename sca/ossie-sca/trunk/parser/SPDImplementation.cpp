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
on the architecture of the CRCs SCA Reference Implementation (SCARI)
see: http://www.crc.ca/en/html/rmsc/home/sdr/projects/scari

*********************************************************************************/
#include <iostream>


#include "ossie/SPDImplementation.h"
#include "ossie/SPDParser.h"
#include "ossie/debug.h"

#include "tinyxml.h"

SPDImplementation::SPDImplementation(TiXmlElement *elem)
{

    parseElement(elem);
}


SPDImplementation::~SPDImplementation ()
{

	for (unsigned int i=0; i<usesDevice.size(); i++)
	{
		delete usesDevice[i];
	}

}

void SPDImplementation::parseElement(TiXmlElement *elem)
{
    parseID (elem);
    parsePRFRef (elem);
    parseCode (elem);
    parseCompiler (elem);
    parsePrgLanguage (elem);
    parseHumanLanguage (elem);
    parseRuntime (elem);
    parseOperatingSystems (elem);
    parseProcessors (elem);
#if 0
    parseSoftwareDependencies (elem);
    parsePropertyDependencies (elem);
#endif
    parseUsesDevices (elem);

}


void SPDImplementation::parseID (TiXmlElement *elem)
{
    DEBUG(4, SPDImplementation, "In parseID.");

    implementationID = elem->Attribute("id");
}


void SPDImplementation::parsePRFRef(TiXmlElement *elem)
{
    DEBUG(4, SPDImplementation, "In parsePRFRef.");

    TiXmlElement *prf = elem->FirstChildElement("propertyfile");

    if (prf) {
	TiXmlElement *local = elem->FirstChildElement("localfile");
	PRFFile = local->GetText();
    }
}


void SPDImplementation::parseCode(TiXmlElement *elem)
{
    DEBUG(4, SPDImplementation, "In parse Code.");

    TiXmlElement *code = elem->FirstChildElement("code");
    setCodeType (code->Attribute("type"));

    TiXmlElement *local = code->FirstChildElement("localfile");
    codeFile = local->Attribute("name");

    TiXmlElement *entry = elem->FirstChildElement("entrypoint");
    if (entry)
	entryPoint = entry->GetText();

    TiXmlElement *stack= elem->FirstChildElement("stacksize");
    if (stack)
	stackSize = entry->GetText();

    TiXmlElement *prio = elem->FirstChildElement("priority");
    if (prio)
	priority = entry->GetText();
}


void SPDImplementation::setCodeType(const char *_ct)
{
    DEBUG(4, SPDImplementation, "In setCodeType.");

    std::string str = _ct;

    if (str == "KernelModule")
	codeType = CF::LoadableDevice::KERNEL_MODULE;
    else if (str == "SharedLibrary")
	codeType = CF::LoadableDevice::SHARED_LIBRARY;
    else if (str == "Executable")
	codeType = CF::LoadableDevice::EXECUTABLE;
    else if (str == "Driver")
	codeType = CF::LoadableDevice::DRIVER;
    else {
	DEBUG(1, SPDImplementation, "Bad code type " << str << " found in xml file.");
	    //throw new SCA.CF.InvalidProfile(msg);
    }
}


void SPDImplementation::parseCompiler(TiXmlElement *elem)
{
    DEBUG(4, SPDImplementation, "In parseCompiler.");

    TiXmlElement *compiler = elem->FirstChildElement("compiler");
    
    if (compiler) {
	compilerName = compiler->Attribute("name");;

	const char *ver = compiler->Attribute("version");
	if (ver)
	    compilerVersion = ver;
    }
}


void SPDImplementation::parsePrgLanguage(TiXmlElement *elem)
{
    DEBUG(4, SPDImplementation, "In parsePrgLanguage.");

    TiXmlElement *lang = elem->FirstChildElement("programminglanguage");

    if (lang) {
	prgLanguageName = lang->Attribute("name");

	const char *ver = lang->Attribute("version");

	if (ver)
	    prgLanguageVersion = ver;
    }
}


void SPDImplementation::parseHumanLanguage(TiXmlElement *elem)
{
    DEBUG(4, SPDImplementation, "In parseHumanLanguage.");

    TiXmlElement *lang = elem->FirstChildElement("humanlanguage");

    if (lang)
	prgLanguageName = lang->Attribute("name");
}


void SPDImplementation::parseRuntime(TiXmlElement *elem)
{
    DEBUG(4, SPDImplementation, "In parseRunTime.");

    TiXmlElement *run = elem->FirstChildElement("runtime");
    
    if (run) {
	runtimeName = run->Attribute("name");
	
	const char *ver = run->Attribute("version");
	
	if (ver)
	    runtimeVersion = ver;
    }
}


void SPDImplementation::parseOperatingSystems (TiXmlElement *elem)
{
    DEBUG(4, SPDImplementation, "In parseOperatingSystems.");

    TiXmlElement *osn = elem->FirstChildElement("os");

    if (osn) {
	const char *name = osn->Attribute("name");
	os.setOSName(name);

	const char *ver = osn->Attribute("version");
	if (ver)
	    os.setOSVersion(ver);
    }
}

void SPDImplementation::parseProcessors (TiXmlElement *elem)
{
    DEBUG(4, SPDImplementation, "In parseProcessors.");

    TiXmlElement *proc = elem->FirstChildElement("processor");

    for (; proc; proc = proc->NextSiblingElement("processor")) {
	const char *procName = proc->Attribute("name");
	processorsName.push_back (procName);
    }
}

#if 0
void SPDImplementation::parseSoftwareDependencies(TiXmlElement *elem)
{
    DEBUG(4, SPDImplementation, "In parseSoftwareDependencies.");

	tmpXMLStr = XMLString::transcode("softpkgref");
	DOMNodeList* nodeList =	_elem->getElementsByTagName(tmpXMLStr);
	sweetd(tmpXMLStr);

	DOMNodeList* _tmpNodeList;

	unsigned int lenofSoftDep = nodeList->getLength();

	if (lenofSoftDep > 0)
	{
	    std::cout << "Add code to parse SW dependencies" << std::endl;
///\todo Rewrite the parser SW dependencies code
#if 0
		DOMElement *_tmpElement;
		softDep = new char *[lenofSoftDep];


		{
		for (int i = 0; i < lenofSoftDep; i++) {
			_tmpElement = (DOMElement *) nodeList->item (i);
			tmpXMLStr = XMLString::transcode("localfile");
			_tmpNodeList = _tmpElement->getElementsByTagName(tmpXMLStr);
			sweetd(tmpXMLStr);
			_tmpElement = (DOMElement *) _tmpNodeList->item (0);

			tmpXMLStr = XMLString::transcode("name");

			// Because of this bug -TT
			softDep[SOFTWARE_SPD_NAME] = XMLString::transcode(_tmpElement->getAttribute(tmpXMLStr));
			sweetd(tmpXMLStr);

//                      _tmpElement = (DOMElement*) _tmpElement->getNextSibling();
			_tmpElement = (DOMElement *) _tmpNodeList->item (1);

			if (_tmpElement != NULL)
			{
				tmpXMLStr = XMLString::transcode("refid");
				const XMLCh *tmp2 = _tmpElement->getAttribute (XMLString::transcode ("refid"));
				softDep[SOFTWARE_IMPL_REF] = XMLString::transcode (tmp2);
			}
			else softDep[SOFTWARE_IMPL_REF] = NULL;
		} // for
		}

#endif
	}
}


void SPDImplementation::parsePropertyDependencies(DOMElement * _elem)
{
    DEBUG(4, SPDImplementation, "In parsePropertyDependencies.");

	std::vector <DOMNode*> _list;
	tmpXMLStr = XMLString::transcode("dependency");
	DOMNodeList *nodeList =	_elem->getElementsByTagName(tmpXMLStr);
	sweetd(tmpXMLStr);

	unsigned int len;
	DOMElement *_tmpElement;

	for (unsigned int i = 0; i < nodeList->getLength (); i++)
	{
		tmpXMLStr = XMLString::transcode("propertyref");
		DOMNodeList *childNodeList =
		((DOMElement *) nodeList->item (i))->getElementsByTagName(tmpXMLStr);
		sweetd(tmpXMLStr);
		len = childNodeList->getLength();

		if (len > 0)
		{
			for (unsigned int i = 0; i < len; i++)
			_list.push_back (childNodeList->item (i));
		}
	}

	len = _list.size();

	const XMLCh *_tmp;
	const XMLCh *_tmp2;
	char *_id;
	char *_val;

	if (len > 0)
	{
		propDep.length (len);
		for (unsigned int i = 0; i < len; i++)
		{
			_tmpElement = (DOMElement *) _list[i];

			tmpXMLStr = XMLString::transcode("refid");
			_tmp = _tmpElement->getAttribute(tmpXMLStr);
			sweetd(tmpXMLStr);
			_id = XMLString::transcode (_tmp);

			propDep[i].id = CORBA::string_dup(_id);

			tmpXMLStr = XMLString::transcode("value");
			_tmp2 = _tmpElement->getAttribute(tmpXMLStr);
			_val = XMLString::transcode(_tmp2);

			propDep[i].value <<= _val;
		}
	}
}
#endif

void SPDImplementation::parseUsesDevices(TiXmlElement *elem)
{
    DEBUG(4, SPDImplementation, "In parseUsesDevices.");

    TiXmlElement *uses = elem->FirstChildElement("usesdevice");

    for (; uses; uses = uses->NextSiblingElement("usesdevice")) {
	const char *id = uses->Attribute("id");
	const char *type = uses->Attribute("type");
	
	CF::Properties props;
	unsigned int i(0);

	TiXmlElement *prop = uses->FirstChildElement("propertyref");
	for (; prop; prop = prop->NextSiblingElement("propertyref")) {
	    const char *refid = prop->Attribute("refid");
	    const char *value = prop->Attribute("value");
	    props.length(i+1);
	    props[i].id = CORBA::string_dup (refid);
	    props[i].value <<= value;
	    i++;
	}
	
	usesDevice.push_back(new SPDUsesDevice (id, type, props));
    }
}


const char* SPDImplementation::getID()
{
	return implementationID.c_str();
}


const char* SPDImplementation::getPRFFile()
{
	return PRFFile.c_str();
}


const char* SPDImplementation::getCodeFile()
{
	return codeFile.c_str();
}


const char* SPDImplementation::getEntryPoint()
{
	return entryPoint.c_str();
}


const char* SPDImplementation::getCompilerName()
{
	return compilerName.c_str();
}


const char* SPDImplementation::getCompilerVersion()
{
	return compilerVersion.c_str();
}


const char* SPDImplementation::getPrgLanguageName()
{
	return prgLanguageName.c_str();
}


const char* SPDImplementation::getPrgLanguageVersion()
{
	return prgLanguageVersion.c_str();
}


const char* SPDImplementation::getHumanLanguageName()
{
	return humanLanguageName.c_str();
}


const char* SPDImplementation::getRuntimeName()
{
	return runtimeName.c_str();
}


const char* SPDImplementation::getRuntimeVersion()
{
	return runtimeVersion.c_str();
}


OSAttributes  SPDImplementation::getOperatingSystem()
{
	return os;
}


char ** SPDImplementation::getSoftwareDependencies()
{
	return softDep;
}


std::vector <std::string>
SPDImplementation::getProcessors() const
{
	return processorsName;
}


std::vector <SPDUsesDevice*>* SPDImplementation::getUsesDevices()
{
	return &usesDevice;
}


CF::Properties SPDImplementation::getPropertyDependencies() const
{
	return propDep;
}


CF::LoadableDevice::LoadType SPDImplementation::getCodeType () const
{
	return codeType;
}
