/****************************************************************************

Copyright 2005, 2006, 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE GPP.

OSSIE GPP is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE GPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE GPP; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/

#include <iostream>
#include <string>

#include "ossie/debug.h"
#include "ossie/ossieSupport.h"

#include "GPP.h"

int main(int argc, char *argv[])

{
    ossieDebugLevel = 3;

    ossieSupport::ORB* orbsup = new ossieSupport::ORB();

    char *id, *profile, *label;

    if (argc != 4) {
        std::cout << argv[0] << " : <identifier> <name> <SPD Profile>" << std::endl;
        exit(-1);
    }

    id = argv[1];
    label = argv[2];
    profile = argv[3];

    DEBUG(1, GPP, "Identifier = " << id << "Label = " << label << " Profile = " << profile)


    // Create Executable device servant and object reference
    GPP_i* GPP_servant;
    CF::ExecutableDevice_var GPP_var;

    GPP_servant = new GPP_i(id, label, profile);
    GPP_var = GPP_servant->_this();

    // Add the object to the Naming Service
    std::string objName = "DomainName1/";
    objName += label;

    orbsup->bind_object_to_name((CORBA::Object_ptr) GPP_var, objName.c_str());

    // Start handling CORBA requests
    orbsup->orb->run();
}
