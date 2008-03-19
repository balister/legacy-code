/****************************************************************************

Copyright 2004, 2007, 2008 Virginia Polytechnic Institute and State University

This file is part of the OSSIE Core Framework.

OSSIE Core Framework is free software; you can redistribute it and/or modify
it under the terms of the Lesser GNU General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

OSSIE Core Framework is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Lesser GNU General Public License for more details.

You should have received a copy of the Lesser GNU General Public License
along with OSSIE Core Framework; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/

#ifndef DEVICE_MANAGER_IMPL_H
#define DEVICE_MANAGER_IMPL_H

#include "ossieSupport.h"

#include "FileManager_impl.h"
#include "automaticTest.h"
#include "DCDParser.h"
#include "SPDParser.h"
#include "PRFParser.h"
#include "SPDImplementation.h"

class OSSIECF_API DeviceManager_impl: public virtual POA_CF::DeviceManager
{
public:
    DeviceManager_impl(const char *, const char *);

    // Run this after the constructor and the caller has created the object reference
    void post_constructor(CF::DeviceManager_var);
    ~DeviceManager_impl();

    char* deviceConfigurationProfile ()
        throw (CORBA::SystemException);

    CF::FileSystem_ptr fileSys()
        throw (CORBA::SystemException);

    char* identifier()
        throw (CORBA::SystemException);

    char* label()
        throw (CORBA::SystemException);

    void
        configure (const CF::Properties & configProperties)
        throw (CF::PropertySet::PartialConfiguration,
        CF::PropertySet::InvalidConfiguration, CORBA::SystemException);

    void
        query (CF::Properties & configProperties)
        throw (CF::UnknownProperties, CORBA::SystemException);

    CORBA::Object* getPort (const char *) throw (CF::PortSupplier::UnknownPort, CORBA::SystemException);

    CF::DeviceSequence* registeredDevices()
        throw (CORBA::SystemException);

    CF::DeviceManager::ServiceSequence* registeredServices()
        throw (CORBA::SystemException);

    void registerDevice(CF::Device_ptr registeringDevice)
        throw (CF::InvalidObjectReference, CORBA::SystemException);

    void unregisterDevice(CF::Device_ptr registeredDevice)
        throw (CF::InvalidObjectReference, CORBA::SystemException);

    void shutdown()
        throw (CORBA::SystemException);

    void registerService(CORBA::Object_ptr registeringService, const char *name)
        throw (CF::InvalidObjectReference, CORBA::SystemException);

    void unregisterService(CORBA::Object_ptr registeredService, const char *name)
        throw (CF::InvalidObjectReference, CORBA::SystemException);

    char* getComponentImplementationId(const char *componentInstantiationId)
        throw (CORBA::SystemException);

#ifdef AUTOMATIC_TEST
    void displayRegisteredDevices (void);
    void displayRegisteredServices (void);
#endif                                            /*  */

private:
    DeviceManager_impl();   // No default constructor
    DeviceManager_impl(DeviceManager_impl &); // No copying

// read only attributes
    std::string                           _identifier;
    std::string                           _label;
    std::string                           _deviceConfigurationProfile;
    
    std::string                           fsRoot;
    FileSystem_impl                       *fs_servant;
    CF::FileSystem_var                    fileSystem;
    CF::FileManager_var                   fileMgr;
    CF::DeviceManager_var                 myObj;
    CF::DeviceSequence                    _registeredDevices;
    CF::DeviceManager::ServiceSequence    _registeredServices;
    
    ossieSupport::ORB                     *orb_obj;

    enum DevMgrAdmnType
    {
        DEVMGR_REGISTERED,
        DEVMGR_UNREGISTERED,
        DEVMGR_SHUTTING_DOWN
    };

    DevMgrAdmnType                        _adminState;
    CF::DomainManager_var                 _dmnMgr;

    bool deviceIsRegistered(CF::Device_ptr);
    bool serviceIsRegistered(const char*);
    void getDomainManagerReference(char *);
    void init();

};

#endif                                            /* __DEVICEMANAGER_IMPL__ */
