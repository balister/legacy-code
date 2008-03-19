/****************************************************************************

Copyright 2004, 2007, 2008 Virginia Polytechnic Institute and State University

This file is part of the OSSIE Core Framework.

OSSIE Core Framework is free software; you can redistribute it and/or modify
it under the terms of the Lesser GNU General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version. 

OSSIE Core Framework is distributed in the hope that it will be useful
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Lesser GNU General Public License for more details.

You should have received a copy of the Lesser GNU General Public License
along with OSSIE Core Framework; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/

/* SCA */
/* Include files */

#include <iostream>
#include <string>
#include <vector>

#include <string.h>

#ifdef HAVE_OMNIEVENTS
#include "omniEvents.hh"
#endif

#include "ossie/StandardEvent.h"    // from idl

#include "ossie/debug.h"
#include "ossie/DomainManager_impl.h"
#include "ossie/FileSystem_impl.h"
#include "ossie/ossieSupport.h"

DomainManager_impl::DomainManager_impl (const char *_dmdFile, const char *rootPath)
{

    _domainManagerProfile = _dmdFile;

    DEBUG(9, DomainManger, "In constructor with _domainManagerProfile " << _domainManagerProfile);

    _numApps = 0;
    _numDevMgrs = 0;
    _applications.length (_numApps);
    _deviceManagers.length (_numDevMgrs);
    
    // Create file manager
    FileManager_impl *_fileMgrImpl = new FileManager_impl ();
    _fileMgr = _fileMgrImpl->_this ();

    // Create root file system and mount on File Manager
    FileSystem_impl *fileSys_serv = new FileSystem_impl (rootPath);
    fileSys = fileSys_serv->_this();

    _fileMgr->mount ("/", fileSys);

    CF::File_var file = _fileMgr->open(_domainManagerProfile.c_str(), true);
    DMDParser _DMDParser (file);

    file->close();

    this->_identifier = _DMDParser.getID();

/// \todo lookup and install any services specified in the DMD

/// \todo Discuss using filename + path directly from xml and screwing with FileSystem

#if 0
    SPDParser _DM_SPD_Parser (_DMDParser.getDomainManagerSoftPkg());
#endif

/*******************************************************

At some point, memory has to be added to the system,
where the domainmanager recalls the last configuration

*******************************************************/

//      create Incoming Domain Management and Outgoing Domain Management event channels
    createEventChannels ();

    // Create the event channel conusmer object to send events to
#ifdef HAVE_OMNIEVENTS
    ODM_Channel_consumer = connectEventChannel(ODM_channel);
#endif
    DEBUG(9, DomainManger, "Leaving constructor with _domainManagerProfile " << _domainManagerProfile);
}


DomainManager_impl::~DomainManager_impl ()
{
  /// \todo uncomment    destroyEventChannels ();

    DEBUG(1, DomMgr, "Domain Manager destructor called")

/**************************************************
 *    Save current state for configuration recall   *
 *    this is not supported by this version          *
 **************************************************/

    unsigned int i;
    for (i=0; i<_pendingConnections.size(); i++) {
        delete _pendingConnections[i];
    }

    for (i=0; i<_registeredDevices.size();i++) {
        delete _registeredDevices[i];
    }

    for (i=0; i<_registeredServices.size();i++) {
        delete _registeredServices[i];
    }
}

char *
DomainManager_impl::identifier (void)
throw (CORBA::SystemException)
{
    DEBUG(6, DomainManager, "Entering identifier.")
    return CORBA::string_dup(_identifier.c_str());
}

void DomainManager_impl::configure (const CF::Properties & configProperties) throw (CF::PropertySet::PartialConfiguration, CF::PropertySet::InvalidConfiguration, CORBA::SystemException)
{
}

void DomainManager_impl::query (CF::Properties & configProperties) throw (CF::UnknownProperties, CORBA::SystemException)
{
}

char *
DomainManager_impl::domainManagerProfile (void)
throw (CORBA::SystemException)
{
    DEBUG(6, DomainManager, "Entering domainManagerProfile with _domainManagerProfile " << _domainManagerProfile);

    std::string result;
    ossieSupport::createProfileFromFileName(_domainManagerProfile, result);
    return CORBA::string_dup(result.c_str());
}


CF::FileManager_ptr DomainManager_impl::fileMgr (void) throw (CORBA::
SystemException)
{
    DEBUG(6, DomainManager, "Entering fileMgr.")
    CF::FileManager_var result = CF::FileManager::_duplicate(_fileMgr);
    return result._retn();
}


CF::DomainManager::ApplicationFactorySequence *
DomainManager_impl::applicationFactories (void) throw (CORBA::
SystemException)
{
    DEBUG(6, DomainManager, "Entering applicationFactories.");
    CF::DomainManager::ApplicationFactorySequence_var result = 
        new CF::DomainManager::ApplicationFactorySequence(_applicationFactories);

    return result._retn();
}


CF::DomainManager::ApplicationSequence *
DomainManager_impl::applications (void) throw (CORBA::SystemException)
{
    DEBUG(6, DomainManager, "Entering applicationSequence.");
    CF::DomainManager::ApplicationSequence_var result = 
        new CF::DomainManager::ApplicationSequence(_applications);
    return result._retn();
}


CF::DomainManager::DeviceManagerSequence *
DomainManager_impl::deviceManagers (void) throw (CORBA::SystemException)
{
    DEBUG(6, DomainManager, "Entering deviceManagers.");
    CF::DomainManager::DeviceManagerSequence_var result = 
        new CF::DomainManager::DeviceManagerSequence(_deviceManagers);
    return result._retn();
}


void
DomainManager_impl::registerDeviceManager (CF::DeviceManager_ptr deviceMgr)
throw (CORBA::SystemException, CF::InvalidObjectReference, CF::InvalidProfile,
CF::DomainManager::RegisterError)
{
    DEBUG(6, DomainManager, "Entering registerDeviceManager.");
    if (CORBA::is_nil (deviceMgr))
    {
//writeLogRecord(FAILURE_ALARM,invalid reference input parameter.);

        throw (CF::
            InvalidObjectReference
            ("Cannot register Device. DeviceMgr is a nil reference."));
    }

//      add device manager to list

    addDeviceMgr (CF::DeviceManager::_duplicate (deviceMgr));

    ossieSupport::sendObjAdded_event(_identifier.c_str(), 
                                     deviceMgr->identifier(), 
                                     deviceMgr->label(), 
                                     (CORBA::Object_var) NULL, 
                                     StandardEvent::DEVICE_MANAGER);

//      mount filesystem under "/DomainName1/<deviceMgr.label>"

    std::string mountPoint = "/";
    mountPoint += deviceMgr->label();

    _fileMgr->mount (mountPoint.c_str(), deviceMgr->fileSys ());

//      add all devices under device manager to registereddevice att.
//      associate input deviceMgr with registeredDevices
    addDeviceMgrDevices (deviceMgr);

//      add all services under device manager to registeredservices
//      associate input deviceMgr with registeredservices
    addDeviceMgrServices (deviceMgr);

//NOTE: The SCA V2.2 describes that all service connections should be established at this point.
//That step has been performed by the registerDevice operation (called by addDeviceMgrDevices) and by
//registerService ( called by addDeviceMgrServices).
//The registerDevice function establish all the connections requiered for the registering device and stores
//any connection when the requested service is not registered yet.
//The registerService function will establish any pending connection with the registering service.
//These two functions together will establish all service connections required.

//Note: In the event of an internal error that prevents the device Mgr registration from success, the
//RegisterError exception shuld be raised and a FAILURE_ALARM log record written to a DomainManagerÂs Log
}


void
DomainManager_impl::addDeviceMgr (CF::DeviceManager_ptr deviceMgr)
{
    DEBUG(6, DomainManager, "Entering addDeviceMgr.");

    if (!deviceMgrIsRegistered (deviceMgr))
    {
        _deviceManagers.length (_deviceManagers.length () + 1);
        _deviceManagers[_deviceManagers.length () - 1] = deviceMgr;
    }
}


void
DomainManager_impl::addDeviceMgrDevices (CF::DeviceManager_ptr deviceMgr)
{
    DEBUG(6, DomainManager, "Entering addDeviceMgrDevices.");

    CF::DeviceSequence * devices;
    devices = deviceMgr->registeredDevices ();

//Call registerDevice for each device in the DeviceMgr
    for (unsigned int i = 0; i < devices->length (); i++)
    {
        CF::Device_ptr _dev = (*devices)[i];      //(CF::Device_ptr)&devices[i];
        registerDevice (_dev, deviceMgr);
    }
//The registerDevice operation will try to establish any service connections specified in the
//deviceMgr's DCD for each device.
}


void
DomainManager_impl::addDeviceMgrServices (CF::DeviceManager_ptr deviceMgr)
{
    DEBUG(6, DomainManager, "Entering addDeviceMgrServices.");

    CF::DeviceManager::ServiceSequence *services;
    services = deviceMgr->registeredServices ();

//Call registerService for each service in the DeviceMgr
    for (unsigned int i = 0; i < services->length (); i++ )
    {
//(CF::Device_ptr)&devices[i];
        CF::DeviceManager::ServiceType _serv = (*services)[i];
        registerService ( _serv.serviceObject, deviceMgr, _serv.serviceName);
    }

//The registerDeviceManager operation shall add the input deviceMgrÂs registeredServices and
//each registeredServiceÂs names to the DomainManager. The registerDeviceManager operation
//associates the input deviceMgrÂs with the input deviceMgrÂs registeredServices in the
//DomainManager in order to support the unregisterDeviceManager operation.

//Note: The registerService operation will establish any pending connection for the registering service
}


void
DomainManager_impl::unregisterDeviceManager (CF::DeviceManager_ptr deviceMgr)
throw (CORBA::SystemException, CF::InvalidObjectReference,
CF::DomainManager::UnregisterError)
{
    DEBUG(6, DomainManager, "Entering unRegisterDeviceManager.");

//The unregisterDeviceManager operation shall raise the CF InvalidObjectReference when the
//input parameter DeviceManager contains an invalid reference to a DeviceManager interface.
//      make sure that the pointer is valid
    if (CORBA::is_nil (deviceMgr))
    {
/*writeLogRecord(FAILURE_ALARM,invalid reference input parameter.); */

        throw (CF::
            InvalidObjectReference
            ("Cannot unregister Device. DeviceMgr is a nil reference."));
        return;
    }

//NOTE: All disconnections of service event channels are performed by unregisterDevice
//(called by removeDeviceMgrDevices)

//release all device(s) and service(s)
    removeDeviceMgrDevices (deviceMgr);

    removeDeviceMgrServices (deviceMgr);

//The unregisterDeviceManager operation shall unmount all DeviceManagerÂs FileSystems from
//its File Manager.
//Unmount filesystem from "/DomainName1/<deviceMgr.label>"
#if 0 ///\todo Fix this code so it figures out what file systems where really mounted
    std::string mountPoint = "/";
    mountPoint += _identifier;
    mountPoint += "/";
    mountPoint += deviceMgr->label();

    _fileMgr->unmount (mountPoint.c_str());
#endif

//The unregisterDeviceManager operation shall unregister a DeviceManager component from the
//DomainManager.
    removeDeviceMgr (deviceMgr);

//Write log record upon successful registration of Device Manager
/*writeLogRecord(ADMINISTRATIVE_EVENT, successful unregistration of DeviceManager); */

//The unregisterDeviceManager operation shall, upon successful unregistration, send an event to
//the Outgoing Domain Management event channel with event data consisting of a
//DomainManagementObjectRemovedEventType. The event data will be populated as follows:
//      1. The producerId shall be the identifier attribute of the DomainManager.
//      2. The sourceId shall be the identifier attribute of the unregistered DeviceManager.
//      3. The sourceName shall be the label attribute of the unregistered DeviceManager.
//      4. The sourceCategory shall be DEVICE_MANAGER.


/// \todo Fix event sending
//    StandardEvent::DomainManagementObjectRemovedEventType _objRemovedEvent;

//    _objRemovedEvent.producerId = CORBA::string_dup (this->_identifier);

//    _objRemovedEvent.sourceId = CORBA::string_dup (deviceMgr->identifier ());

//    _objRemovedEvent.sourceName = CORBA::string_dup (deviceMgr->label ());

//    _objRemovedEvent.sourceCategory = StandardEvent::DEVICE_MANAGER;

//    CORBA::Any _anyObjRemovedEvent;

//    _anyObjRemovedEvent <<= _objRemovedEvent;

//    sendEventToOutgoingChannel (_anyObjRemovedEvent);

//Note: In the event of an internal error that prevents the deviceMgr unregistration from success, the
//RegisterError exception shuld be raised and a FAILURE_ALARM log record written to a DomainManagerÂs Log
}


void
DomainManager_impl::removeDeviceMgrDevices (CF::DeviceManager_ptr deviceMgr)
{
    DEBUG(6, DomainManager, "Entering removeDeviceMgrDevices.");

//The unregisterDeviceManager operation shall release all device(s) associated with
//the DeviceManager that is being unregistered.
    CF::DeviceSequence * devices;
    devices = deviceMgr->registeredDevices ();

//Call unregisterDevice for each device in the DeviceMgr
    for (unsigned int i = 0; i < devices->length (); i++)
    {
        CF::Device_ptr _dev = (*devices)[i];
        unregisterDevice (_dev);
    }

}


void
DomainManager_impl::removeDeviceMgrServices (CF::DeviceManager_ptr deviceMgr)
{
    DEBUG(6, DomainManager, "Entering removeDeviceMgrServices.");

    return; ///\todo Deal with services
//The unregisterDeviceManager operation shall release all service(s) associated with
//the DeviceManager that is being unregistered.
    CF::DeviceManager::ServiceSequence* services;
    services = deviceMgr->registeredServices ();

//Call unregisterDevice for each device in the DeviceMgr
    for (unsigned int i = 0; i < services->length (); i++)
    {
        unregisterService ( (*services)[i].serviceObject, (*services)[i].serviceName );
    }

}


void
DomainManager_impl::removeDeviceMgr (CF::DeviceManager_ptr deviceMgr)
{
    DEBUG(6, DomainManager, "Entering removeDeviceMgr.");

//Look for registeredDeviceMgr in _deviceManagers
//if deviceMgr is not registered, do nothing
    for (unsigned int i = 0; i < _deviceManagers.length (); i++) {
        if (strcmp (_deviceManagers[i]->label (), deviceMgr->label ()) == 0) {
//_deviceManagers.remove(i);
            for (unsigned int j = i; j < _deviceManagers.length () - 1; j++) {
                _deviceManagers[j] = _deviceManagers[j + 1];
            }
            _deviceManagers.length (_deviceManagers.length () - 1);
            break;
        }
    }
}


void
DomainManager_impl::registerDevice (CF::Device_ptr registeringDevice,
    CF::DeviceManager_ptr registeredDeviceMgr)
    throw (CORBA::SystemException, CF::InvalidObjectReference,
           CF::InvalidProfile,
           CF::DomainManager::DeviceManagerNotRegistered,
           CF::DomainManager::RegisterError)
{
    DEBUG(6, DomainManager, "Entering registerDevice.");

//Verify they are not a nil reference
    if (CORBA::is_nil (registeringDevice)
        || CORBA::is_nil (registeredDeviceMgr)) {
	DEBUG(1, DomainManager, "Bad device passed to registerDevice.");
/*writeLogRecord(FAILURE_ALARM,invalid reference input parameter.); */
        throw (CF::InvalidObjectReference("Cannot register Device. Either Device or DeviceMgr is a nil reference."));
    }

//Verify that input is a registered DeviceManager
    if (!deviceMgrIsRegistered (registeredDeviceMgr))
    {
/*writeLogRecord(FAILURE_ALARM,the device could not register because the DeviceManager is not registered with the DomainManager.); */

        throw CF::DomainManager::DeviceManagerNotRegistered ();
        return;
    }

//Add registeringDevice and its attributes to domain manager
    if (storeDeviceInDomainMgr (registeringDevice, registeredDeviceMgr) !=
        NULL_ERROR)
    {
/*writeLogRecord(FAILURE_ALARM,the registeringDevice has an invalid profile.); */
//The registerDevice operation shall raise the CF InvalidProfile exception when:
//      1. The Device's SPD file and the SPDÂs referenced files do not exist or cannot be processed
//      due to the file not being compliant with XML syntax, or
//      2. The DeviceÂs SPD does not reference allocation properties.
//              throw( CF::InvalidProfile() );
        throw CF::InvalidProfile ();
        return;
    }

//Check the DCD for connections and establish them
    establishServiceConnections(registeringDevice, registeredDeviceMgr);

    ossieSupport::sendObjAdded_event(_identifier.c_str(), registeringDevice->identifier(), registeringDevice->label(), (CORBA::Object_var) NULL, StandardEvent::DEVICE);

//write a log record when registration is successful
/*writeLogRecord(ADMINISTRATIVE_EVENT,the device has successfully registered with the DomainManager); */

//NOTE: This function only checks that the input references are valid and the device manager is registered.
//No other sanity test is performed. In the event of any internal error that impedes a succesful registration,
// a FAILURE_ALARM log record should be written and the RegisterError exception should be raised
}


//This function adds the registeringDevice and its atributes to the DomainMgr.
//if the device already exists it does nothing
DomainManager_impl::ErrorCode 
DomainManager_impl::storeDeviceInDomainMgr (CF::Device_ptr registeringDevice,
    CF::DeviceManager_ptr registeredDeviceMgr)
{
    DEBUG(6, DomainManager, "Entering storeDeviceinDomainMgr.");

    ErrorCode parserResult;

//check if device is already registered
    if (deviceIsRegistered (registeringDevice))
    {
        return NULL_ERROR;                        //if registeringDevice exists already, do nothing
    }
//If this part is reached, the registering device has to be added
//Get read-only attributes from registeringDevice
    DeviceNode* newDeviceNode = new DeviceNode;

    newDeviceNode->devicePtr = registeringDevice;
    newDeviceNode->devMgrPtr = registeredDeviceMgr;
    newDeviceNode->label = CORBA::string_dup (registeringDevice->label ());
    newDeviceNode->softwareProfile = 
        CORBA::string_dup (registeringDevice->softwareProfile ());

//parse and get device properties from SPD
    parserResult = getDeviceProperties (*newDeviceNode);
    if (parserResult != NULL_ERROR) {
//There was an error
        return parserResult;
    }
    else {
//add registeringDevice to DomainManager's registeredDevices
        _registeredDevices.push_back (newDeviceNode);
        return NULL_ERROR;
    }
}


DomainManager_impl::ErrorCode DomainManager_impl::getDeviceProperties(DeviceNode& registeringDeviceNode)
{
    DEBUG(6, DomainManager, "Entering getDeviceProperties.");

/*PRFProperty parsedProperties;
   //parse SPD
   SPDParser SPDFile( registeringDeviceNode.softwareProfile );

   if( SPDFile.isScaCompliant() )
   {
   //Parse PRF file to get device's properties
   PRFParser PRFFile( SPDFile.getPRFFile() );
   parsedProperties = PRFFile.getProperties();
   //store properties in registeringDeviceNode
   CORBA::Any anyVar;
for(int i = 0; i < parsedProperties.size(); i++)
{
registeringDeviceNode.properties[i].id = CORBA::string_dup( parsedProperties[i]->getID() );
ORB_WRAP::anyType( parsedProperties[i]->getType(), parsedProperties[i]->getValue(), anyVar );
registeringDeviceNode.properties[i].value = anyVar;
}
//return result
return NULL_ERROR;
}
else
{
return NOT_SCA_COMPLIANT;
} */
    return NULL_ERROR;
}


//This function adds the registeringService and its name to the DomainMgr.
//if the service already exists it does nothing
void
DomainManager_impl::storeServiceInDomainMgr (CORBA::Object_ptr registeringService,
    CF::DeviceManager_ptr registeredDeviceMgr,const char * name)
{
    DEBUG(6, DomainManager, "Entering storeServiceInDomainMgr.");
//check if device is already registered
    if (serviceIsRegistered (name))
    {
        return ;                                  //if registeringDevice exists already, do nothing
    }
//If this part is reached, the registering device has to be added
//Get read-only attributes from registeringDevice
    ServiceNode* newServiceNode = new ServiceNode;

    newServiceNode->objectPtr = registeringService;
    newServiceNode->devMgrPtr = registeredDeviceMgr;
    newServiceNode->name = CORBA::string_dup (name);

//add registeringService to DomainManager's registeredServices
    _registeredServices.push_back (newServiceNode);
}


//This function removes the registeringService and its name to the DomainMgr.
//if the service already exists it does nothing
void
DomainManager_impl::removeServiceFromDomainMgr (CORBA::Object_ptr registeringService, const char * name)
{
    DEBUG(6, DomainManager, "Entering removeServiceFromDomainMgr.");

    std::vector<ServiceNode*>::iterator p = _registeredServices.begin ();
//check if service is already registered
    while (p != _registeredServices.end ())
    {
        ServiceNode* _tmp = *p;
        if (strcmp (name, _tmp->name) == 0)
        {
            _registeredServices.erase (p);
            return;                               //unregisteringDevice has been removed
        }
        else
        {
            p++;
        }
    }
//If this part is reached, unregisteringDevice was not registered
    return;

}


void
DomainManager_impl::unregisterDevice (CF::Device_ptr unregisteringDevice)
    throw (CORBA::SystemException, CF::InvalidObjectReference,
           CF::DomainManager::UnregisterError)
{
    DEBUG(3, DomainManager, "Entering unregisterDevice.");

    if (CORBA::is_nil (unregisteringDevice)
        || !deviceIsRegistered (unregisteringDevice)) {
	DEBUG(1, DomainManager, "Attempt to unregister invalid device.");
/*writeLogRecord(FAILURE_ALARM,Device reference is not valid); */

        throw (CF::InvalidObjectReference("Cannot Unregister Device. Invalid reference"));
    }

    DEBUG(6, DomainManager, "About to remove device.");

    removeDeviceFromDomainMgr (unregisteringDevice);

/*writeLogRecord(ADMINISTRATIVE_EVENT, Device was successfuly unregistered); */

/// \todo Fix event sending

//    StandardEvent::DomainManagementObjectRemovedEventType _objRemovedEvent;

//    _objRemovedEvent.producerId = CORBA::string_dup (this->_identifier);

//_objRemovedEvent.sourceId = CORBA::string_dup( unregisteringDevice->identifier() );//Devices don't have identifiers

//    _objRemovedEvent.sourceName =
//        CORBA::string_dup (unregisteringDevice->label ());

//    _objRemovedEvent.sourceCategory = StandardEvent::DEVICE;

//    CORBA::Any _anyObjRemovedEvent;

//    _anyObjRemovedEvent <<= _objRemovedEvent;

//    sendEventToOutgoingChannel (_anyObjRemovedEvent);

}


//This function removes the registeringDevice and its atributes from the DomainMgr.
//if the device doesn't exist, it does nothing
void
DomainManager_impl::removeDeviceFromDomainMgr (CF::Device_ptr unregisteringDevice)
{
    DEBUG(6, DomainManager, "Entering removeDeviceFromDomainMgr.");

    std::vector < DeviceNode * >::iterator p = _registeredDevices.begin ();
//check if device is already registered
    while (p != _registeredDevices.end ())
    {
        DeviceNode *_tmp = *p;
        if (!strcmp (unregisteringDevice->label (), _tmp->devicePtr->label ()))
        {
            _registeredDevices.erase (p);
            return;                               //unregisteringDevice has been removed
        }
        else
        {
            p++;
        }
    }

//If this part is reached, unregisteringDevice was not registered
    return;

}


//This function returns TRUE if the input registeredDevice is contained in the _registeredDevices
bool
DomainManager_impl::deviceIsRegistered (CF::Device_ptr registeredDevice)
{
    DEBUG(6, DomainManager, "Entering deviceIsRegistered.");

    for (unsigned int i = 0; i < _registeredDevices.size (); i++) {
	DEBUG(9, DomainManager, "Comparing " << registeredDevice->label());
	DEBUG(9, DomainManager, "With " << _registeredDevices[i]->devicePtr->label());
        if (strcmp(_registeredDevices[i]->devicePtr->label(), registeredDevice->label()) == 0) {
	    DEBUG(9, DomainManager, "Found device in deviceIsRegistered.");
            return true;
        }
    }

    DEBUG(9, DomainManager, "Device not found in deviceIsRegistered.");
    return false;
}


bool
DomainManager_impl::serviceIsRegistered (const char *serviceName)
{
    DEBUG(6, DomainManager, "Entering ServiceIsRegistered.");

    for (unsigned int i = 0; i < _registeredServices.size (); i++)
    {
        if (strcmp (_registeredServices[i]->name, serviceName) == 0)
        {
            return true;
        }
    }
    return false;
}


//This function returns TRUE if the input registeredDeviceMgr is contained in the _deviceManagers list attribute
bool
DomainManager_impl::deviceMgrIsRegistered (CF::DeviceManager_ptr registeredDeviceMgr)
{
    DEBUG(6, DomainManager, "Entering deviceMgrIsRegistered.");

//Look for registeredDeviceMgr in _deviceManagers
    for (unsigned int i = 0; i < _deviceManagers.length (); i++)
    {
        if (strcmp
            (_deviceManagers[i]->identifier (),
            registeredDeviceMgr->identifier ()) == 0)
        {
            return true;
        }
    }
    return false;
}


void
DomainManager_impl::establishServiceConnections(
    CF::Device_ptr registeringDevice, CF::DeviceManager_ptr registeredDeviceMgr)
{

// The registerDeviceManager operation shall perform the connections specified 
// in the connections element of the deviceMgrÂs Device Configuration Descriptor
// (DCD) file. If the DeviceManager DCD describes a connection for a service
// that has not been registered with the DomainManager, the registerDeviceManager
// operation shall establish any pending connection when the service registers
// with the DomainManager by the registerDeviceManager operation.

    DEBUG(6, DomainManager, "Entering establishServiceConnections.");

    // Obtain DCD profile and DeviceManager file system
    const char* dcdProfile = registeredDeviceMgr->deviceConfigurationProfile();
    CF::FileSystem_var devManagerFileSys= registeredDeviceMgr->fileSys();
    
    // Parse filename from DCD profile     
    std::string fileName = ossieSupport::getFileNameFromProfile(dcdProfile);
    if (fileName.empty() ) {
        DEBUG(3, DomainManager, "Error parsing DCD file profile");
        return;
    }
 
    // Open and parse DCD
    CF::File_var file;
    try {
        file = devManagerFileSys->open(fileName.c_str(), true);
    } catch (CF::InvalidFileName) {
        DEBUG(3, DomainManager, "Error opening DCD file");
        return;
    } catch (CF::FileException) {
        DEBUG(3, DomainManager, "Error opening DCD file");
        return;
    }

    DCDParser dcdParser(file);
    std::vector<Connection*>* connections = dcdParser.getConnections();

    enum UType {U_CIR, U_DTLTC, U_DUBTC, U_FB};
    enum PType {P_PP, P_CSI, P_FB};
   
    int uType;
    int pType;
    const char* uFindByName;
    const char* pFindByName;
    const char* uPortName;


    // Iterate through connections
    std::vector<Connection*>::iterator i = connections->begin();

    while (i != connections->end() ) {
        UsesPort* uPortParser = (*i)->getUsesPort();
        if (uPortParser->isComponentInstantiationRef()) {
            DEBUG(3, DomainManager, "Uses port not supported");
            return;
        } else if (uPortParser->isDeviceThatLoadedThisComponentRef()) {
            DEBUG(3, DomainManager, "Uses port not supported");
            return;
        } else if (uPortParser->isDeviceUsedByThisComponentRef()) {
            DEBUG(3, DomainManager, "Uses port not supported");
            return;
        } else if (uPortParser->isFindBy()) {
            DEBUG(3, DomainManager, "Uses port is findby");
            uFindByName =
                uPortParser->getFindBy()->getFindByNamingServiceName();
            uPortName = uPortParser->getID();
            uType = U_FB;
        } else {
            DEBUG(3, DomainManager, "Uses port invalid");
            return;
        }

        if ((*i)->isFindBy()) {
            DEBUG(3, DomainManager, "Connect interface is findby");
            pFindByName = (*i)->getFindBy()->getFindByNamingServiceName();
            pType = P_FB;
        } else if ((*i)->isProvidesPort()) {
            DEBUG(3, DomainManager, "Connect interface not supported");
            return;
        } else if ((*i)->isComponentSupportedInterface()) {
            DEBUG(3, DomainManager, "Connect interface not supported");
            return;
        } else {
            DEBUG(3, DomainManager, "Connect interface invalid");
            return;
        }

        if (uType == U_FB && pType == P_FB) {

            // 
            // Verify that connection can be made
            //
            bool devMatch = false;
            bool svcMatch = false;

            if (!strcmp(uFindByName, registeringDevice->label() ) ) {
                DEBUG(3, DomainManager, "Device match in DCD");
                devMatch = true;
            } 

            for (unsigned int j = 0; j < _registeredServices.size(); j++) {
                if (!strcmp(pFindByName, _registeredServices[j]->name) ) {
                    DEBUG(3, DomainManager, "Service match in DCD");
                    svcMatch = true;
                    break;
                }
            }
         
            if (devMatch && svcMatch) {
                //
                // Make connection
                //
            } else {
                DEBUG(3, DomainManager, "Connection cannot be established");
                DEBUG(3, DomainManager, "Adding pending connection"); 
                _pendingConnections.push_back(new Connection( **i ));
            }
        }

        // Increase iterator
        i++;
    }
}


void
DomainManager_impl::establishPendingServiceConnections (const char * serviceName)
{
    DEBUG(6, DomainManager, "Entering establishPendingServiceConnections.");
#if 0
    enum UType {U_CIR, U_DTLTC, U_DUBTC, U_FB};
    
    enum PType {P_PP, P_CSI, P_FB};

    int uType;
    int pType;
    const char* uFindByName;
    const char* pFindByName;
    const char* uPortName;

    // Iterate through connections
    std::vector<Connection*>::iterator i = _pendingConnections.begin();

    while (i != _pendingConnections.end() ) {
        UsesPort* uPortParser = (*i)->getUsesPort();
        if (uPortParser->isComponentInstantiationRef()) {
            DEBUG(3, DomainManager, "Uses port not supported");
            return;
        } else if (uPortParser->isDeviceThatLoadedThisComponentRef()) {
            DEBUG(3, DomainManager, "Uses port not supported");
            return;
        } else if (uPortParser->isDeviceUsedByThisComponentRef()) {
            DEBUG(3, DomainManager, "Uses port not supported");
            return;
        } else if (uPortParser->isFindBy()) {
            DEBUG(3, DomainManager, "Uses port is findby");
            uFindByName =
                uPortParser->getFindBy()->getFindByNamingServiceName();
            uPortName = uPortParser->getID();
            uType = U_FB;
        } else {
            DEBUG(3, DomainManager, "Uses port invalid");
            return;
        }

        if ((*i)->isFindBy()) {
            DEBUG(3, DomainManager, "Connect interface is findby");
            pFindByName = (*i)->getFindBy()->getFindByNamingServiceName();
            pType = P_FB;
        } else if ((*i)->isProvidesPort()) {
            DEBUG(3, DomainManager, "Connect interface not supported");
            return;
        } else if ((*i)->isComponentSupportedInterface()) {
            DEBUG(3, DomainManager, "Connect interface not supported");
            return;
        } else {
            DEBUG(3, DomainManager, "Connect interface invalid");
            return;
        }
  
        
        if (uType == U_FB && pType == P_FB) {
            // 
            // Check connection
            //
            bool devMatch = false;
            bool svcMatch = false;

            for (unsigned int j; j < _registeredDevices.size(); j++) {
                if (!strcmp(uFindByName, _registeredDevices[j]->label)) {
                    DEBUG(3, DomainManager, "Device match in DCD");
                    devMatch = true;
                    break;
                }
            }
 
            if (!strcmp(pFindByName, serviceName) ) {
                DEBUG(3, DomainManager, "Device match in DCD");
                svcMatch = true;
            } 

            if (devMatch && svcMatch) {
                //
                // Make connection
                //
 
                //
                // Temporary until I look at built in ORB support
                //

                ossieSupport::ORB *orb = new ossieSupport::ORB();
                CORBA::Object_var initServ;
                initServ = orb->orb->resolve_initial_references("NameService");
                CosNaming::NamingContextExt_var rootContext;
                rootContext = CosNaming::NamingContextExt::_narrow(initServ);

                CORBA::Object_var u_Obj;
                CORBA::Object_var p_Obj;

                CosNaming::Name uCosName;
                CosNaming::Name pCosName;
  
                uCosName.length(2);
                uCosName[0].id   = "DomainName1";
                uCosName[0].kind = (const char*) "";
                uCosName[1].id   = uFindByName;
                uCosName[1].kind = (const char*) "";
                 
                pCosName.length(2);
                pCosName[0].id   = "DomainName1";
                pCosName[0].kind = (const char*) "";
                pCosName[1].id   = pFindByName;
                pCosName[1].kind = (const char*) "";

                u_Obj = rootContext->resolve(uCosName);
                p_Obj = rootContext->resolve(pCosName);
                if (CORBA::is_nil(u_Obj)) {
                    DEBUG(3, DomainManager, "u_Obj is nil reference");
                    return;
                }
                if (CORBA::is_nil(p_Obj)) {
                    DEBUG(3, DomainManager, "p_Obj is nil reference");
                    return;
                }

                CF::PortSupplier_var portSupplier =
                    CF::PortSupplier::_narrow(u_Obj);
                if (CORBA::is_nil(portSupplier)) {
                    DEBUG(3, DomainManager, "portSupplier is nil reference");
                    return;
                }

                CORBA::Object_var portObj;
                portObj = portSupplier->getPort(uPortName);
                CF::Port_var uPort = CF::Port::_narrow(portObj);
                if (CORBA::is_nil(uPort)) {
                    DEBUG(3, DomainManager, "uPort is nil reference");
                    return;
                }

                try {
                    uPort->connectPort(p_Obj, (*i)->getID());
                } catch (CF::Port::InvalidPort) {
                    DEBUG(3, DomainManager, "Caught InvalidPort Exception"); 
                } catch (CF::Port::OccupiedPort) {
                    DEBUG(3, DomainManager, "Caught OccupiedPort Exception");
                }
                
                DEBUG(3, DomainManager, "Removing pending connection");
                delete *i;
                _pendingConnections.erase(i);                

                delete orb;
            }
        }
        i++   
    }                  
#endif
}


void
DomainManager_impl::disconnectThisService (const char * serviceName)
{
    DEBUG(6, DomainManager, "Entering disconnectThisService.");

#if 0
//search registered devices to obtain the deviceManager of this service
    CF::DeviceManager_ptr devMgr;
    for (unsigned int i = 0; i < _registeredServices.size (); i++ )
        if (!strcmp (serviceName, _registeredServices[i]->name))
            devMgr = _registeredServices[i]->devMgrPtr;

//get the DCD
    DCDParser _dcdParser (devMgr->deviceConfigurationProfile ());

//parse connections of this DCD
    std::vector<Connection*>* _connection = _dcdParser.getConnections ();

//Find the connections that involve this service
//disconnect the ports the are using this service
    for (int _numConnections = _connection->size () - 1; _numConnections >= 0; _numConnections--)
    {
        UsesPort* _usesPortParser = (*_connection)[ _numConnections ]->getUsesPort ();

        FindBy* _findUsesPortBy = _usesPortParser->getFindBy ();

        ProvidesPort* _providesPortParser = NULL;

        FindBy* _findProvidesPortBy = NULL;

//ComponentSupportInterface* _componentSuppInterface = NULL:

        if ((*_connection)[ _numConnections ]->isProvidesPort ())
            _providesPortParser = (*_connection)[ _numConnections ]->getProvidesPort ();
        else if ((*_connection)[ _numConnections ]->isFindBy ())
            _findProvidesPortBy = (*_connection)[ _numConnections ]->getFindBy ();
// else if ( _connection[ _numConnections ]->isComponentSupportedInterface ())
        else
        {
//throw an exception?
        }

//get the connections that involve this device
        if (!strcmp (_providesPortParser->getID (), serviceName))
        {
	    CORBA::Object_var _usesObj = CORBA::Object::_nil ();

            if (_findUsesPortBy->isFindByNamingService ())
		{
		    while (CORBA::is_nil (_usesObj))
			///\todo When services are implemented fix this                    _usesObj = _orb_wrap->lookup (_findUsesPortBy->getFindByNamingServiceName ());
			;
		}
	    
            CF::PortSupplier_ptr _usesPort = CF::PortSupplier::_narrow (_usesObj);

            CF::Port_ptr _port = CF::Port::_narrow (_usesPort->getPort (_usesPortParser->getID ()));

            _port->disconnectPort (CORBA::string_dup ((*_connection)[ _numConnections ]->getID ()));

//store this connection in pendingConnections so it is established
//again when this service registers again
            _pendingConnections.push_back ( new Connection ( *(*_connection)[ _numConnections ]));
            CORBA::release(_usesPort);
            CORBA::release(_port);
        }
    }
#endif
}





/*void DomainManager_impl::writeLogRecord(CF::LogLevelType logLevelType)
{
    //There are no services considered in this release
}*/

//      METHOD:         installApplication
//      PURPOSE:        verify that all application file dependencies are available within
//                              the domain managers file manager
//      EXCEPTIONS:
//              --              InvalidProfile
//              --              InvalidFileName
//              --              ApplicationInstallationError

void
DomainManager_impl::installApplication (const char *profileFileName)
    throw (CORBA::SystemException, CF::InvalidProfile, CF::InvalidFileName,
           CF::DomainManager::ApplicationInstallationError)
{
// NOTE: the <softwareassembly> name attribute is the name of the App Factory
//               that is currently installed because it is the installed factory that
//               provides the value of profileFileName
    DEBUG(1, DomMgr, "Entering installApplication with " << profileFileName);

	if ((strstr (profileFileName, ".sad.xml")) == NULL)
	    std::cerr << "File " << profileFileName << " should end with .sad.xml." << std::endl;
	
	DEBUG(8, DomainManager, "About to parse SAD file.");

	CF::File_var file;
	try {
	    file = _fileMgr->open(profileFileName, true);
	} catch (CF::FileException ex) {
	    throw CF::InvalidFileName (CF::CFEINVAL, "File not found.");
	}

	SADParser *sadParser = new SADParser (file);
	file->close();

	DEBUG(8, DomainManager, "Done parsing SAD file.");

// check if application factory already exists for this profile
    for (unsigned int i = 0; i < appFact_servants.size(); i++)
    {
      if (strcmp(sadParser->getID (), appFact_servants[i]->identifier()) == 0)
        {
	    DEBUG(1, DomainManager, "Application already installed.");
            delete sadParser;
            throw CF::DomainManager::ApplicationInstallationError(CF::CFEEXIST, "Application already installed.");
        }
    }

    DEBUG(8, DomainManager, "Query the SAD for all ComponentPlacement's");

    std::vector < SADComponentPlacement * >*sadComponents =
        sadParser->getComponents ();

    DEBUG(8, DomainManager, "Query each ComponentPlacement for its SPD file");

    std::vector < SADComponentPlacement * >::iterator _iterator = sadComponents->begin ();

    while (_iterator != sadComponents->end ())
    {
        this->validateSPD (sadParser->getSPDById((*_iterator)->getFileRefId ()));
        ++_iterator;
    }

    DEBUG(8, DomainManager, "Create new Application Factory.");

    appFact_servants.push_back(new ApplicationFactory_impl (sadParser, _fileMgr, &_applications));

    DEBUG(8, DomainManager, "Add Application Factory to list.");

    unsigned int appFactIndex = appFact_servants.size()-1;

    _applicationFactories.length(appFactIndex+1); ///\todo Preallocate based on number of apps in init()
    _applicationFactories[appFactIndex] = appFact_servants[appFactIndex]->_this();

    ossieSupport::sendObjAdded_event(_identifier.c_str(), sadParser->getID(), sadParser->getName(), (CORBA::Object_var) NULL, StandardEvent::APPLICATION_FACTORY);

    DEBUG(1, DomMgr, "Leaving installApplication")
}


void
DomainManager_impl::uninstallApplication (const char *applicationId)
    throw (CORBA::SystemException, CF::DomainManager::InvalidIdentifier,
           CF::DomainManager::ApplicationUninstallationError)
{
    DEBUG(6, DomainManager, "Entering uninstallApplication.");
// NOTE: applicationId is the value of the <softwareassembly> name attribute
//               for the App Factory's SAD profile

    int appNum = -1;

    {    ///\bug Figure out MSVC flags to corectly scope for loops
        for (unsigned int i = 0; i < appFact_servants.size(); i++)
        {
            if (strcmp (applicationId,
                appFact_servants[i]->identifier ()) == 0)
            {
                appNum = i;
                break;
            }
        }
    }

    if (appNum == -1)
        throw CF::DomainManager::InvalidIdentifier ();

#if 0 ///\todo Figure out how to uninstall application without creating a new SADParser
    SADParser sadParser (appFact_servants[appNum]->softwareProfile ());

// remove all files associated with the Application
    std::vector < SADComponentPlacement * >*sadComponents =
        sadParser.getComponents ();

    std::vector < SADComponentPlacement * >::iterator _iterator =
        sadComponents->begin ();

    while (_iterator != sadComponents->end ())
    {
//this->removeSPD( (*_iterator)->getSPDFile() );  // technically, this is supposed to happen
// however, we don't think so, so we took it out.
        ++_iterator;
    }
#endif
    ///\todo Finish vectorization of uninstallApplication if double free fixed
    //    installedApplicationFactories.erase(appNum);

// if SUCCESS, write an ADMINISTRATIVE_EVENT to the DomainMgr's Log

// send event to Outgoing Domain Management channel consisting of:
// DomainManager identifier attribute
// this->_identifier
// uninstalled AppFactory identifier
// sadParser.getId()
// uninstalled AppFactory name
// sadParser.getName()
// uninstalled AppFactory IOR
// ask the ORB
// sourceCategory = APPLICATION_FACTORY
// StandardEvent enumeration

/// \todo Fix event sending

//    StandardEvent::DomainManagementObjectRemovedEventType _objRemovedEvent;

//    _objRemovedEvent.producerId = CORBA::string_dup (this->_identifier);

//    _objRemovedEvent.sourceId = CORBA::string_dup (sadParser.getID ());

//    _objRemovedEvent.sourceName = CORBA::string_dup (sadParser.getName ());

//    _objRemovedEvent.sourceCategory = StandardEvent::APPLICATION_FACTORY;

//    CORBA::Any _anyObjRemovedEvent;

//    _anyObjRemovedEvent <<= _objRemovedEvent;

//    sendEventToOutgoingChannel (_anyObjRemovedEvent);
}


void
DomainManager_impl::registerWithEventChannel(CORBA::Object_ptr registeringObject,
    const char *registeringId, const char *eventChannelName)
    throw (CORBA::SystemException,
           CF::InvalidObjectReference, 
           CF::DomainManager::InvalidEventChannelName,
           CF::DomainManager::AlreadyConnected)
{
    DEBUG(6, DomainManager, "Entering registerWithEventChannel.");

#if 0 ///\todo uncommment
// connect input registeringObject to an event channel as specified by the eventChannelName
// Narrow the object to the CosEventComm::PushConsumer interface
    CosEventComm::PushConsumer_var consumer =
        CosEventComm::PushConsumer::_narrow (registeringObject);

    if (CORBA::is_nil (consumer))
    {
// Raise the CF::InvalidObjectReference exception
//std::cerr << "Invalid PushConsumer Object Reference" << std::endl;
        throw CF::InvalidObjectReference ();
    }

// Get the ConsumerAdmin object from the event channel
    CosEventChannelAdmin::ConsumerAdmin_var consumerAdmin;

// Any registerWithEventChannel request received should be for the ODM_Channel only.
    if (strcmp (eventChannelName, "ODM_Channel") == 0)
        consumerAdmin = _odmEventChannel->for_consumers ();
    else if (strcmp (eventChannelName, "IDM_Channel") == 0)
        consumerAdmin = _idmEventChannel->for_consumers ();
    else
    {
// Raise the CF::InvalidEventChannelName exception
//std::cerr << "Invalid Event Channel Name: " << eventChannelName << std::endl;
        throw CF::DomainManager::InvalidEventChannelName ();
    }

    CosEventChannelAdmin::ProxyPushSupplier_var supplier =
        consumerAdmin->obtain_push_supplier ();

// Connect to the Push supplier: Raises AlreadyConnected
    supplier->connect_push_consumer (consumer);
#endif
}


void
DomainManager_impl::unregisterFromEventChannel (const char *unregisteringId,
    const char *eventChannelName)
    throw (CORBA::SystemException, CF::DomainManager::InvalidEventChannelName,
           CF::DomainManager::NotConnected)
{
    DEBUG(6, DomainManager, "Entering unregisterFromEventChannel.");
#if 0 /// \todo uncomment
// Any registerWithEventChannel request received should be for the IDM_Channel only.
    if (!(strcmp (eventChannelName, "IDM_Channel") == 0))
    {
// Raise the CF::InvalidEventChannelName exception
//std::cerr << "Invalid Event Channel Name: " << eventChannelName << std::endl;
        throw CF::DomainManager::InvalidEventChannelName ();
    }

// Get the ConsumerAdmin object from the event channel
    CosEventChannelAdmin::ConsumerAdmin_var consumerAdmin =
        _idmEventChannel->for_consumers ();

    CosEventChannelAdmin::ProxyPushSupplier_var supplier =
        consumerAdmin->obtain_push_supplier ();

// Connect to the Push supplier: Raises NotConnected

// get the reference from the "unregisteringId"

    CORBA::Object_var obj = ORB_WRAP::orb->string_to_object (unregisteringId);

    if (CORBA::is_nil (obj.in ()))    {
//std::cerr << "Object " << unregisteringId << " is invalid!!" << std::endl;
        throw CF::InvalidObjectReference ();
    }

    CosEventComm::PushConsumer_var consumer =
        CosEventComm::PushConsumer::_narrow (obj.in ());

    consumer->disconnect_push_consumer ();
#endif
}


void
DomainManager_impl::registerService(CORBA::Object_ptr registeringService,
    CF::DeviceManager_ptr registeredDeviceMgr, const char * name)
    throw (CF::DomainManager::RegisterError,
           CF::DomainManager::DeviceManagerNotRegistered,
           CF::InvalidObjectReference,
           CORBA::SystemException)
{
//The registerService operation shall verify the input registeringService and registeredDeviceMgr
//are valid object references.

//Verify they are not a nil reference
    if ( CORBA::is_nil(registeringService) || CORBA::is_nil(registeredDeviceMgr) ) {
//The registerService operation shall raise the CF InvalidObjectReference exception when input
//parameters registeringService or registeredDeviceMgr contains an invalid reference.

/*writeLogRecord(FAILURE_ALARM,invalid reference input parameter.);*/
        throw( CF::InvalidObjectReference("Cannot register Service. Either registeringService or DeviceMgr is a nil reference.") );
    }

//The registerService operation shall verify the input registeredDeviceMgr has been previously
//registered with the DomainManager.

//Verify that input is a registered DeviceManager
    if (!deviceMgrIsRegistered (registeredDeviceMgr)) {
//The registerService operation shall raise a DeviceManagerNotRegistered exception when the
//input registeredDeviceMgr parameter is not a nil reference and is not registered with the
//DomainManager.

/*writeLogRecord(FAILURE_ALARM,the service could not register because
the DeviceManager is not registered with the DomainManager.);*/
//throw( CF::DomainManager::DeviceManagerNotRegistered() );
    }

//The registerService operation shall add the registeringServices object reference and the
//registeringServices name to the DomainManager, if the name for the type of service being
//registered does not exist within the DomainManager. However, if the name of the registering
//service is a duplicate of a registered service of the same type, then the new service shall not be
//registered with the DomainManager.

//The registerService operation shall associate the input registeringService parameter with the
//input registeredDeviceMgr parameter in the DomainManagers, when the registeredDeviceMgr
//parameter indicates a DeviceManager registered with the DomainManager.

//Add registeringService and its name to domain manager
    storeServiceInDomainMgr(registeringService, registeredDeviceMgr, name);

//The registerService operation shall, upon successful service registration, establish any pending
//connection requests for the registeringService. The registerService operation shall, upon
//successful service registration, write an ADMINISTRATIVE_EVENT log record to a
//DomainManagers Log.

    establishPendingServiceConnections(name);

//The registerService operation shall, upon unsuccessful service registration, write a
//FAILURE_ALARM log record to a DomainManagers Log.

//The registerService operation shall, upon successful service registration, send an event to the
//Outgoing Domain Management event channel with event data consisting of a
//DomainManagementObjectAddedEventType. The event data will be populated as follows:
//1. The producerId shall be the identifier attribute of the DomainManager.
//2. The sourceId shall be the identifier attribute from the componentinstantiation element
//associated with the registered service.
//3. The sourceName shall be the input name parameter for the registering service.
//4. The sourceIOR shall be the registered service object reference.
//5. The sourceCategory shall be SERVICE.

//The registerService operation shall raise the RegisterError exception when an internal error
//exists which causes an unsuccessful registration.
}


void DomainManager_impl::unregisterService(CORBA::Object_ptr unregisteringService,const char * name)
throw (CF::DomainManager::UnregisterError, CF::InvalidObjectReference,CORBA::SystemException)
{
    DEBUG(6, DomainManager, "Entering unregisterService.");

//The unregisterService operation shall raise the CF InvalidObjectReference exception when the
//input parameter contains an invalid reference to a Service interface.
    if( CORBA::is_nil(unregisteringService) || !serviceIsRegistered(name) )
    {
/*writeLogRecord(FAILURE_ALARM,Device reference is not valid);*/

//The unregisterDevice operation shall raise the CF InvalidObjectReference exception when the
//input parameter contains an invalid reference to a Device interface.
        throw( CF::InvalidObjectReference("Cannot Unregister Service. Invalid reference") );
        return;
    }

//The unregisterService operation shall remove the unregisteringService entry specified by the
//input name parameter from the DomainManager.

//The unregisterService operation shall release (client-side CORBA release) the
//unregisteringService from the DomainManager.

    disconnectThisService(name);

    removeServiceFromDomainMgr(unregisteringService, name);

//The unregisterService operation shall, upon the successful unregistration of a Service, write an
//ADMINISTRATIVE_EVENT log record to a DomainManagers Log.

//The unregisterService operation shall, upon unsuccessful unregistration of a Service, write a
//FAILURE_ALARM log record to a DomainManagers Log.

//The unregisterService operation shall, upon successful service unregistration, send an event to
//the Outgoing Domain Management event channel with event data consisting of a
//DomainManagementObjectRemovedEventType. The event data will be populated as follows:
//1. The producerId shall be the identifier attribute of the DomainManager.
//2. The sourceId shall be the ID attribute from the componentinstantiation element
//associated with the unregistered service.
//3. The sourceName shall be the input name parameter for the unregistering service.
//4. The sourceCategory shall be SERVICE.

//The unregisterService operation shall raise the UnregisterError exception when an internal error
//exists which causes an unsuccessful unregistration.

}


void
DomainManager_impl::validateSPD (const char *_spdProfile, int _cnt)
{
    DEBUG(6, DomainManager, "Entering validateSPD. This function does nothing.");

#if 0 ///\todo Figure out if this is really necessary
    // It does not make sense to create and destroy parsers, this is a heavy operation
  if (_spdProfile == NULL)
    return;

  /// \todo Figure out checks for xml conforming to dtd, through suitable exception if it doesn't. Possibly CF::InvalidProfile

  try
    {
    
      // check the filename ends with the extension given in the spec
      if ((strstr (_spdProfile, ".spd.xml")) == NULL)
	  std::cerr << "File " << _spdProfile << " should end with .spd.xml" << std::endl;
      
      SPDParser spdParser (_spdProfile);

      // query SPD for PRF
    
      // check the file name ends with the extension given in the spec
      if ((strstr (spdParser.getPRFFile (), ".prf.xml")) == NULL && strlen(spdParser.getPRFFile()))
	  std::cerr << "File " << spdParser.getPRFFile() << " should end in .prf.xml." << std::endl;

      PRFParser prfParser (spdParser.getPRFFile ());

      // query SPD for SCD

      // Check the filename ends with  the extension given in the spec
      if ((strstr (spdParser.getSCDFile (), ".scd.xml")) == NULL)
	  std::cerr << "File " << spdParser.getSCDFile() << " should end with .scd.xml." << std::endl;
 

      SCDParser scdParser (spdParser.getSCDFile ());

      /// \todo Figure out if this should go: this->validateSPD( spdParser.getSPDFile(), ++_cnt );

    } catch (CF::InvalidFileName ex) {
      throw CF::DomainManager::ApplicationInstallationError (CF::CFEBADF, ex.msg);
    }
#endif
}


void
DomainManager_impl::removeSPD (const char *_spdProfile, int _cnt)
{
    DEBUG(6, DomainManager, "Entering removeSPD.");

#if 0 ///\todo Fiugre out shwt this code is for
    // I think it supports s CF requirement that is silly

    if (_spdProfile == NULL)
        return;

    SPDParser *_spdParser = new SPDParser (_spdProfile);

    try
    {
        _fileMgr->remove (_spdParser->getPRFFile ());
    }
    catch (CF::InvalidFileName &)
    {
// TODO: write FAILURE_ALARM
// TODO: output the error message from the invalid file name

        delete _spdParser;
        _spdParser = NULL;

        throw CF::DomainManager::ApplicationUninstallationError (CF::CFEBADF,
            "[DomainManager:removeSPD] invalid PRF file");
    }
    catch (CF::FileException &)
    {
// TODO: write FAILURE_ALARM
// TODO: output the error message from the file exception

        delete _spdParser;
        _spdParser = NULL;

        throw CF::DomainManager::ApplicationUninstallationError (CF::CFEBADF,
            "[DomainManager:removeSPD] PRF file exception");
    }

    try
    {
        _fileMgr->remove (_spdParser->getSCDFile ());
    }
    catch (CF::InvalidFileName &)
    {
// TODO: write FAILURE_ALARM
// TODO: output the error message from the invalid file name

        delete _spdParser;
        _spdParser = NULL;

        throw CF::DomainManager::ApplicationUninstallationError (CF::CFEBADF,
            "[DomainManager:removeSPD] invalid SCD file");
    }
    catch (CF::FileException &)
    {
// TODO: write FAILURE_ALARM
// TODO: output the error message from the file exception

        delete _spdParser;
        _spdParser = NULL;

        throw CF::DomainManager::ApplicationUninstallationError (CF::CFEBADF,
            "[DomainManager:removeSPD] SCD file exception");
    }

    removeSPD (_spdParser->getSPDFile (), ++_cnt);

    delete _spdParser;
    _spdParser = NULL;

    try
    {
        _fileMgr->remove (_spdProfile);
    }
    catch (CF::InvalidFileName &)
    {
/// \todo write FAILURE_ALARM
/// \todo output the error message from the invalid file name
        throw CF::DomainManager::ApplicationUninstallationError (CF::CFEBADF,
            "[DomainManager:removeSPD] invalid SPD file");
    }
    catch (CF::FileException &)
    {
/// \todo write FAILURE_ALARM
/// \todo output the error message from the file exception
        throw CF::DomainManager::ApplicationUninstallationError (CF::CFEBADF,
            "[DomainManager:removeSPD] SPD file exception");
    }
#endif
}


void DomainManager_impl::createEventChannels (void)
{
    DEBUG(6, DomainManager, "Entering createEventChannels.");

  // This code is very likely omniEvent specific. See pp -- in --.
  /// \todo break into smaller chuncks and make error handling better
  /// \todo Have this routine take the Event Channel name as an argument and return the object


  // Get the Name Service root context
#ifdef HAVE_OMNIEVENTS
  CORBA::Object_var obj = ORB_WRAP::orb->resolve_initial_references("NameService");
  CosNaming::NamingContext_var rootContext = CosNaming::NamingContext::_nil();

  rootContext = CosNaming::NamingContext::_narrow(obj);
  if (CORBA::is_nil(rootContext))
    throw CORBA::OBJECT_NOT_EXIST();
  
  // Find the object reference for the Event Channel factory
  CosNaming::Name name;
  name.length(1);
  name[0].id = CORBA::string_dup("EventChannelFactory");


  obj = rootContext->resolve(name);

  omniEvents::EventChannelFactory_var factory = omniEvents::EventChannelFactory::_narrow(obj);

  if (CORBA::is_nil(factory))
    throw CORBA::OBJECT_NOT_EXIST();  /// \todo throw something better

  // Check that object support EventChannel object interface
  CosLifeCycle::Key key;
  key.length(1);
  key[0].id = CORBA::string_dup("EventChannel");
  key[0].kind = CORBA::string_dup("object interface");

  if (!factory->supports(key)) {
      std::cerr << "Factory does not support Event Channel interface, dying" << std::endl;
    exit (-1);
  }

  // Create the IDM Event Channel Object
  CosLifeCycle::Criteria criteria; // I think this passes options to the channel PJB

  CORBA::Object_var IDM_channelObj = factory->create_object(key, criteria);
  if (CORBA::is_nil(IDM_channelObj))
    exit(-1);

  IDM_channel = CosEventChannelAdmin::EventChannel::_narrow(IDM_channelObj);
  if (CORBA::is_nil(IDM_channel))
    exit (-1);

  CosNaming::Name idm_name;

  ORB_WRAP::getCosName (idm_name, "/DomainName1/IDM_Channel");
  rootContext->rebind(idm_name, IDM_channel.in());

  // Create the ODM Event Channel object

  CORBA::Object_var ODM_channelObj = factory->create_object(key, criteria);
  if (CORBA::is_nil(ODM_channelObj))
    exit(-1);

  ODM_channel = CosEventChannelAdmin::EventChannel::_narrow(ODM_channelObj);
  if (CORBA::is_nil(ODM_channel))
    exit (-1);

  CosNaming::Name odm_name;

  ORB_WRAP::getCosName (odm_name, "/DomainName1/ODM_Channel");


  rootContext->rebind(odm_name, ODM_channel.in());
#endif
}


#ifdef HAVE_OMNIEVENTS
CosEventChannelAdmin::ProxyPushConsumer_var DomainManager_impl::connectEventChannel(CosEventChannelAdmin::EventChannel_var eventChannel)
{

  /// \todo Add exception handling
  CosEventChannelAdmin::SupplierAdmin_var supplier_admin;

  supplier_admin = eventChannel->for_suppliers();

  CosEventChannelAdmin::ProxyPushConsumer_var proxy_consumer;

  proxy_consumer = supplier_admin->obtain_push_consumer();

  return proxy_consumer;
}

DomainManagerEventHandler::DomainManagerEventHandler (DomainManager_impl *
_dmn)
{
    DEBUG(6, DomainManager, "Entering DomainManagerEventHandler.");
    _dmnMgr = _dmn;
}


void
DomainManagerEventHandler::push (const CORBA::Any & _any)
throw (CORBA::SystemException, CosEventComm::Disconnected)
{
    DEBUG(6, DomainManager, "Entering push.");

    _dmnMgr->_applications[0] = CF::Application::_nil ();
}


void
DomainManagerEventHandler::disconnect_push_consumer ()
throw (CORBA::SystemException)
{
    CORBAOBJ _obj = ORB_WRAP::orb->resolve_initial_references ("POACurrent");

    PortableServer::Current_var _current = 
        PortableServer::Current::_narrow (_obj);

    PortableServer::POA_var _poa = _current->get_POA ();
    PortableServer::ObjectId_var _oid = _current->get_object_id ();
    _poa->deactivate_object (_oid);
}
#endif

#ifdef AUTOMATIC_TEST

void DomainManager_impl::displayAttributes(void)
{
    printf("\n\nDomain Manager Identifier: %s \n", _identifier);
    printf("\nDomain Manager Profile: %s \n\n", _domainManagerProfile);
}


void DomainManager_impl::displayDeviceManagers(void)
{
//CF::DeviceManager_ptr myDevMgr_ptr;
    if(_deviceManagers.length() > 0) {
        printf("\nDevice Managers registered in %s Domain Manager\n", this->identifier());
        for (int i = 0; i < _deviceManagers.length(); i++) {
            printf( "\n  %s\n", _deviceManagers[i]->label());
        }
    }
    else {
        printf("\nThere are no Device Managers registered in %s Domain Manager\n", this->identifier());
    }

}


void DomainManager_impl::displayRegisteredDevices(void)
{

//CF::DeviceManager_ptr myDevMgr_ptr;
    if(_registeredDevices.size() > 0) {
        printf("\nDevices registered in %s Domain Manager\n", this->identifier());
        for (int i = 0; i < _registeredDevices.size(); i++) {
            printf( "\n  %s\n", _registeredDevices[i]->devicePtr->label());
        }
    }
    else {
        printf("\nThere are no registered Devices in %s Domain Manager\n", this->identifier());
    }
}


void DomainManager_impl::displayRegisteredServices (void)
{
    if(_registeredServices.size() > 0) {
        printf("\nServices registered in %s Domain Manager\n", this->identifier());
        for (int i = 0; i < _registeredServices.size(); i++) {
            if( !CORBA::is_nil(_registeredServices[i]->objectPtr) )
                printf("\n%s\n", _registeredServices[i]->name);
            else
                printf("\nInvalid object reference for %s\n",_registeredServices[i]->name);
        }
    }
    else {
        printf("\nThere are no registered devices in %s Domain Manager\n", this->identifier());
    }
}
#endif
