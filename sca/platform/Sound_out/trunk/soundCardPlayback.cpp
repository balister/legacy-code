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

/** \file
    The soundCardPlayback.cpp file contains definitions for the
    SoundCardPlayback_i class implementation.
*/

#include <iostream>

#include "ossie/cf.h"
#include "ossie/portability.h"

#include "soundCardPlayback.h"

#include <pulse/simple.h>
#include <pulse/error.h>

// Initializing constructor
SoundCardPlayback_i::SoundCardPlayback_i(char *id, char *label, char *profile)
{
    DEBUG(3, SoundCardPlayback, "constructor invoked")


    dev_id = id;
    dev_label = label;
    dev_profile = profile;

    sound_out_port = new standardInterfaces_i::complexShort_p("soundOut", "DomainName1", 20);

    dev_usageState = CF::Device::IDLE;
    dev_operationalState = CF::Device::ENABLED;
    dev_adminState = CF::Device::UNLOCKED;

    // Start playback thread
    play_sound_thread = new omni_thread(do_play_sound, (void *) this);
    play_sound_thread->start();

}

// Default destructor
SoundCardPlayback_i::~SoundCardPlayback_i()
{
}

// Device methods
CF::Device::UsageType SoundCardPlayback_i::usageState() throw (CORBA::SystemException)
{
    return dev_usageState;
}

CF::Device::AdminType SoundCardPlayback_i::adminState() throw (CORBA::SystemException)
{
    return dev_adminState;
}

CF::Device::OperationalType SoundCardPlayback_i::operationalState() throw (CORBA::SystemException)
{
    return dev_operationalState;
}

CF::AggregateDevice_ptr SoundCardPlayback_i::compositeDevice() throw (CORBA::SystemException)
{
    return NULL;
}

void SoundCardPlayback_i::adminState (CF::Device::AdminType _adminType)
throw (CORBA::SystemException)
{
    dev_adminState = _adminType;
}

CORBA::Boolean SoundCardPlayback_i::allocateCapacity (const CF::
Properties & capacities)
throw (CORBA::SystemException, CF::Device::InvalidCapacity,
CF::Device::InvalidState)
{

    return true;
}

void SoundCardPlayback_i::deallocateCapacity (const CF::Properties & capacities)
throw (CORBA::SystemException, CF::Device::InvalidCapacity,
CF::Device::InvalidState)
{

}

char *SoundCardPlayback_i::label ()
throw (CORBA::SystemException)
{
    return CORBA::string_dup(dev_label.c_str());
}


char *SoundCardPlayback_i::softwareProfile ()
throw (CORBA::SystemException)
{
    return CORBA::string_dup(dev_profile.c_str());
}

// Resource methods
void SoundCardPlayback_i::start() throw (CF::Resource::StartError, CORBA::SystemException)

{
    DEBUG(3, SoundCardPlayback, "start() invoked")
}

void SoundCardPlayback_i::stop() throw (CF::Resource::StopError, CORBA::SystemException)

{
    DEBUG(3, SoundCardPlayback, "stop() invoked")
}

char *SoundCardPlayback_i::identifier () throw (CORBA::SystemException)
{
    return CORBA::string_dup(dev_id.c_str());
}


// PortSupplier methods
CORBA::Object_ptr SoundCardPlayback_i::getPort(const char* portName) throw(CF::PortSupplier::UnknownPort, CORBA::SystemException)

{
    DEBUG(3, SoundCardPlayback, "getPort() invoked with : " << portName)
}

// Life Cycle methods
void SoundCardPlayback_i::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException)

{
}

void SoundCardPlayback_i::releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException)

{

    DEBUG(3, SoundCardPlayback, "releaseObject invoked")
}

// Property Set methods
void SoundCardPlayback_i::configure(const CF::Properties &props) throw (CORBA::SystemException, CF::PropertySet::InvalidConfiguration, CF::PropertySet::PartialConfiguration)
{
    DEBUG(3, SoundCardPlayback, "configure invoked. Number of props = " << props.length() );

    // read properties from .prf
    CORBA::ULong rate(16000); // default sampling rate

    for (unsigned int i=0; i<props.length(); i++)
    {
        DEBUG(3, SoundCardPlayback, "configure property id : " << props[i].id)

        if (strcmp(props[i].id, "DCE:98ca3738-5511-4fb1-ba00-2b86dc4e3c99")==0)
        {
            CORBA::ULong n;
            props[i].value >>= n;
            rate = n;
            DEBUG(3, SoundCardPlayback, "sample rate: " << rate << " Hz")
        }
        else
        {
            DEBUG(1, SoundCardPlayback, "ERROR: unkown configure() property id " << props[i].id)
            throw(CF::PropertySet::InvalidConfiguration());
        }
    }


}

void SoundCardPlayback_i::query (CF::Properties & configProperties)
throw (CORBA::SystemException, CF::UnknownProperties)
{

}

// TestableObject interfaces

void SoundCardPlayback_i::runTest (CORBA::ULong _number, CF::Properties & _props)
throw (CORBA::SystemException, CF::TestableObject::UnknownTest,
CF::UnknownProperties)
{

}

void SoundCardPlayback_i::play_sound()

{

    pa_sample_spec ss;

    ss.format = PA_SAMPLE_S16LE;
    ss.rate = 48000;
    ss.channels = 2;

    pa_simple *s(NULL);
    int error(0);

    if (!(s = pa_simple_new(NULL, "SoundOut", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {
	std::cerr << ": pa_simple_new() failed: " << pa_strerror(error) << std::endl;
	exit(-1);
    }

    unsigned int prev_len(0);
    short *buf(NULL);

    while (1) {
	PortTypes::ShortSequence *L(NULL), *R(NULL);
	
	sound_out_port->getData(L, R);

	unsigned int len = L->length();

	if (len > prev_len) {
	    delete buf;

	    prev_len = len;
	    buf = new short[len*2];
	    DEBUG(5, SoundCardPlayback, "buffer size is : " << len);
	}

	for (unsigned int i(0); i<len; i++) {
	    buf[i*2] = (*L)[i];
	    buf[i*2+1] = (*R)[i];
	}

	sound_out_port->bufferEmptied();

	if (pa_simple_write(s, buf, (size_t) len*4, &error) < 0) {
	    std::cerr << "pa_simple_write() failed: " << pa_strerror(error) << std::endl;
            exit(-1);
        }
    }
}

