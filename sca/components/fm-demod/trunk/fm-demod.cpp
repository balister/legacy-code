/****************************************************************************

Copyright 2008 Virginia Polytechnic Institute and State University

This file is part of the OSSIE WFMdemod.

OSSIE WFMdemod is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE WFMdemod is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE WFMdemod; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#include <string>
#include <iostream>

#include <math.h>
#include <assert.h>
#include <byteswap.h>

#include "fm-demod.h"

WFMDemod_i::WFMDemod_i(const char *uuid, omni_condition *condition) : 
Resource_impl(uuid), I1(0), I2(0), Q1(0), Q2(0), component_running(condition) 
{
    dataIn = new standardInterfaces_i::complexShort_p("dataIn");
    dataOut = new standardInterfaces_i::complexShort_u("dataOut");

    //Create the thread for the writer's processing function 
    processing_thread = new omni_thread(run, (void *) this);

    //Start the thread containing the writer's processing function 
    processing_thread->start();

}

WFMDemod_i::~WFMDemod_i(void)
{   
    delete dataIn;
    delete dataOut;
}

// Static function for omni thread
void WFMDemod_i::run( void * data )
{
    ((WFMDemod_i*)data)->ProcessData();
}

CORBA::Object_ptr WFMDemod_i::getPort( const char* portName ) throw (
    CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, WFMDemod, "getPort() invoked with " << portName)
    
    CORBA::Object_var p;

    p = dataIn->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    p = dataOut->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    /*exception*/
    throw CF::PortSupplier::UnknownPort();
}

void WFMDemod_i::start() throw (CORBA::SystemException, 
    CF::Resource::StartError)
{
    DEBUG(3, WFMDemod, "start() invoked")
}

void WFMDemod_i::stop() throw (CORBA::SystemException, CF::Resource::StopError) 
{  
    DEBUG(3, WFMDemod, "stop() invoked")
}

void WFMDemod_i::releaseObject() throw (CORBA::SystemException,
    CF::LifeCycle::ReleaseError)
{
    DEBUG(3, WFMDemod, "releaseObject() invoked")
    
    component_running->signal();
}

void WFMDemod_i::initialize() throw (CF::LifeCycle::InitializeError,
    CORBA::SystemException)
{
    DEBUG(3, WFMDemod, "initialize() invoked")
}

void WFMDemod_i::configure(const CF::Properties& props)
throw (CORBA::SystemException,
    CF::PropertySet::InvalidConfiguration,
    CF::PropertySet::PartialConfiguration)
{
    DEBUG(3, WFMDemod, "configure() invoked")
    
    std::cout << "props length : " << props.length() << std::endl;

    for (unsigned int i = 0; i <props.length(); i++)
    {
        std::cout << "Property id : " << props[i].id << std::endl;

        if (strcmp(props[i].id, "DCE:35c5eb82-c548-11dc-9f05-000c29a1ba37") == 0)
        {
            CORBA::Short simple_temp;
            props[i].value >>= simple_temp;
            simple_0_value = simple_temp;
        }

    }
}

void WFMDemod_i::query (CF::Properties & configProperties) throw (CORBA::SystemException, CF::UnknownProperties)
{
}

void WFMDemod_i::runTest (CORBA::ULong _number, CF::Properties & _props) throw (CORBA::SystemException, CF::TestableObject::UnknownTest, CF::UnknownProperties)
{
}

void WFMDemod_i::ProcessData()
{
    DEBUG(3, WFMDemod, "ProcessData() invoked");

    Demod_Lyons();
}


void WFMDemod_i::Demod_Lyons()
{
    PortTypes::ShortSequence I_out, Q_out;


    PortTypes::ShortSequence *I_in(NULL), *Q_in(NULL);

    while(1)
    {
        dataIn->getData(I_in, Q_in);

	unsigned int len = (*I_in).length();
	I_out.length(len);
	Q_out.length(len);

	for (unsigned int i(0); i < len; ++i) {
	    int I0((*I_in)[i]), Q0((*Q_in)[i]);

 	    DEBUG(10, WFMDemod, "Normalized: I_in = " << I0 << ", Q_in = " << Q0 << ", mag = " << sqrt(I0*I0 + Q0*Q0));

#if 1
	    // Calculate output from Lyon's Fig 13-61(b)

	    I_out[i] = ((I1 * (Q0 - Q2)) >> 10) - ((Q1 * (I0 - I2)) >> 10);
#else
	    // Brute force FM demod
	    I_out[i] = atan2((I0*I1 + Q0*Q1), (I1*Q0 - I0*Q1)) * 5000;
#endif	    
	    Q_out[i] = I_out[i];

	    // Update delay terms;

	    I2 = I1;
	    I1 = I0;
	    Q2 = Q1;
	    Q1 = Q0;

	    
	}

        dataIn->bufferEmptied();
        dataOut->pushPacket(I_out, Q_out);
    }

}
