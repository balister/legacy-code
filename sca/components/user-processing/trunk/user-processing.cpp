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

    while(1) {
        dataIn_0->getData(I_in, Q_in);

        len = I_in->length();


	// calculate energy
	unsigned long energy(0);
	for (unsigned int i(0); i < len; ++i) {
	    if (abs((*I_in)[i]) > abs((*Q_in)[i])) 
		energy += abs((*I_in)[i]);
	    else
		energy += abs((*Q_in)[i]);
	}
	energy = 10*log10(energy/len);


        dataIn_0->bufferEmptied();
    }
}

