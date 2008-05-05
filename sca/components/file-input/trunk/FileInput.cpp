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


#include <string>
#include <iostream>
#include <fstream>

#include <math.h>
#include <assert.h>
#include <byteswap.h>

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include <omnithread.h>

#include "FileInput.h"

#include "ossie/debug.h"
#include "ossie/Resource_impl.h"

FileInput_i::FileInput_i(const char *uuid, omni_condition *condition) : Resource_impl(uuid), component_running(condition), running(false), num_blocks(-1), file_repeat(1)
{
    dataOut = new standardInterfaces_i::complexShort_u("DataOut");
}

CORBA::Object_ptr FileInput_i::getPort( const char* portName ) throw (CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, FileInput, "FileInput_i getPort called with : " << portName)
    
    CORBA::Object_var p;

    p = dataOut->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    if (strcmp(portName, "Resource") == 0)
	return CORBA::Object::_duplicate(myObjectRef);

    /*exception*/
    throw CF::PortSupplier::UnknownPort();
}

void FileInput_i::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    DEBUG(3, FileInput, "start called on FileInput")

    // Create and start the thread that reads the file and send the data
    processing_thread = new omni_thread(run, (void *) this);
    processing_thread->start();

    running = true;
}

void FileInput_i::stop() throw (CORBA::SystemException, CF::Resource::StopError) 
{  
    DEBUG(3, FileInput, "stop called on FileInput")

    running = false;
}

void FileInput_i::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    DEBUG(3, FileInput, "releaseObject called on FileInput")
    
    component_running->signal();
}

void FileInput_i::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException)
{
    DEBUG(3, FileInput, "initialize called on FileInput")
}

void FileInput_i::configure(const CF::Properties& props) throw (CORBA::SystemException, CF::PropertySet::InvalidConfiguration, CF::PropertySet::PartialConfiguration)
{
    DEBUG(3, FileInput, "configure called on FileInput")
    
    DEBUG(4, FileInput, "props length : " << props.length())

    for (unsigned int i = 0; i <props.length(); i++){
        DEBUG(4, FileInput, "Property id : " << props[i].id)

	// input_file_name, string
	if (strcmp(props[i].id, "DCE:f86bbd8a-aac0-47ce-a5cb-a06f093de95f") == 0) {
	    const char *msg;
	    props[i].value >>= msg;

	    omni_mutex_lock l(accessPrivateData);
	    fileName = msg;
	    DEBUG(4, FileInput, "File name is: " << fileName)
	}
	// input_file_type, string
	if (strcmp(props[i].id, "DCE:240ba394-44c7-4369-84d1-29177e06b1a7") == 0) {
	    const char *msg;
	    props[i].value >>= msg;

	    omni_mutex_lock l(accessPrivateData);
	    fileType = msg;
	    DEBUG(4, FileInput, "File type is: " << fileType)
	}
	// num_blocks, unsigned long 
	else if (strcmp(props[i].id, "DCE:019fd89f-589f-4714-8d33-57f108d7d004") == 0) {
	    CORBA::Long val;
	    props[i].value >>= val;

	    omni_mutex_lock l(accessPrivateData);
	    num_blocks = val;
	}
	// file_repeat, unsigned long
	else if (strcmp(props[i].id, "DCE:23882924-7988-11db-9fc2-00123f573a7f") == 0) {
	    CORBA::ULong val;
	    props[i].value >>= val;

	    omni_mutex_lock l(accessPrivateData);
	    file_repeat = val;
	    DEBUG(4, FileInput, "File repeat = " << file_repeat)
	}
	// inter_packet_time, float
	else if (strcmp(props[i].id, "DCE:07624606-7a52-11db-820d-0004236abd3a") == 0) {
	    CORBA::Float val;
	    props[i].value >>= val;

	    omni_mutex_lock l(accessPrivateData);
	    packetDelay.tv_sec = (time_t) floor(val);
	    packetDelay.tv_nsec = (long) ((val - floor(val)) * 1e9);
	    DEBUG(4, FileInput, "Inter packet delay " << val << " split into "<< packetDelay.tv_sec << " seconds, " << packetDelay.tv_nsec << " nanoseconds.")
	}
	// Block size, long, -1 use filesize and repeat count as block size
	else if (strcmp(props[i].id, "DCE:3b35cc0c-cdda-446f-9e23-5877f213f1e9") == 0) {
	    CORBA::Long val;
	    props[i].value >>= val;

	    omni_mutex_lock l(accessPrivateData);
	    blockSize = val;
	    DEBUG(4, FileInput, "Block size = " << blockSize)
	}
    }
}

void FileInput_i::query (CF::Properties & configProperties) throw (CORBA::SystemException, CF::UnknownProperties)
{
}

void FileInput_i::runTest (CORBA::ULong _number, CF::Properties & _props) throw (CORBA::SystemException, CF::TestableObject::UnknownTest, CF::UnknownProperties)
{
}

void FileInput_i::process_data()
{
    DEBUG(3, FileInput, "FileInput process_data thread started");

    if (fileType == "S16_LE")
	process_binary_file();
    else if (fileType == "S16_BE")
	process_binary_file();
    else
	process_text_file(); //default to text file

    processing_thread->exit();

}

void FileInput_i::process_binary_file()
{
    DEBUG(3, FileInput, "Process binary data with type: " << fileType);

    // Each block element contains two shorts (I, Q)
    // Read number of shorts * 2 bytes per short to fill one data block

    assert(blockSize);

    short *dataBuffer = new(short[blockSize * 2]);

    unsigned int readLength(blockSize * 2 * sizeof(short));

    PortTypes::ShortSequence I, Q;

    I.length(blockSize);
    Q.length(blockSize);
    
    bool swapBytes(false);

    /// \todo take into account architecture we are built for, this code assumes we are running in little endian machine

#ifdef __powerpc
    if (fileType == "S16_LE")
	swapBytes = true;
    else if (fileType == "S16_BE")
	swapBytes = false;
#else // Default byte order is little endian
    if (fileType == "S16_LE")
	swapBytes = false;
    else if (fileType == "S16_BE")
	swapBytes = true;
#endif

    struct timespec delayTime(packetDelay);

    for (unsigned int k(0); k < file_repeat; ++k) {

	/// \todo Figure out why seeking to the begining of the file did not work.
	std::ifstream fin(fileName.c_str(), std::ios::in|std::ios::binary);
	
	if (!fin) {
	    std::cerr << "FileInput: Failed to open file: " << fileName << std::endl;
	    running = false;
	    processing_thread->exit();
	}

	while (true) {
	    
	    fin.read((char *)dataBuffer, readLength);
	    
	    // Do not worry about sending the last bit of data
	    // binary files are normally relatively long, the last few
	    // samples shouldn't be missed
	    if (fin.eof())
		break;
	    
	    for (unsigned int i(0); i < blockSize * 2; i+=2) {
		if (swapBytes) {
		    I[i/2] = bswap_16(dataBuffer[i]);
		    Q[i/2] = bswap_16(dataBuffer[i+1]);
		} else {
		    I[i/2] = dataBuffer[i];
		    Q[i/2] = dataBuffer[i+1];
		}
		DEBUG(10, FileInput, "I = " << I[i] << ", Q = " << Q[i]);
	    }
	    
	    dataOut->pushPacket(I, Q);
	}
	
	DEBUG(5, FileInput, "Iteration " << k << " completed.");
       	delay(delayTime);
	fin.close();
	
    }
    
    delete dataBuffer;
}

void FileInput_i::process_text_file()
{
    DEBUG(3, FileInput, "Process text data.");

    // read data from file
    unsigned int file_size(0);

    struct timespec delayTime(packetDelay);

    std::ifstream fin(fileName.c_str());

    if (!fin) {
	std::cerr << "FileInput: Failed to open file: " << fileName << std::endl;
	running = false;
	processing_thread->exit();
    }

    PortTypes::ShortSequence I, Q;

    while ( true )
    {
	short d, q;

        fin >> d >> q;

        if ( fin.eof() )
            break;

	DEBUG(20, FileInput, "input file: " << d << "    " << q)
	I.length(file_size + 1);
	Q.length(file_size + 1);
	I[file_size] = d;
	Q[file_size] = q;
        file_size++;
    }
    fin.close();
    DEBUG(2, FileInput, "File contained " << file_size << " I/Q pairs")

    // prepare output sequences
    PortTypes::ShortSequence I_out, Q_out;
    

    DEBUG(4, FileInput, "Ouput sequence length = " << file_size * file_repeat)
    I_out.length(file_size * file_repeat);
    Q_out.length(file_size * file_repeat);
    
    for (unsigned int i = 0; i < file_repeat; ++i) {
	for (unsigned int j = 0; j < file_size; ++j) {
	    I_out[i * file_size + j] = I[j];
	    Q_out[i * file_size + j] = Q[j];
	}
    }

    DEBUG(4, FileInput, "Modulator:About to send data.")

    // send output sequences num_block times
    int k(num_blocks);
    PortTypes::ShortSequence I_block, Q_block;

    if (blockSize > 0) {
	I_block.length(blockSize);
	Q_block.length(blockSize);
    }

    while( running )
    {
	if (blockSize < 0) { // size of I_out is block size
	    dataOut->pushPacket(I_out, Q_out);
	} else {  // size of I_out is not block size
	  DEBUG(5, FileInput, "Splitting input to blockSize: " << blockSize)
	    for (unsigned int i = 0; i < I_out.length()/blockSize; ++i) {
	      DEBUG(6, FileInput, "Sending block: " << i)
		for (unsigned int j = 0; j < blockSize; ++j) {
		    I_block[j] = I_out[i * blockSize + j];
		    Q_block[j] = Q_out[i * blockSize + j];
		}
		dataOut->pushPacket(I_block, Q_block);
	    }
	    if ((I_out.length() % blockSize) != 0) { // Send last bits and pad with zero
		unsigned int p = I_out.length() - (blockSize * (I_out.length() / blockSize));
		DEBUG(5, FileInput, "Left over samples = " << p)
		for (unsigned int i = 0; i < blockSize; ++i) {
		    if (p < I_out.length()) {
			I_block[i] = I_out[p];
			Q_block[i] = Q_out[p];
			++p;
		    } else {
			I_block[i] = 0;
			Q_block[i] = 0;
		    }
		}
		dataOut->pushPacket(I_block, Q_block);
	    }
	}

	if (k > 0)
	    if (--k == 0)
		running = false;
	
	delay(delayTime);

    }

}

void FileInput_i::delay(struct timespec waitTime)
{

    bool done(false);
    struct timespec rem;
    while (!done) {
        int ret = nanosleep(&waitTime, &rem);
    
        if (ret == EINTR)
            waitTime = rem;
        else
            done = true;
    }
}
