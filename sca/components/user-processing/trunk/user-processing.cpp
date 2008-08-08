/****************************************************************************

Copyright 2008 Philip Balister, philip@opensdr.com

This file is part of the user-processing component.

user-processing is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

user-processing is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with user-processing; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/

#include <iostream>

#include <math.h>

#include "user-processing.h"
#include "ossie/debug.h"

user_processing_i::user_processing_i(const char *uuid, omni_condition *condition) : Resource_impl(uuid), component_running(condition) 
{
    dataIn_0 = new standardInterfaces_i::complexShort_p("DataIn");
    RXControl = new standardInterfaces_i::RX_Control_u("RX_Control");

    processing_thread = new omni_thread(run_process_data, (void *) this); 
    processing_thread->start();

}

user_processing_i::~user_processing_i(void)
{   
    delete dataIn_0;
}

CORBA::Object_ptr user_processing_i::getPort( const char* portName ) throw (CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, user_processing, "getPort called with : " << portName);
    
    CORBA::Object_var p;

    p = dataIn_0->getPort(portName);
    if (!CORBA::is_nil(p))
        return p._retn();

    p = RXControl->getPort(portName);
    if (!CORBA::is_nil(p))
        return p._retn();

    /*exception*/
    throw CF::PortSupplier::UnknownPort();
}

void user_processing_i::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    DEBUG(3, user_processing, "Start called");

}

void user_processing_i::stop() throw (CORBA::SystemException, CF::Resource::StopError) 
{  
    DEBUG(3, user_processing, "Stop called");
}

void user_processing_i::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    DEBUG(3, user_processing, "releaseObject called");
    
    component_running->signal();
}

void user_processing_i::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException)
{
    DEBUG(3, user_processing, "initialize called");

}

void user_processing_i::configure(const CF::Properties& props) throw (CORBA::SystemException, CF::PropertySet::InvalidConfiguration, CF::PropertySet::PartialConfiguration)
{
    DEBUG(3, user_processing, "configure called");
    
    DEBUG(4, user_processing, "props length : " << props.length());

    for (unsigned int i = 0; i <props.length(); i++)
    {
        DEBUG(4, user_processing, "Property id : " << props[i].id);

    }
}

void user_processing_i::query (CF::Properties & configProperties) throw (CORBA::SystemException, CF::UnknownProperties)
{
}

void user_processing_i::runTest (CORBA::ULong _number, CF::Properties & _props) throw (CORBA::SystemException, CF::TestableObject::UnknownTest, CF::UnknownProperties)
{
}

void user_processing_i::process_data()
{
    DEBUG(3, user_processing, "process_data thread started");

    PortTypes::ShortSequence *I_in(NULL), *Q_in(NULL);
    unsigned int len(0);
    unsigned int packet_count(0);

    CF::Properties props;
    CORBA::UShortSeq io(4);
    io.length(4);

    io[3] = 0xFFFE;
    io[2] = 2;
    io[1] = 0xFFFE;
    io[0] = 2;

    props.length(1);

    props[0].id = CORBA::string_dup("WRITE_IO");

    unsigned int sample_cnt(0);

    while(1) {
        dataIn_0->getData(I_in, Q_in);

	
	sample_cnt += I_in->length();
	if (sample_cnt > 250000) {
	    io[2] = io[2] << 1;
	    io[0] = io[0] << 1;
	    if (io[0] == 0) {
		io[0] = 2;
		io[2] = 2;
	    }
	    props[0].value <<= io;

	    RXControl->set_values(props);
	    sample_cnt = 0;
	}

	packet_count++;
	if (packet_count < 100) {
	    dataIn_0->bufferEmptied();
	    continue;
	}
	packet_count = 0;

        len = I_in->length();

	// calculate energy, assume two channels
	float energy1(0), energy2(0);
	for (unsigned int i(0); i < len; i+=2) {
	    energy1 += sqrt((*I_in)[i]*(*I_in)[i] + (*Q_in)[i]*(*Q_in)[i]);
	    energy2 += sqrt((*I_in)[i+1]*(*I_in)[i+1] + (*Q_in)[i+1]*(*Q_in)[i+1]);
	}

        unsigned int number_of_samples(len / 2);
	float average_energy1(20*log10(energy1/number_of_samples));
	float average_energy2(20*log10(energy2/number_of_samples));

        DEBUG(1, user-processing, "Packet energy 1 = " << average_energy1 << ", Packet energy 2 = " << average_energy2);

        dataIn_0->bufferEmptied();
    }
}

