/****************************************************************************

Copyright 2006 Virginia Polytechnic Institute and State University

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

/** \file
    The port_impl.h file contains definitions for the port implementations.
*/


#include <iostream>
#include <omnithread.h>

#include <unistd.h>

#include <ossie/debug.h>

#include "soundCardPlayback.h"


soundOut_i::soundOut_i(SoundCardPlayback_i *_base) : scp(_base)
{
    DEBUG(3, SoundCardPlayback, "Run constructor for soundOut ");
}

void soundOut_i::pushPacket(const PortTypes::ShortSequence &L, const PortTypes::ShortSequence &R)

{
    //    std::cout << "Entering sound pushPacket with insert_idx = " << scp->insert_idx << " and read_idx = " << scp->read_idx << std::endl;

    omni_mutex_lock(scp->playback_mutex);

    for (unsigned int i = 0; i < L.length(); ++i) {

        // Check for buffer full cases and sleep until these is room for data
	//\todo This needs to be refactored
	bool bufferFull(false);
	do {
	    bufferFull = false;
	    if ((scp->insert_idx == (scp->length - 2)) &&
		(scp->read_idx == 0)) {
		DEBUG(5, SoundCardPlayBack, "Sound playback overrun.");
		bufferFull = true;
	    } else if ((scp->read_idx - scp->insert_idx) == 2) {
		DEBUG(5, SoundCardPlayBack, "Sound playback overrun.");
		bufferFull = true;
	    }

	    if (bufferFull)
		usleep(100);

	} while(bufferFull);

        // Room to insert a sample
        scp->playback_buffer[scp->insert_idx] = L[i];
        scp->playback_buffer[scp->insert_idx + 1] = R[i];
        scp->insert_idx += 2;
        if (scp->insert_idx >= scp->length)
            scp->insert_idx = 0;
    }

    int len = scp->insert_idx - scp->read_idx;
    if (((len > 0) && (len > (scp->length/4))) ||
        ((len < 0) && (scp->length - scp->insert_idx + scp->read_idx) > scp->length/4))
        scp->data_available.signal();

}

#if 0   // Comment out until ready to implement sound input
RX_data_i::RX_data_i(int channel)
{
    std::cout << "Run constructor for RX channel " << channel << std::endl;
}

void RX_data_i::connectPort(CORBA::Object_ptr connection, const char *connectionId)
{
    std::cout << "Connect port for " << connectionId << std::endl;
}

void RX_data_i::disconnectPort(const char *connectionId)
{
    std::cout << "Disconnect port for " << connectionId << std::endl;
}
#endif

soundOutControl_i::soundOutControl_i()
{
    std::cout << "soundOutControl port constructor called" << std::endl;
}

soundInControl_i::soundInControl_i()
{
    std::cout << "soundInControl port constructor called" << std::endl;
}

