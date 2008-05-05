/****************************************************************************

Copyright 2005, 2006, 2008 Virginia Polytechnic Institute and State University

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

#include <fstream>

#include "ossie/cf.h"
#include "ossie/PortTypes.h"

#include "standardinterfaces/complexShort_u.h"
#include "standardinterfaces/complexShort_p.h"

#include "sigproc/SigProc.h"

#include "ossie/Resource_impl.h"

// USRP test component definition
class Decimator_i : public virtual Resource_impl

{
  public:
    Decimator_i(const char *uuid, omni_condition *sem);

    static void do_run_decimation(void *data) { ((Decimator_i *)data)->run_decimation(); };

    void start() throw (CF::Resource::StartError, CORBA::SystemException);
    void stop() throw (CF::Resource::StopError, CORBA::SystemException);

    CORBA::Object_ptr getPort(const char* portName) throw(CF::PortSupplier::UnknownPort, CORBA::SystemException);

    void initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException);
    void releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);

    void configure(const CF::Properties&) throw (CORBA::SystemException, CF::PropertySet::InvalidConfiguration, CF::PropertySet::PartialConfiguration);
    void query (CF::Properties & configProperties) throw (CORBA::SystemException, CF::UnknownProperties);

    void runTest (CORBA::ULong _number, CF::Properties & _props) throw (CORBA::SystemException, CF::TestableObject::UnknownTest, CF::UnknownProperties);


 private:
    Decimator_i(); // No default constructor
    Decimator_i(Decimator_i&); // No copying

    void run_decimation();

    // For component shutdown
    omni_condition *component_running;

    omni_thread *processing_thread;

    // For decimation operation
    float *h;  // Array for filter coefficients
    unsigned int len_h;  // Length of filter

    unsigned int M;  // Decimation factor
    unsigned int sample_count;
    unsigned int previous_length; // Length of previous input sequence

    SigProc::fir_filter *i_filter, * q_filter; // Signal processing object pointers

    short f2s(float r); // convert float to short with clipping


    // Data out port
    standardInterfaces_i::complexShort_u* dataOut;
    standardInterfaces_i::complexShort_p* dataIn;

    // debugging
    std::ofstream *outFile;
};
