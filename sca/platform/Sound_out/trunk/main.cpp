/****************************************************************************

Copyright 2006, 2008 Virginia Polytechnic Institute and State University
Copyright 2008       Philip Balister, philip@opensdr.com

This file is part of the OSSIE Sound_out Device.

OSSIE Sound_out Device is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE Sound_out Device is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE Sound_out Device; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


****************************************************************************/

#include <iostream>
#include "ossie/ossieSupport.h"

#include "soundCardPlayback.h"

using namespace std;
using namespace standardInterfaces;  // For standard OSSIE interface classes


int main(int argc, char* argv[])

{
    ossieDebugLevel = 10;

    ossieSupport::ORB *orb = new ossieSupport::ORB;

    if (argc != 4) {
	cout << argv[0] << " <identifier> <usage name> <software profile>" << endl;
	exit (-1);
    }

    char *id = argv[1];
    char *label = argv[2];
    char *profile = argv[3]; 


    SoundCardPlayback_i* soundCardPlayback_servant;
    CF::Device_var soundCardPlayback_var;

    // Create the Sound Card device servant and object reference

    soundCardPlayback_servant = new SoundCardPlayback_i(id, label, profile);
    soundCardPlayback_var = soundCardPlayback_servant->_this();

    string objName = "DomainName1/";
    objName += label;
    orb->bind_object_to_name((CORBA::Object_ptr) soundCardPlayback_var, objName.c_str());

    // Start the orb
    orb->orb->run();

}
