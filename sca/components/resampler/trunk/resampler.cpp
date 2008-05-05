/****************************************************************************

Copyright 2008 Virginia Polytechnic Institute and State University

This file is part of the OSSIE RationalResamplerComplexShort.

OSSIE RationalResamplerComplexShort is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE RationalResamplerComplexShort is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE RationalResamplerComplexShort; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#include <string>
#include <iostream>

#include <ossie/debug.h>

#include "resampler.h"

RationalResamplerComplexShort_i::RationalResamplerComplexShort_i(const char *uuid, omni_condition *condition) : 
Resource_impl(uuid), component_running(condition), N(1), D(1), i_filter(NULL), q_filter(NULL), dataOut(NULL), dataIn(NULL)
{
    dataOut = new standardInterfaces_i::complexShort_u("dataOut");
    dataIn = new standardInterfaces_i::complexShort_p("dataIn");

    //Create the thread for the writer's processing function 
    processing_thread = new omni_thread(run, (void *) this);

    //Start the thread containing the writer's processing function 
    processing_thread->start();

}

RationalResamplerComplexShort_i::~RationalResamplerComplexShort_i(void)
{   
    delete dataOut;
    delete dataIn;
}

// Static function for omni thread
void RationalResamplerComplexShort_i::run( void * data )
{
    ((RationalResamplerComplexShort_i*)data)->ProcessData();
}

CORBA::Object_ptr RationalResamplerComplexShort_i::getPort( const char* portName ) throw (
    CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, RationalResamplerComplexShort, "getPort() invoked with " << portName)
    
    CORBA::Object_var p;

    p = dataOut->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    p = dataIn->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    /*exception*/
    throw CF::PortSupplier::UnknownPort();
}

void RationalResamplerComplexShort_i::start() throw (CORBA::SystemException, 
    CF::Resource::StartError)
{
    DEBUG(3, RationalResamplerComplexShort, "start() invoked")
}

void RationalResamplerComplexShort_i::stop() throw (CORBA::SystemException, CF::Resource::StopError) 
{  
    DEBUG(3, RationalResamplerComplexShort, "stop() invoked")
}

void RationalResamplerComplexShort_i::releaseObject() throw (CORBA::SystemException,
    CF::LifeCycle::ReleaseError)
{
    DEBUG(3, RationalResamplerComplexShort, "releaseObject() invoked")
    
    component_running->signal();
}

void RationalResamplerComplexShort_i::initialize() throw (CF::LifeCycle::InitializeError,
    CORBA::SystemException)
{
    DEBUG(3, RationalResamplerComplexShort, "initialize() invoked")
}

void RationalResamplerComplexShort_i::configure(const CF::Properties& props)
throw (CORBA::SystemException,
    CF::PropertySet::InvalidConfiguration,
    CF::PropertySet::PartialConfiguration)
{
    DEBUG(3, RationalResamplerComplexShort, "configure() invoked");
    
    DEBUG(3, RationalResamplerComplexShort, "props length : " << props.length());

    for (unsigned int i = 0; i <props.length(); i++)
    {
        DEBUG(3, RationalResamplerComplexShort, "Property id : " << props[i].id);

	// Numerator, Short
        if (strcmp(props[i].id, "DCE:20134bba-c7a8-11dc-96f6-000c29a1ba37") == 0) {
            CORBA::Short tmp; ///\todo convert to unsigned short
            props[i].value >>= tmp;
	    DEBUG(5, RationalResamplerComplexShort, "Numerator = " << tmp);
	    if (tmp == 0) {
		DEBUG(1, RationalResamplerComplexShort, "Numerator cannot be 0");
		throw CF::PropertySet::InvalidConfiguration();
	    }
            N = tmp;
        }

	// Denominator, Short
        if (strcmp(props[i].id, "DCE:2fdaf14c-c7a8-11dc-8a7c-000c29a1ba37") == 0) {
            CORBA::Short tmp; ///\todo convert to unsigned short
            props[i].value >>= tmp;
	    DEBUG(5, RationalResamplerComplexShort, "Denominator = " << tmp);
	    if (tmp == 0) {
		DEBUG(1, RationalResamplerComplexShort, "Denominator cannot be 0");
		throw CF::PropertySet::InvalidConfiguration();
	    }
            D = tmp;
        }

	// Filter coefficients, Float sequence
        if (strcmp(props[i].id, "DCE:4899d72a-c7a8-11dc-abeb-000c29a1ba37") == 0) {
	    CORBA::FloatSeq *f;
            props[i].value >>= f;

	    DEBUG(5, RationalResamplerComplexShort, "Reading filter coefficients");

	    h.clear();
	    delete i_filter;
	    delete q_filter;

	    for (unsigned int i(0); i < f->length(); ++i) {
		DEBUG(8, RationalResamplerComplexShort, "Filter h[" << i << "] = " << (*f)[i]);
		h.push_back((*f)[i]);
	    }

	    i_filter = new SigProc::fir_filter(&h[0], h.size());
	    q_filter = new SigProc::fir_filter(&h[0], h.size());
        }

    }
}

void RationalResamplerComplexShort_i::query (CF::Properties & configProperties) throw (CORBA::SystemException, CF::UnknownProperties)
{
}

void RationalResamplerComplexShort_i::runTest (CORBA::ULong _number, CF::Properties & _props) throw (CORBA::SystemException, CF::TestableObject::UnknownTest, CF::UnknownProperties)
{
}

void RationalResamplerComplexShort_i::ProcessData()
{
    DEBUG(3, RationalResamplerComplexShort, "ProcessData() called")

    PortTypes::ShortSequence I_out, Q_out;


    PortTypes::ShortSequence *I_in(NULL), *Q_in(NULL);
    unsigned int previous_length(0);
    int sample_count(0);

    while(1)
    {

        dataIn->getData(I_in, Q_in);


        unsigned int len = I_in->length();

	DEBUG(5, RationalResamplerComplexShort, "Got data, len = " << len);

        if (len != previous_length) {
            I_out.length(len*N/D + 2);
            Q_out.length(len*N/D + 2);
	    previous_length = len;
        }

        unsigned int out_idx(0);
	unsigned int in_idx(0);

	DEBUG(9, RationalResamplerComplexShort, "Entering for loop len = " << len << ", N = " << N);
	unsigned int D_cnt(0);
	unsigned N_cnt(0);
        for (unsigned int i(0); i < (len * N); ++i) {
            short i_out, q_out;

	    ++sample_count;
	    ++D_cnt;

	    DEBUG(9, RationalResamplerComplexShort, "Sample count = " << sample_count);
            bool output_sample(false);
            if (D_cnt == D) {
                output_sample = true;
		D_cnt = 0;
	    }

	    N_cnt++;
	    if (N_cnt == N) {
		DEBUG(10, RationalResamplerComplexShort, "Loading i_in = " << (*I_in)[in_idx] << ", q_in = " << (*Q_in)[in_idx] << " into filter");
		i_filter->do_work(output_sample, (*I_in)[in_idx], i_out);
		q_filter->do_work(output_sample, (*Q_in)[in_idx], q_out);
		++in_idx;
		N_cnt = 0;
	    } else {
		DEBUG(10, RationalResamplerComplexShort, "Loading 0 into filter");
		i_filter->do_work(output_sample, 0, i_out);
		q_filter->do_work(output_sample, 0, q_out);
	    }

            if (output_sample) {
                I_out[out_idx] = i_out;
                Q_out[out_idx] = q_out;
                ++out_idx;

		DEBUG(10, RationalResamplerComplexShort, "i_out = " << i_out << ", q_out = " << q_out);
            }

        }


        dataIn->bufferEmptied();

        // Set length of output sequences to actual number of samples
        I_out.length(out_idx);
        Q_out.length(out_idx);

	DEBUG(5, RationalResamplerComplexShort, "Sending data, packet size = " << out_idx);
        dataOut->pushPacket(I_out, Q_out);
    }
}


