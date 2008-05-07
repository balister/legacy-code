/****************************************************************************

Copyright 2006, 2008 Virginia Polytechnic Institute and State University

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
    ossieDebugLevel = 1;

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

    // Create control ports for sound in and out control

    soundOutControl_i* soundOutControl_servant;
    audioOutControl_var soundOutControl_var;

    soundOutControl_servant = new soundOutControl_i();
    soundOutControl_var = soundOutControl_servant->_this();

    soundInControl_i* soundInControl_servant;
    audioInControl_var soundInControl_var;

    soundInControl_servant = new soundInControl_i();
    soundInControl_var = soundInControl_servant->_this();

    objName = "DomainName1/";
    objName += "soundOutControl";
    orb->bind_object_to_name((CORBA::Object_ptr) soundOutControl_var, objName.c_str());
    
    objName = "DomainName1/";
    objName += "soundInControl";
    orb->bind_object_to_name((CORBA::Object_ptr) soundInControl_var, objName.c_str());


    // Create the ports for sound output data

    soundOut_i* soundOut;

    complexShort_var soundOut_var;

    soundOut = new soundOut_i(soundCardPlayback_servant);
    soundOut_var = soundOut->_this();

    objName = "DomainName1/";
    objName += "soundOut";
    orb->bind_object_to_name((CORBA::Object_ptr) soundOut_var, objName.c_str());

#if 0 // Save for when I need to input from souncard
    // Create the ports for RX Data

    RX_data_i *rx_data_1;

    CF::Port_var rx_data_1_var;

    rx_data_1 = new RX_data_i(1);

    rx_data_1_var = rx_data_1->_this();

    objName = "DomainName1/";
    objName += "RX_Data_1";
    orb->bind_object_to_name((CORBA::Object_var) rx_data_1_var, objName.c_str());
#endif

    // Start the orb
    orb->orb->run();

}
