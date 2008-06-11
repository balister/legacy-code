/****************************************************************************

Copyright 2005,2006 Virginia Polytechnic Institute and State University

This file is part of the OSSIE USRP Device.

OSSIE USRP Device is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE USRP Device is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE USRP Device; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


****************************************************************************/

#include <iostream>

#include <sched.h>

#include "ossie/ossieSupport.h"
#include "ossie/debug.h"

#include "USRP.h"


int main(int argc, char* argv[])

{

    if (argc != 4) {
        std::cerr << argv[0] << " <identifier> <usage name> <software profile>" << std::endl;
        exit (-1);
    }

    ossieDebugLevel = 6;

    struct sched_param prio;

    prio.sched_priority = 5;

    int rc = sched_setscheduler(0, SCHED_RR, &prio);

    if (rc < 0)
        std::cerr << "Failed to set RR scheduler for USRP device." << std::endl;

    ossieSupport::ORB *orb = new ossieSupport::ORB;

    char *id = argv[1];
    char *label = argv[2];
    char *profile = argv[3]; 


    USRP_i* usrp_servant;
    CF::Device_var usrp_var;

    // Create the USRP device servant and object reference

    usrp_servant = new USRP_i(id, label, profile);
    usrp_var = usrp_servant->_this();

    std::string objName = "DomainName1/";
    objName += label;
    orb->bind_object_to_name((CORBA::Object_ptr) usrp_var, objName.c_str());


    // Start the orb
    orb->orb->run();

}
