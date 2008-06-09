/****************************************************************************

Copyright 2008 Philip Balister philip@opensdr.com

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


#ifndef USERPROCESSING_IMPL_H
#define USERPROCESSING_IMPL_H

#include <string>

#include <stdlib.h>

#include "ossie/cf.h"
#include "ossie/PortTypes.h"

#include "standardinterfaces/complexShort_p.h"

#include "ossie/Resource_impl.h"

void process_data(void *data);

class user-processing_i : public virtual Resource_impl
{

    public:
        user-processing_i(const char *uuid, omni_condition *sem);
        ~user-processing_i(void);

	static void run_process_data(void *data) { ((user-processing_i *)data)->process_data(); };

        void start() throw (CF::Resource::StartError, CORBA::SystemException);
        void stop() throw (CF::Resource::StopError, CORBA::SystemException);

        CORBA::Object_ptr getPort( const char* portName ) throw (CF::PortSupplier::UnknownPort, CORBA::SystemException);

        void releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);

        void initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException);
        void configure(const CF::Properties&) throw (CORBA::SystemException, CF::PropertySet::InvalidConfiguration, CF::PropertySet::PartialConfiguration);


    private:
        user-processing_i();
        user-processing_i(user-processing_i&);
   
	void process_data();

        omni_condition *component_running;  //for component shutdown
        omni_thread *processing_thread;     //for component writer function
    	
        //list components provides and uses ports
        standardInterfaces_i::complexShort_p *dataIn_0;
        
};
#endif
