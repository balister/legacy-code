/****************************************************************************

Copyright 2006, 2008 Virginia Polytechnic Institute and State University

This file is part of the OSSIE FileInput.

OSSIE FileInput is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE FileInput is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE FileInput; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#ifndef FILEINPUT_IMPL_H
#define FILEINPUT_IMPL_H

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include "ossie/cf.h"
#include "ossie/PortTypes.h"
#include "ossie/Resource_impl.h"

#include "standardinterfaces/complexShort_u.h"

class FileInput_i : public virtual Resource_impl
{
    public:
        FileInput_i(const char *uuid, omni_condition *sem);

	void post_constructor(CF::Resource_ptr ref) { myObjectRef = ref; };
	static void run(void *data) { ((FileInput_i *)data)->process_data(); };

        void start() throw (CF::Resource::StartError, CORBA::SystemException);
        void stop() throw (CF::Resource::StopError, CORBA::SystemException);

        CORBA::Object_ptr getPort( const char* portName ) throw (CF::PortSupplier::UnknownPort, CORBA::SystemException);

        void releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);

        void initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException);
        void configure(const CF::Properties&) throw (CORBA::SystemException, CF::PropertySet::InvalidConfiguration, CF::PropertySet::PartialConfiguration);
	void query (CF::Properties & configProperties) throw (CORBA::SystemException, CF::UnknownProperties);

	void runTest (CORBA::ULong _number, CF::Properties & _props) throw (CORBA::SystemException, CF::TestableObject::UnknownTest, CF::UnknownProperties);


    private:
        FileInput_i();
        FileInput_i(const FileInput_i&);
   
	void process_data();
	void process_text_file();
	void process_binary_file();

	void delay(struct timespec waitTime);

	CF::Resource_var myObjectRef;

        omni_condition *component_running;  //for component shutdown
        omni_thread *processing_thread;     //for component writer function
    	omni_mutex accessPrivateData;           // for access to private data

	bool running;

	// Values from properties
	std::string fileName;
	std::string fileType;
	unsigned long num_blocks;
	unsigned long file_repeat;
	struct timespec packetDelay;
	unsigned int blockSize;
	
        //list components provides and uses ports
        standardInterfaces_i::complexShort_u *dataOut;

};
#endif
