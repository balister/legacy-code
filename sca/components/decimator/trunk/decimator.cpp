/****************************************************************************

Copyright 2005,2006, 2008 Virginia Polytechnic Institute and State University

This file is part of the OSSIE Decimator.

OSSIE Decimator is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE Decimator is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE Decimator; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


****************************************************************************/

#include <iostream>

#include <string.h>
#include <math.h>  // For energy debugging routine
#include "ossie/cf.h"

#include "decimator.h"

    //#define PRINT_ENERGY 1

Decimator_i::Decimator_i(const char *uuid, omni_condition *condition) : Resource_impl(uuid), component_running(condition), M(1), sample_count(0), previous_length(0)
{
    // Create the port for output data
    dataOut = new standardInterfaces_i::complexShort_u("outData");

    // Create port for input Data
    dataIn = new standardInterfaces_i::complexShort_p("inData");

    // Start the run_decimation thread
    processing_thread = new omni_thread(do_run_decimation, (void *) this);
    processing_thread->start();


    // Filter length is L
    len_h = 1;
    h = new float[len_h];

    h[0] = 1.0;

    i_filter = new SigProc::fir_filter(h, len_h);
    q_filter = new SigProc::fir_filter(h, len_h);

    outFile = new std::ofstream("decimator_out.dat");
}

void Decimator_i::start() throw (CF::Resource::StartError, CORBA::SystemException)

{
    std::cout << "Start Decimator called" << std::endl;

}

void Decimator_i::stop() throw (CF::Resource::StopError, CORBA::SystemException)

{
    std::cout << "Stop decimator called" << std::endl;

}

CORBA::Object_ptr Decimator_i::getPort(const char* portName) throw(CF::PortSupplier::UnknownPort, CORBA::SystemException)

{
    std::cout << "Decimator getPort called with : " << portName << std::endl;

    CORBA::Object_var p;

    p = dataOut->getPort(portName);

    if (!CORBA::is_nil(p))
	return p._retn();

    p = dataIn->getPort(portName);

    if (!CORBA::is_nil(p))
	return p._retn();

    throw CF::PortSupplier::UnknownPort();
}

void Decimator_i::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException)

{
    std::cout << "Decimator Initialize called" << std::endl;

}

void Decimator_i::releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException)

{

    std::cout << "decimator releaseObject called" << std::endl;

    delete outFile;

    // Clear the component running semaphore so main shuts down everything
    component_running->signal();
}

void Decimator_i::configure(const CF::Properties& props) throw (CORBA::SystemException, CF::PropertySet::InvalidConfiguration, CF::PropertySet::PartialConfiguration)

{
    std::cout << "decimator Configure called" << std::endl;

    std::cout << "Props length : " << props.length() << std::endl;

    for (unsigned int i = 0; i < props.length(); i++) {
	std::cout << "Property id : " << props[i].id << std::endl;
	
	// DecimateBy property, sets the decimator factor
	if (strcmp(props[i].id, "DCE:cea26b54-9d86-4b68-a761-14186efa9415") == 0) {
	    CORBA::UShort D;
	    props[i].value >>= D;
	    M = D;
	    std::cout << "Decimation factor set to " << M << std::endl;
	}

	// filter property, Filter coefficients
	if (strcmp(props[i].id, "DCE:134e5dd8-c773-47af-a557-2837076358c4") == 0) {
	    CORBA::FloatSeq *coeff_ptr;
	    props[i].value >>= coeff_ptr;

	    len_h = coeff_ptr->length();

	    delete []h;
	    delete i_filter;
	    delete q_filter;

	    h = new float[len_h];
	    std::cout << "Decimator filter length = " << len_h << std::endl;
	    for (unsigned int i = 0; i < len_h; i++) {
		h[i] = (*coeff_ptr)[i];
		std::cout << "Coeff[" << i << "] = " << h[i] << std::endl;
	    }
	    i_filter = new SigProc::fir_filter(h, len_h);
	    q_filter = new SigProc::fir_filter(h, len_h);

	}
	
    }
}

void Decimator_i::query (CF::Properties & configProperties) throw (CORBA::SystemException, CF::UnknownProperties)
{
}

void Decimator_i::runTest (CORBA::ULong _number, CF::Properties & _props) throw (CORBA::SystemException, CF::TestableObject::UnknownTest, CF::UnknownProperties)
{
}


void Decimator_i::run_decimation()
{
    std::cout << "run_decimation thread started" << std::endl;

#ifdef LOG_DATA
    dump_data pre_decim("pre.dat", 1000, 1000);
    dump_data post_decim("post.dat", 1000, 1000);
#endif

    PortTypes::ShortSequence I_out, Q_out;

    while (1) {
	PortTypes::ShortSequence *I_in(NULL), *Q_in(NULL);


	dataIn->getData(I_in, Q_in);

	unsigned int len = I_in->length();

	if (len != previous_length) {
	    I_out.length(len/M + 2);
	    Q_out.length(len/M + 2);
	}

	unsigned int out_idx(0);

#ifdef PRINT_ENERGY
	float E_in = 0, E_out = 0;
#endif

	for (unsigned int i = 0; i < len; ++i) {
	    short i_out, q_out;

	    sample_count = (++sample_count) % M;

	    bool output_sample(false);
	    if (sample_count == 0)
		output_sample = true;

	    i_filter->do_work(output_sample, (*I_in)[i], i_out);
	    q_filter->do_work(output_sample, (*Q_in)[i], q_out);

#ifdef LOG_DATA
	    pre_decim.write_data((float)(*I_in)[i], (float)(*Q_in)[i]);
#endif

#ifdef PRINT_ENERGY
	    E_in  += (*I_in)[i] * (*I_in)[i] + (*Q_in)[i] * (*Q_in)[i];
	    E_out += i_out * i_out + q_out * q_out;
#endif

	    if (output_sample) {
#ifdef LOG_DATA
       		post_decim.write_data((float)i_out, (float)q_out);
#endif
		I_out[out_idx] = i_out;
		Q_out[out_idx] = q_out;
		++out_idx;
	    }

	}


	dataIn->bufferEmptied();

#ifdef PRINT_ENERGY
	std::cout << "Energy in = " << 10 * log10(E_in/decimator->I_in.length() + 0.01) << "  Energy out = " << 10 * log10(E_out/out_idx + 0.01) << std::endl;
#endif

	// Set length of output sequences to actual number of samples
	I_out.length(out_idx);
	Q_out.length(out_idx);

	dataOut->pushPacket(I_out, Q_out);
    }

}

short Decimator_i::f2s(float r)
{
    if (r > SHRT_MAX)
	return SHRT_MAX;

    if (r < SHRT_MIN)
	return SHRT_MIN;

    short ret = (short) r;

    return ret;
}
