/*******************************************************************************

Copyright 2006,2007, Virginia Polytechnic Institute and State University

This file is the OSSIE Node Booter.

OSSIE nodebooter is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE nodebooter is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE nodebooter; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

nodebooter.cpp

*******************************************************************************/

#include <iostream>
#include <string>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <ossie/debug.h>
#include <ossie/cf.h>
#include <ossie/DomainManager_impl.h>
#include <ossie/DeviceManager_impl.h>
#include <ossie/ossieSupport.h>

using namespace ossieSupport;

/** \mainpage

\section Nodebooter

what it does

\section Installation

How to install

*/

/** \file
    A description of the nodebooter.cpp file
*/


void usage()
{
    std::cout << "nodeBooter -D<optional dmd file> -d dcd file -P path to Domain Manager file system -p path to Device Manager file system" << std::endl;
    std::cout << "Example: nodeBooter -d DeviceManager.dcd.xml -p /sdr/dev" << std::endl;
    std::cout << "Example: nodeBooter -D -d DeviceManager.dcd.xml -P /sdr/dom -p /sdr/dev" << std::endl;
    std::cout << "Example: nodeBooter -DDomainManager.dmd.xml -d DeviceManager.dcd.xml -P /sdr/dom -p /sdr/dev (the lack of a space between the -D and the filename is not a typo!" << std::endl;
}



int main(int argc, char *argv[])

{
    // Set debug verbosity 0 - no debug messages 3 for typical noisy level
    ossieDebugLevel = 10;

    DomainManager_impl *DomainManager_servant = NULL;
    DeviceManager_impl *DeviceManager_servant = NULL;

    CF::DomainManager_var DomainManager_objref;
    CF::DeviceManager_var DeviceManager_objref;



    // parse command line options

    int c;

    std::string dmdFile = "/domain/DomainManager.dmd.xml";
    std::string dcdFile;
    std::string devMgrPath = "/sdr/dev";
    std::string domMgrPath = "/sdr/dom";

    int startDeviceManager = 0;
    int startDomainManager = 0;


    while ((c = getopt(argc, argv, "D::d:P:p:h")) > 0) {
	switch (c) {
	case 'D' :
	    startDomainManager = 1;
	    if (optarg)
		dmdFile = optarg;

	    break;

	case 'd':
	    startDeviceManager = 1;
	    dcdFile = optarg;

	    break;

	case 'P' :
	    domMgrPath = optarg;

	    break;
	    
	case 'p' :
	    devMgrPath = optarg;

	    break;
	    
	case 'h':
	    usage();
	    break;

	}
    }

    // Check that there is work to do
    if (!(startDeviceManager || startDomainManager)) {
	usage();
	exit (0);
    }

    // Start CORBA

    ORB *orb_obj = new ORB(argc, argv);
  

    // Start Domain Manager if requested

    if (startDomainManager) {
	DEBUG(1, NB, "Starting Domain Manager with dmd file " << dmdFile << " and Dom Mgr root" << domMgrPath )

	// Create naming context for Domain. Must be done before servant
	// instantiation so the event channels can find their context
  
	///\todo Figure out how to make DomainName1 run time configurable
	///\todo and name context stuff to ORB class
	CosNaming::Name_var base_context = orb_obj->string_to_CosName("DomainName1");
        CosNaming::NamingContext_var rootContext;
 
	try { ///\todo review this code and see what alternative solutions exist
	    orb_obj->inc->bind_new_context (base_context);
	} catch (CosNaming::NamingContext::AlreadyBound &) {
            rootContext = CosNaming::NamingContext::_narrow(orb_obj->inc->resolve(base_context));
            orb_obj->unbind_all_from_context(rootContext);

	}
    
	// Create Domain Manager servant and object
       
	DomainManager_servant = new DomainManager_impl(dmdFile.c_str(), domMgrPath.c_str());
	DomainManager_objref = DomainManager_servant->_this();


	// Add object ref to the Name Service
	orb_obj->bind_object_to_name((CORBA::Object_ptr) DomainManager_objref, "DomainName1/DomainManager"); 

    }

    // Start Device Manager if requested
    if (startDeviceManager) {
	DEBUG(1, NB, "Starting Device Manager with " << dcdFile << " path " << devMgrPath);

	DeviceManager_servant = new DeviceManager_impl(dcdFile.c_str(), devMgrPath.c_str());
	DeviceManager_objref = DeviceManager_servant->_this();

	// finish initializing the Device Manager
	DeviceManager_servant->post_constructor(DeviceManager_objref);
    }

    orb_obj->orb->run();

}
