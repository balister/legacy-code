/****************************************************************************

Copyright 2005, 2006, 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE GPP.

OSSIE GPP is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE GPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE GPP; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


****************************************************************************/

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <errno.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "GPP.h"

#include "ossie/debug.h"

namespace fs = boost::filesystem;

GPP_i::GPP_i (char *id, char* label, char* profile)

{

    dev_id = id;
    dev_label = label;
    dev_profile = profile;

    localFileRoot = "/sdr";
    localFileRoot /= dev_label;
    fs::create_directory(localFileRoot);

    // Assumption: all executable files named as /bin/exec_name
    fs::path binDir = localFileRoot / "bin";
    fs::create_directory(binDir);

    dev_usageState = CF::Device::IDLE;
    dev_operationalState = CF::Device::ENABLED;
    dev_adminState = CF::Device::UNLOCKED;

}

// ExecutableDevice interface

CF::ExecutableDevice::ProcessID_Type GPP_i::execute (const char 
*name, const CF::Properties & options, const CF::Properties & parameters) throw
     (CORBA::SystemException, CF::Device::InvalidState,
      CF::ExecutableDevice::InvalidFunction,
      CF::ExecutableDevice::InvalidParameters,
      CF::ExecutableDevice::InvalidOptions, CF::InvalidFileName,
      CF::ExecutableDevice::ExecuteFail)
{
    DEBUG(3, GPP, "Executing: " << name);

    // verify device is in the correct state
    if (!isUnlocked () || isDisabled ()) {
        DEBUG(1, GPP, "Cannot execute. System is either LOCKED, SHUTTING DOWN or DISABLED.")
        throw (CF::Device::InvalidState("Cannot execute. System is either LOCKED, SHUTTING DOWN or DISABLED."));
    }

    if (!isFileLoaded (name)) {
        DEBUG(1, GPP, "Cannot Execute. File was not already loaded.");
        throw (CF::InvalidFileName (CF::CFENOENT, "Cannot Execute. File was not already loaded."));
    }
    

    // All parameters must be strings
    for (unsigned int i = 0; i < parameters.length (); i++) {
	CORBA::TypeCode_var tc = parameters[i].value.type ();
        
        if (tc->kind () != CORBA::tk_string) {
            DEBUG(3, GPP, "Invalid Parameters.")
            throw CF::ExecutableDevice::InvalidParameters();
        }
    }
    
    /// \todo implement stack size and priority control for GPP device
    CORBA::ULong stackSize, priority;
    priority=0;
    stackSize=4096;
    
    // Validate options
    for (unsigned int i = 0; i < options.length (); i++) {
        /// \todo Move type validation into local function (maybe global)

        if (strcmp (options[i].id, CF::ExecutableDevice::PRIORITY_ID) == 0) {
	    CORBA::TypeCode_var tc;
	    tc = options[i].value.type ();
	    
	    if (tc->kind () != CORBA::tk_ulong) {
		DEBUG(1, GPP, "Invalid Options.");
		throw CF::ExecutableDevice::InvalidOptions();
	    }
            options[i].value >>= priority;
        }

	if (strcmp (options[i].id, CF::ExecutableDevice::STACK_SIZE) == 0) {
	    CORBA::TypeCode_var tc;
	    tc = options[i].value.type ();
	    
	    if (tc->kind () != CORBA::tk_ulong) {
		DEBUG(1, GPP, "Invalid Options.");
		throw CF::ExecutableDevice::InvalidOptions();
	    }
            options[i].value >>= stackSize;
	}
    }
    
    // Create argv array for exec

    bool valgrind(false);
    if (getenv("VALGRIND"))
	valgrind = true;

    unsigned int argv_len(2 + parameters.length());
    if (valgrind)
	argv_len += 2;

    char *argv[argv_len];
    unsigned int argv_idx(0);

    // Needed for the valgrind case
    std::string valgrind_path = "/usr/bin/valgrind";
    std::string logFile = "--log-file=";


    // Extract executable file name
    fs::path execPath = localFileRoot;
    execPath /= name;

    if (valgrind) {
	argv[argv_idx++] = (char *) valgrind_path.c_str();
	logFile += execPath.string().c_str();
	argv[argv_idx++] = (char *) logFile.c_str();
    }

    // Add executable file name
    argv[argv_idx++] = (char *) execPath.string().c_str();

    DEBUG(5, GPP, "Executing file name: " << argv[argv_idx-1]);


    // Add parameters to argv array
    for (unsigned int i = 0; i < parameters.length (); i++) {
	const char *tmp = parameters[i].id; // Muck around to extract CORBA::String_var
	argv[argv_idx++] = (char *) tmp;

	const char *tmpStr;
	parameters[i].value >>= tmpStr;
	argv[argv_idx++] = (char *) tmpStr;
    }

    DEBUG(8, GPP, "argv_idx = " << argv_idx);
    // Set last entry in argv to NULL to mark end of argument list
    argv[argv_idx] = NULL;

    CF::ExecutableDevice::ProcessID_Type new_pid;

    DEBUG(6, GPP, "Forking ...");
    if ((new_pid = fork()) < 0) {
        DEBUG(1, GPP, "error executing fork()");
        return((CF::ExecutableDevice::ProcessID_Type) -1);
    }
    
    CF::ExecutableDevice::ProcessID_Type PID;

    if (new_pid > 0) {
        
        // in parent process
        PID = new_pid;
    } else {
        // in child process
	DEBUG(6, GPP, "execing (without valgrind), argv[0]: " << argv[0]);
	execv(argv[0], argv);

	DEBUG(1, GPP, "Exec failed, errno =  " << errno << ", message: " << strerror(errno));
	exit(-1); // If we ever get here the program we tried to start and failed
    }

    DEBUG(6, GPP, "Returning, PID = " << PID);
    return (PID);
}


void GPP_i::terminate (CF::ExecutableDevice::ProcessID_Type processId)
    throw (CORBA::SystemException, CF::ExecutableDevice::InvalidProcess,
    CF::Device::InvalidState)
{
    DEBUG(3, GPP, "Entering terminate.");
    
    // validate device state
    if (isLocked () || isDisabled ()) {
        DEBUG(1, GPP, "Cannot terminate. System is either LOCKED or DISABLED.");
        throw (CF::Device::InvalidState("Cannot terminate. System is either LOCKED or DISABLED."));
    }

    // release object method on the component causes the component
    // to exit. Catch SIGCHLD from the component here. Do not modify unless
    // you understand unix process handling.
    ///\todo Consider how to identify rogue processes and kill them
    int status;
    waitpid(processId, &status, 0);
    DEBUG(4, GPP, "waitpid returned status: " << status);
}

// Loadabledevice interface

void GPP_i::load (CF::FileSystem_ptr fs, const char *fileName, CF::LoadableDevice::LoadType loadKind)
throw (CORBA::SystemException, CF::Device::InvalidState,
CF::LoadableDevice::InvalidLoadKind, CF::InvalidFileName,
CF::LoadableDevice::LoadFail)
{
    DEBUG(3, GPP, "Loading: " << fileName);

    //CF::File_ptr fileToLoad;                      // file pointer
    
    // verify that the device is in a valid state for loading
    if (!isUnlocked () || isDisabled ()) {
        throw (CF::Device::InvalidState("Cannot load. System is either LOCKED, SHUTTING DOWN or DISABLED."));
    }
    
    // verify that the loadKind is supported (only executable is supported by this version)
    if (loadKind != CF::LoadableDevice::EXECUTABLE) {
        throw CF::LoadableDevice::InvalidLoadKind ();
    }
    
    // verify the file name exists in the file system and get a pointer to it
    
    if (!fs->exists ((char *) fileName))
        throw (CF::InvalidFileName (CF::CFENOENT, "Cannot load. File name is invalid."));
    
    if (incrementFile (fileName) == 1) { // First use of file update local cache
        CF::File_var execFile = fs->open(fileName, true);

	fs::path filePath = localFileRoot / fileName;
	
	std::fstream f;
	std::ios_base::openmode mode(std::ios::in|std::ios::out|std::ios::trunc);
	f.open(filePath.string().c_str(), mode);
        
	if (!f.is_open()) {
	    DEBUG(3, GPP, "File " << filePath.string() << " did not open succesfully.");
	    
	    return;
	}
        
	bool done(false);
	do {
	    CF::OctetSequence_var data;
	    execFile->read(data, 8192);
	    f.write((const char *)data->get_buffer(), data->length());
	    
	    done = (data->length() != 0);
	} while (done);
	
	///\todo Make permissions more os independent 
	// Set executable permissions 775 if necessary
	if (loadKind == CF::LoadableDevice::EXECUTABLE) {
	    mode_t mode = S_IRUSR|S_IWUSR|S_IXUSR|
		S_IRGRP|S_IWGRP|S_IXGRP|
		S_IROTH|S_IXOTH;
	    chmod(filePath.string().c_str(), mode);
	}
	
	f.close();

        execFile->close();
       
    }
}


void GPP_i::unload (const char *fileName)
throw (CORBA::SystemException, CF::Device::InvalidState, CF::InvalidFileName)
{
    DEBUG(3, GPP, "Unloading: " << fileName);

    // verify that the device is in a valid state for loading
    if (isLocked () || isDisabled ()) {
        throw (CF::Device::InvalidState("Cannot unload. System is either LOCKED or DISABLED."));
    }
    
    // decrement the list entry counter for this file
    decrementFile (fileName);
    
}

int GPP_i::incrementFile (const char *fileName)
{
    for (unsigned int i = 0; i < loadedFiles.size (); i++) {
        if (loadedFiles[i].fileId == fileName) {
            loadedFiles[i].counter++;
            return loadedFiles[i].counter++;
        }
    }
    //If the file is not loaded yet, then add a new element to loadedFiles
    fileCount tempNode;
    tempNode.fileId = fileName;
    tempNode.counter = 1;
    loadedFiles.push_back (tempNode);
    return 1;
}

int GPP_i::decrementFile (const char *fileName)
{
    for (unsigned int i = 0; i < loadedFiles.size (); i++) {
        if (loadedFiles[i].fileId == fileName) {
            if (--loadedFiles[i].counter == 0) {
                
                // unload fileName using API (device specific, not currently implemented)
                
                loadedFiles.erase (loadedFiles.begin () + i);
            }
            return loadedFiles[i].counter;
        }
    }
    
    //If it gets to this point the file was not loaded previously
    throw (CF::InvalidFileName (CF::CFENOENT, "Cannot unload. File was not already loaded."));
}


bool GPP_i::isFileLoaded (const char *fileName)
{
    for (unsigned int i = 0; i < loadedFiles.size (); i++) {
        if (loadedFiles[i].fileId == fileName) {
            return true;
        }
    }
    return false;
}

// Device methods

CF::Device::UsageType GPP_i::usageState() throw (CORBA::SystemException)
{
    return dev_usageState;
}

CF::Device::AdminType GPP_i::adminState() throw (CORBA::SystemException)
{
    return dev_adminState;
}

CF::Device::OperationalType GPP_i::operationalState() throw (CORBA::SystemException)
{
    return dev_operationalState;
}

CF::AggregateDevice_ptr GPP_i::compositeDevice() throw (CORBA::SystemException)
{
    return NULL;
}

void GPP_i::adminState (CF::Device::AdminType _adminType)
throw (CORBA::SystemException)
{
    dev_adminState = _adminType;
}

CORBA::Boolean GPP_i::allocateCapacity (const CF::
Properties & capacities)
throw (CORBA::SystemException, CF::Device::InvalidCapacity,
CF::Device::InvalidState)
{

    return true;
}

void GPP_i::deallocateCapacity (const CF::Properties & capacities)
throw (CORBA::SystemException, CF::Device::InvalidCapacity,
CF::Device::InvalidState)
{

}

char *GPP_i::label ()
throw (CORBA::SystemException)
{
    return CORBA::string_dup(dev_label.c_str());
}


char *GPP_i::softwareProfile ()
throw (CORBA::SystemException)
{
    return CORBA::string_dup(dev_profile.c_str());
}

// Resource methods

void GPP_i::start () throw (CORBA::SystemException, CF::Resource::StartError)
{
    dev_usageState = CF::Device::ACTIVE;
}


void GPP_i::stop () throw (CORBA::SystemException, CF::Resource::StopError)
{
    dev_usageState = CF::Device::IDLE;
}


char *GPP_i::identifier () throw (CORBA::SystemException)
{
    return CORBA::string_dup(dev_id.c_str());
}

bool GPP_i::isUnlocked ()
{
    if (dev_adminState == CF::Device::UNLOCKED)
        return true;
    else
        return false;
}


bool GPP_i::isLocked ()
{
    if (dev_adminState == CF::Device::LOCKED)
        return true;
    else
        return false;
}


bool GPP_i::isDisabled ()
{
    if (dev_operationalState == CF::Device::DISABLED)
        return true;
    else
        return false;

}


bool GPP_i::isBusy ()
{
    if (dev_usageState == CF::Device::BUSY)
        return true;
    else
        return false;

}

bool GPP_i::isIdle ()
{
    if (dev_usageState == CF::Device::IDLE)
        return true;
    else
        return false;

}

// LifeCyle methods

void GPP_i::initialize ()
throw (CORBA::SystemException, CF::LifeCycle::InitializeError)
{

}


void GPP_i::releaseObject ()
throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{

}

// TestableObject interfaces

void GPP_i::runTest (CORBA::ULong _number, CF::Properties & _props)
throw (CORBA::SystemException, CF::TestableObject::UnknownTest,
CF::UnknownProperties)
{

}

// PropertySet interfaces
void GPP_i::configure (const CF::Properties & configProperties)
throw (CORBA::SystemException, CF::PropertySet::InvalidConfiguration,
CF::PropertySet::PartialConfiguration)
{

}

void GPP_i::query (CF::Properties & configProperties)
throw (CORBA::SystemException, CF::UnknownProperties)
{

}

// PortSupplier interfaces

CORBA::Object *GPP_i::getPort(const char *_name)
    throw (CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    return NULL;
}
