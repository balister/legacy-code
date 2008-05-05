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


#ifndef RATIONALRESAMPLERCOMPLEXSHORT_IMPL_H
#define RATIONALRESAMPLERCOMPLEXSHORT_IMPL_H

#include <vector>

#include <stdlib.h>

#include "ossie/cf.h"
#include "ossie/PortTypes.h"
#include "ossie/Resource_impl.h"
#include "ossie/debug.h"


#include "standardinterfaces/complexShort_u.h"
#include "standardinterfaces/complexShort_p.h"

#include "sigproc/SigProc.h"

/** \brief
 *
 *
 */
class RationalResamplerComplexShort_i : public virtual Resource_impl
{
  public:
    /// Initializing constructor
    RationalResamplerComplexShort_i(const char *uuid, omni_condition *sem);

    /// Destructor
    ~RationalResamplerComplexShort_i(void);

    /// Static function for omni thread
    static void run( void * data );

    ///
    void start() throw (CF::Resource::StartError, CORBA::SystemException);

    ///
    void stop() throw (CF::Resource::StopError, CORBA::SystemException);

    ///
    CORBA::Object_ptr getPort( const char* portName )
        throw (CF::PortSupplier::UnknownPort, CORBA::SystemException);

    ///
    void releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);

    ///
    void initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException);

    /// Configures properties read from .prf.xml
    void configure(const CF::Properties&)
        throw (CORBA::SystemException,
            CF::PropertySet::InvalidConfiguration,
            CF::PropertySet::PartialConfiguration);

    void query (CF::Properties & configProperties) throw (CORBA::SystemException, CF::UnknownProperties);

    void runTest (CORBA::ULong _number, CF::Properties & _props) throw (CORBA::SystemException, CF::TestableObject::UnknownTest, CF::UnknownProperties);

  private:
    /// Disallow default constructor
    RationalResamplerComplexShort_i();

    /// Disallow copy constructor
    RationalResamplerComplexShort_i(RationalResamplerComplexShort_i&);

    /// Main signal processing method
    void ProcessData();
   
    omni_condition *component_running;  ///< for component shutdown
    omni_thread *processing_thread;     ///< for component writer function
    	

    // Values from prf file
    CORBA::Short N; // Numerator
    CORBA::Short D; // Denominator
    std::vector<float> h;      // Resampling filter
    
    // Filters
    SigProc::fir_filter *i_filter, * q_filter;

    // provides and uses ports
    standardInterfaces_i::complexShort_u *dataOut;
    standardInterfaces_i::complexShort_p *dataIn;
    
};
#endif
