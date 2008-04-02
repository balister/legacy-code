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

#include <string>

#include <boost/filesystem/path.hpp>

#include "ossie/cf.h"

class fileCount
{
  public:
    std::string fileId;
    int counter;
};


class GPP_i : public virtual POA_CF::ExecutableDevice

{
  public:
    GPP_i(char *id, char *label, char *profile);

    // Executable device methods
    CF::ExecutableDevice::ProcessID_Type execute (const char *name,
         const CF::Properties & options,
         const CF::Properties & parameters) throw (
         CF::ExecutableDevice::ExecuteFail,
         CF::InvalidFileName, CF::ExecutableDevice::InvalidOptions,
         CF::ExecutableDevice::InvalidParameters,
         CF::ExecutableDevice::InvalidFunction,
         CF::Device::InvalidState,
         CORBA::SystemException);
    
    void terminate (CF::ExecutableDevice::ProcessID_Type processId) throw
      (CF::Device::InvalidState, CF::ExecutableDevice::InvalidProcess,
       CORBA::SystemException);
    
    // Loadable device methods
    void load (CF::FileSystem_ptr fs, const char *fileName,
               CF::LoadableDevice::LoadType loadKind)
      throw (CF::LoadableDevice::LoadFail, CF::InvalidFileName,
             CF::LoadableDevice::InvalidLoadKind, CF::Device::InvalidState,
	     CORBA::SystemException);

    void unload (const char *fileName)
      throw (CF::InvalidFileName, CF::Device::InvalidState,
             CORBA::SystemException);
    
    // Device methods
    CF::Device::UsageType usageState ()
        throw (CORBA::SystemException);
    CF::Device::AdminType adminState ()
        throw (CORBA::SystemException);
    CF::Device::OperationalType operationalState ()
        throw (CORBA::SystemException);
    CF::AggregateDevice_ptr compositeDevice ()
        throw (CORBA::SystemException);
    void adminState (CF::Device::AdminType _adminType)
        throw (CORBA::SystemException);
    void deallocateCapacity (const CF::Properties & capacities) 
        throw (CF::Device::InvalidState, CF::Device::InvalidCapacity,
               CORBA::SystemException);
    CORBA::Boolean allocateCapacity (const CF::Properties & capacities)
        throw (CF::Device::InvalidState, CF::Device::InvalidCapacity,
               CORBA::SystemException);
    
    char *label () throw (CORBA::SystemException);
    char *softwareProfile () throw (CORBA::SystemException);


    // Resource methods
    void start () throw (CF::Resource::StartError, CORBA::SystemException);
    void stop () throw (CF::Resource::StopError, CORBA::SystemException);

    char *identifier () throw (CORBA::SystemException);

    // LifeCyle Interfaces
    void initialize () throw (CF::LifeCycle::InitializeError, CORBA::SystemException);
    void releaseObject () throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);

    // TestableObject Interfaces
    void runTest (CORBA::ULong TestID, CF::Properties & testValues)
        throw (CF::UnknownProperties, CF::TestableObject::UnknownTest,
        CORBA::SystemException);

    // PropertySet interfaces
    void configure (const CF::Properties & configProperties)
        throw (CF::PropertySet::PartialConfiguration,
        CF::PropertySet::InvalidConfiguration, CORBA::SystemException);

    void query (CF::Properties & configProperties)
        throw (CF::UnknownProperties, CORBA::SystemException);

    // Port Supplier interfaces
    CORBA::Object* getPort (const char *) throw (CF::PortSupplier::UnknownPort, 
CORBA::SystemException);


  private:
    GPP_i();
    GPP_i(const GPP_i &);

    std::string dev_id, dev_label, dev_profile;
    boost::filesystem::path localFileRoot;
    std::vector <fileCount> loadedFiles;
    int incrementFile (const char *);
    int decrementFile (const char *);

    bool isUnlocked();
    bool isLocked();
    bool isDisabled();
    bool isBusy();
    bool isIdle();
    bool isFileLoaded(const char* fname);

    CF::Device::AdminType dev_adminState;
    CF::Device::UsageType dev_usageState;
    CF::Device::OperationalType dev_operationalState;

};
