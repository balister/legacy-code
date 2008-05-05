
/*******************************************************************************
 
Copyright 2006, Virginia Polytechnic Institute and State University
 
This file is part of the OSSIE  waveform loader.
 
OSSIE Waveform Loader is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
 
OSSIE Sample Waveform is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with OSSIE Waveform loader; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
 
*******************************************************************************/

#include <iostream>

#include "DASParser.hh"


DASParser::DASParser(CF::File_ptr das_file) : num_elements(0)
{
    das_var = new CF::DeviceAssignmentSequence;
    
    std::cout << "DASParser Entering constructor for file " << das_file->fileName() << std::endl;

    unsigned long fileSize = das_file->sizeOf();
    CF::OctetSequence_var fileData;

    das_file->read(fileData, fileSize);

    das_file->close();

    unsigned char *fileBuffer = fileData->get_buffer();

    XMLDoc.Parse((const char *)fileBuffer);

    root = XMLDoc.FirstChild("deploymentenforcement");
    if (!root) {
        std::cerr << "ERROR: Not a deployment enforcement file" << std::endl;
        throw 0;
    }

    TiXmlHandle docHandle(root);

    // Handle device assignement sequence file
    TiXmlElement *elem = docHandle.FirstChild("deviceassignmentsequence").FirstChild("deviceassignmenttype").Element();
    if (!root) {
        std::cerr << "ERROR: DASParser could not find <deviceassignmentsequence> node" << std::endl;
        throw 0;
    }
    
    std::cout << "c_wavLoader: DAS parse about to parse deviceassementtypes" << std::endl;

    for (; elem; elem = elem->NextSiblingElement()) {
        std::cout << "DASParser: found deviceassignmenttype, adding element" << std::endl;

	add_element();

        // try to find <componentid> node
        TiXmlElement *cid = elem->FirstChildElement("componentid");
        std::cout << "Found componentid: " << cid->GetText();
        das_var[num_elements - 1].componentId = CORBA::string_dup(cid->GetText());
        
        // try to find <assigndeviceid> node
        TiXmlElement *adid = elem->FirstChildElement("assigndeviceid");
        std::cout << "Found assigndeviceid: " << adid->GetText();
        das_var[num_elements - 1].assignedDeviceId = CORBA::string_dup(adid->GetText());
    }

    std::cout << "DASParser: finished parsing file!" << std::endl;
    
}
