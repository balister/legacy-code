/****************************************************************************

Copyright 2004, 2006, 2007, 2008 Virginia Polytechnic Institute and State University

Copyright 2008, Philip Balister philip@opensdr.com

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

Nov/10/03	C. Neely	Created
C. Aguayo

****************************************************************************/
#include <iostream>

#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else  /// \todo change else to ifdef windows var
#include <process.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <errno.h>

#include "ossie/debug.h"
#include "ossie/ossieSupport.h"
#include "ossie/DeviceManager_impl.h"
#include "ossie/portability.h"

DeviceManager_impl::~DeviceManager_impl ()
{
    DEBUG(6, DeviceManager, "Entering destructor.");
}


DeviceManager_impl::DeviceManager_impl(const char *DCDInput, const char *rootPath)

{
    DEBUG(6, DeviceManager, "Entering constructor with " << DCDInput);
    
    _deviceConfigurationProfile = DCDInput;
    fsRoot = rootPath;
}

//Parsing constructor
void DeviceManager_impl::post_constructor (CF::DeviceManager_var my_object_var)
{
    DEBUG(6, DeviceManager, "Entering post_constructor.");
    orb_obj = new ossieSupport::ORB();

    myObj = my_object_var;

    _registeredDevices.length(0);
    _registeredServices.length(0);

    fs_servant = new FileSystem_impl(fsRoot.c_str());
    fileSystem = fs_servant->_this();


    if (!fileSystem->exists (_deviceConfigurationProfile.c_str())) {
	DEBUG(1, DeviceManager, "DCD File not found.");
        throw (CF::FileException (CF::CFEEXIST, "DCD File not found."));
    }
    
    
    //Get Device Manager attributes (deviceConfigurationProfile, identifier and label)
    //from DCD file
    
    CF::File_var file = fileSystem->open(_deviceConfigurationProfile.c_str(), true);
    
    DCDParser _DCDParser (file);
    
    _identifier = _DCDParser.getID();
    _label = _DCDParser.getName();
    
    //get DomainManager reference
    
    getDomainManagerReference ((char *)_DCDParser.getDomainManagerName ());
    
    fileMgr = _dmnMgr->fileMgr(); ///\todo delete this line

    //Register DeviceManager with DomainManager
    _dmnMgr->registerDeviceManager (my_object_var);
    
    _adminState = DEVMGR_REGISTERED;
    
    //parse filesystem names
    
    //Parse local componenents from DCD files
    std::vector <componentPlacement> componentPlacements = _DCDParser.getComponentPlacements ();
    
    for (unsigned int i = 0; i < componentPlacements.size(); i++) {
	CF::File_var file = fileSystem->open(_DCDParser.getFileNameFromRefId(componentPlacements[i].refId()), true);
	
	SPDParser _SPDParser (file);
	file->close();
	
	//get code file name from implementation
	std::vector < SPDImplementation * >*_implementations =
	    _SPDParser.getImplementations ();
	
	///----------Assuming only one implementation
	//spawn device
	
#ifdef MS_dotNET                        //FIXME PJB
	int myPid2 = _spawnl (_P_DETACH,
			      //Device Proxy
			      (*_implementations)[0]->getCodeFile (),
			      //argv[0]
			      (*_implementations)[0]->getCodeFile (),
			      componentPlacements[i].usageName(),     //Device Label --argv[1]
			      _DCDParser.getFileNameFromRefId(componentPlacements[i].refId()),      //Device SoftwareProfile --argv[2]
			      NULL);
#endif
#ifdef HAVE_WORKING_FORK
	int myPid2;
	
	std::string execFilePath = fsRoot + (*_implementations)[0]->getCodeFile ();
	DEBUG(2, DevMgr, "Launching Device file " << execFilePath.c_str() << " Usage name " << componentPlacements[i].usageName());
	    
	if ((myPid2 = fork()) < 0)
	    std::cout << "Fork Error" << std::endl;
	

	if (myPid2 == 0) {
	    // in child
	    if (getenv("VALGRIND")) {
		std::string logFile = "--log-file=";
		logFile += (*_implementations)[0]->getCodeFile ();
		char *val = "/usr/local/bin/valgrind";
		execl(val, val, logFile.c_str(), execFilePath.c_str(), componentPlacements[i].id(), componentPlacements[i].usageName() , _DCDParser.getFileNameFromRefId(componentPlacements[i].refId()), NULL);
	    } else {
		execl(execFilePath.c_str(), execFilePath.c_str(), componentPlacements[i].id(), componentPlacements[i].usageName() , _DCDParser.getFileNameFromRefId(componentPlacements[i].refId()), NULL);
		
	    }
	    std::cout << "Device did not execute : " << strerror(errno) << std::endl;
	    exit (-1);
	}
#endif
	
	CORBA::Object_var _obj = CORBA::Object::_nil();
	///\todo Convert to use iostream, you can buffer overflow with long usageName
	char nameStr[255];
	sprintf( nameStr, "DomainName1/%s", componentPlacements[i].usageName() );
	DEBUG(3, DevMgr, "searching for "<< nameStr);
	for (unsigned int delay(1000000);;) {
	    /// \todo sleep prevents system from beating Name Service to death, Fix better
	    ossieSupport::nsleep(0, delay);

	    try { // the call will throw if the name is not found
		_obj = orb_obj->get_object_from_name(nameStr);
	    } catch (CosNaming::NamingContext::NotFound) {};

	    if (!CORBA::is_nil(_obj))
		break;

	    if (delay < 800000000)
		delay *= 2;
	}

	DEBUG(3, DevMgr, "found "<< nameStr);
	
	CF::Device_var tempDevice = CF::Device::_narrow (_obj);
	tempDevice->initialize ();
	
	if (strlen(_SPDParser.getPRFFile ()) > 0){
	    DEBUG(7, DevMgr, "getting PRF file: "<<_SPDParser.getPRFFile ());

	    //Get properties from SPD
	    std::string prfFilePath;
	    CF::File_var prfFile = fileSystem->open(ossieSupport::spd_rel_file(_DCDParser.getFileNameFromRefId(componentPlacements[i].refId()), _SPDParser.getPRFFile (), prfFilePath), true);
	    PRFParser _PRFparser (prfFile);
	    prfFile->close();
	    
	    std::vector <PRFProperty *> *prfSimpleProp = _PRFparser.getConfigureProperties ();
	    CF::Properties configCapacities;
	    configCapacities.length (prfSimpleProp->size ());
	    for (unsigned int i = 0; i < prfSimpleProp->size (); i++) {
	        configCapacities[i] = *((*prfSimpleProp)[i]->getDataType ());
	    }
	    
	    //configure properties
	    DEBUG(3, DevMgr, "Configuring capacities");
	    tempDevice->configure (configCapacities);
	}

	DEBUG(3, DevMgr, "Registering device");
	registerDevice (CF::Device::_duplicate(tempDevice));
	DEBUG(3, DevMgr, "Device Registered");
	
    }
    
    //So far, it is assumed that all components are local
    /*//Obtain DEPLY-ON components
      std::vector<DCDComponentPlacement> DeployOnComponentsVector = _DCDParser.getDeployOnComponents();
      
      std::vector<char*> DPDList;
      
      //Get DPD file names from DeployOnComponentsVector
      for(int i = 0; i < DeployOnComponentsVector.size(); i++) //Makes list of DPD files.
      DPDList.push_back( DeployOnComponentsVector[i].getDPDFile() );
    */
    
    
}


void
DeviceManager_impl::init ()
{
    DEBUG(6, DeviceManager, "Entering init.");

    _adminState = DEVMGR_UNREGISTERED;
}


void
DeviceManager_impl::getDomainManagerReference (char *domainManagerName)
{
    DEBUG(6, DeviceManager, "Entering getDomainManagerReference.");

    CORBA::Object_var obj = CORBA::Object::_nil();

/// \todo sleep prevents system from beating Name Service to death, Fix better
    for (unsigned int delay(1000);;) {

	try {
	    obj = orb_obj->get_object_from_name (domainManagerName);
	} catch (CosNaming::NamingContext::NotFound) {};

	if (!CORBA::is_nil(obj))
	    break;

	usleep(delay);

	if (delay < 1000000)
	    delay *= 2;
    }
    
    _dmnMgr = CF::DomainManager::_narrow (obj);
}


char *DeviceManager_impl::deviceConfigurationProfile ()
throw (CORBA::SystemException)
{
    DEBUG(6, DeviceManager, "Entering deviceConfigurationProfile.");

    std::string result;
    ossieSupport::createProfileFromFileName(_deviceConfigurationProfile, result);
    return CORBA::string_dup(result.c_str());
}


CF::FileSystem_ptr DeviceManager_impl::fileSys ()throw (CORBA::
SystemException)
{
    DEBUG(6, DeviceManager, "Entering fileSys.");
    CF::FileSystem_var result = fileSystem;
    return result._retn();
}


char *DeviceManager_impl::identifier ()
throw (CORBA::SystemException)
{
    DEBUG(6, DeviceManager, "Entering identifier.");
    return CORBA::string_dup (_identifier.c_str());
}


char *DeviceManager_impl::label ()
throw (CORBA::SystemException)
{
    DEBUG(6, DeviceManager, "Entering label.");
    return CORBA::string_dup (_label.c_str());
}


void DeviceManager_impl::configure (const CF::Properties & configProperties) throw (CF::PropertySet::PartialConfiguration, CF::PropertySet::InvalidConfiguration, CORBA::SystemException)
{
}

void DeviceManager_impl::query (CF::Properties & configProperties) throw (CF::UnknownProperties, CORBA::SystemException)
{
}

CORBA::Object* DeviceManager_impl::getPort (const char *) throw (CF::PortSupplier::UnknownPort, CORBA::SystemException)
{
  return NULL;
}

CF::DeviceSequence *
DeviceManager_impl::registeredDevices ()throw (CORBA::SystemException)
{
    DEBUG(6, DeviceManager, "Entering registeredDevices.");
    CF::DeviceSequence_var result = new CF::DeviceSequence(_registeredDevices);
    return result._retn();
}


CF::DeviceManager::ServiceSequence *
DeviceManager_impl::registeredServices ()throw (CORBA::SystemException)
{
    DEBUG(6, DeviceManager, "Entering registeredServices.");

    CF::DeviceManager::ServiceSequence_var result = 
        new CF::DeviceManager::ServiceSequence(_registeredServices);
    return result._retn();
}


void
DeviceManager_impl::registerDevice (CF::Device_ptr registeringDevice)
throw (CORBA::SystemException, CF::InvalidObjectReference)
{
    DEBUG(6, DeviceManager, "Entering registerDevice.");
    if (CORBA::is_nil (registeringDevice)) {
	//writeLogRecord(FAILURE_ALARM,invalid reference input parameter.)

        throw (CF::
            InvalidObjectReference
            ("Cannot register Device. registeringDevice is a nil reference."));
        return;
    }

    // Register the device with the Device manager, unless it is already
    // registered 
    if (!deviceIsRegistered (registeringDevice)) {
        _registeredDevices.length (_registeredDevices.length () + 1);
        _registeredDevices[_registeredDevices.length () - 1] =
            registeringDevice;
    }

    // If this Device Manager is registered with a Domain Manager, register
    // the new device with the Domain Manager
    if (_adminState == DEVMGR_REGISTERED) {
        _dmnMgr->registerDevice (registeringDevice, myObj);
    }

//The registerDevice operation shall write a FAILURE_ALARM log record to a
//DomainManagers Log, upon unsuccessful registration of a Device to the DeviceManagers
//registeredDevices.
}


//This function returns TRUE if the input registeredDevice is contained in the _registeredDevices list attribute
bool DeviceManager_impl::deviceIsRegistered (CF::Device_ptr registeredDevice)
{
    DEBUG(6, DeviceManager, "Entering deviceIsRegistered.");
//Look for registeredDevice in _registeredDevices
    for (unsigned int i = 0; i < _registeredDevices.length (); i++)
    {
        if (strcmp (_registeredDevices[i]->label (), registeredDevice->label ())
            == 0)
        {
            return true;
        }
    }
    return false;
}


//This function returns TRUE if the input serviceName is contained in the _registeredServices list attribute
bool DeviceManager_impl::serviceIsRegistered (const char *serviceName)
{
    DEBUG(6, DeviceManager, "Entering serviceIsRegistered.");
//Look for registeredDevice in _registeredDevices
    for (unsigned int i = 0; i < _registeredServices.length (); i++)
    {
        if (strcmp (_registeredServices[i].serviceName, serviceName)  == 0)
        {
            return true;
        }
    }
    return false;
}


void
DeviceManager_impl::unregisterDevice (CF::Device_ptr registeredDevice)
throw (CORBA::SystemException, CF::InvalidObjectReference)
{
    DEBUG(6, DeviceManager, "Entering unRegisterDevice.");
    bool deviceFound = false;
    if (CORBA::is_nil (registeredDevice))         //|| !deviceIsRegistered(registeredDevice) )
    {
//The unregisterDevice operation shall write a FAILURE_ALARM log record, when it cannot
//successfully remove a registeredDevice from the DeviceManagers registeredDevices.

//The unregisterDevice operation shall raise the CF InvalidObjectReference when the input
//registeredDevice is a nil CORBA object reference or does not exist in the DeviceManagers
//registeredDevices attribute.
/*writeLogRecord(FAILURE_ALARM,invalid reference input parameter.); */
        throw (CF::
            InvalidObjectReference
            ("Cannot unregister Device. registeringDevice is a nil reference."));

        return;
    }

//The unregisterDevice operation shall remove the input registeredDevice from the
//DeviceManagers registeredDevices attribute.

//Look for registeredDevice in _registeredDevices
    for (unsigned int i = 0; i < _registeredDevices.length (); i++)
    {
        if (strcmp (_registeredDevices[i]->label (), registeredDevice->label ())
            == 0)
        {
//when the appropiater device is found, remove it from the _registeredDevices sequence
            deviceFound = true;
            if (_adminState == DEVMGR_REGISTERED)
            {
                _dmnMgr->unregisterDevice (CF::Device::_duplicate (registeredDevice));
                CORBA::release (registeredDevice);
            }
            for (unsigned int j = i; j < _registeredDevices.length () - 1; j++)
            {
//The unregisterDevice operation shall unregister
//the input registeredDevice from the DomainManager when the input registeredDevice is
//registered with the DeviceManager and the DeviceManager is not shutting down.
                _registeredDevices[j] = _registeredDevices[j + 1];
            }
//_registeredDevices[_registeredDevices.length() - 1] = 0;
            _registeredDevices.length (_registeredDevices.length () - 1);
//TO DO: Avoid memory leaks by reducing the length of the sequence _registeredDevices
            break;
        }
    }
    if (!deviceFound)
    {
/*writeLogRecord(FAILURE_ALARM,invalid reference input parameter.); */

        throw (CF::
            InvalidObjectReference
            ("Cannot unregister Device. registeringDevice was not registered."));
        return;
    }

}


void
DeviceManager_impl::shutdown ()
throw (CORBA::SystemException)
{
    DEBUG(6, DeviceManager, "Entering shutdown.");
    _adminState = DEVMGR_SHUTTING_DOWN;

//The shutdown operation shall unregister the DeviceManager from the DomainManager.
    _dmnMgr->unregisterDeviceManager (this->_this ()); ///\bug This looks wrong.

//The shutdown operation shall perform releaseObject on all of the DeviceManagers registered
//Devices (DeviceManagers registeredDevices attribute).

    for (int i = _registeredDevices.length () - 1; i >= 0; i--)
    {
//Important Note: It is necessary to manage the lenght of the _registeredDevices sequence
//otherwise, some elements in the sequence will be null.
        _registeredDevices[i]->label ();          ////////////////////////////////////////////////test
        CF::Device_var tempDev = CF::Device::_duplicate (_registeredDevices[i]);
//_registeredDevices[i]->releaseObject();
        unregisterDevice (_registeredDevices[i]);
        tempDev->releaseObject ();
    }

}


void
DeviceManager_impl::registerService (CORBA::Object_ptr registeringService,
const char *name)
throw (CORBA::SystemException, CF::InvalidObjectReference)
{
    DEBUG(6, DeviceManager, "Entering registerService.");
//This release does not support services
    if (CORBA::is_nil (registeringService))
    {
/*writeLogRecord(FAILURE_ALARM,invalid reference input parameter.); */

        throw (CF::
            InvalidObjectReference
            ("Cannot register Device. registeringDevice is a nil reference."));
        return;
    }

//The registerService operation shall add the input registeringService to the DeviceManagers
//registeredServices attribute when the input registeringService does not already exist in the
//registeredServices attribute. The registeringService is ignored when duplicated.
    if (!serviceIsRegistered (name))
    {
        int len = _registeredServices.length();
        _registeredServices.length (len + 1);
        _registeredServices[len].serviceObject = CORBA::Object::_duplicate(registeringService);
        _registeredServices[len].serviceName = name;
    }

//The registerService operation shall register the registeringService with the DomainManager
//when the DeviceManager has already registered to the DomainManager and the
//registeringService has been successfully added to the DeviceManagers registeredServices
//attribute.
    if (_adminState == DEVMGR_REGISTERED)
    {
        _dmnMgr->registerService (registeringService, this->_this (), name);
    }

//The registerService operation shall write a FAILURE_ALARM log record, upon unsuccessful
//registration of a Service to the DeviceManagers registeredServices.
//The registerService operation shall raise the CF InvalidObjectReference exception when the
//input registeringService is a nil CORBA object reference.

}


void
DeviceManager_impl::unregisterService (CORBA::Object_ptr registeredService,
const char *name)
throw (CORBA::SystemException, CF::InvalidObjectReference)
{
    DEBUG(6, DeviceManager, "Entering unRegisterService.");
    if (CORBA::is_nil (registeredService))
    {
/*writeLogRecord(FAILURE_ALARM,invalid reference input parameter.); */

        throw (CF::
            InvalidObjectReference
            ("Cannot unregister Service. registeringService is a nil reference."));
        return;
    }

//The unregisterService operation shall remove the input registeredService from the
//DeviceManagers registeredServices attribute. The unregisterService operation shall unregister
//the input registeredService from the DomainManager when the input registeredService is
//registered with the DeviceManager and the DeviceManager is not in the shutting down state.

//Look for registeredService in _registeredServices
    for (unsigned int i = 0; i < _registeredServices.length (); i++)
    {
        if (strcmp (_registeredServices[i].serviceName, name) == 0)
        {
//when the appropiater device is found, remove it from the _registeredDevices sequence
            if (_adminState == DEVMGR_REGISTERED)
            {
                _dmnMgr->unregisterService (registeredService, name);
            }

            for (unsigned int j = i; j < _registeredServices.length ()-1; j++)
            {

                CORBA::release (registeredService);
                _registeredServices[j] = _registeredServices[j+1];
            }
            _registeredServices.length (_registeredServices.length () - 1);
            return;
        }
    }

//If it didn't find registeredDevice, then throw an exception
/*writeLogRecord(FAILURE_ALARM,invalid reference input parameter.);*/
    throw (CF::
        InvalidObjectReference
        ("Cannot unregister Service. registeringService was not registered."));
//The unregisterService operation shall write a FAILURE_ALARM log record, when it cannot
//successfully remove a registeredService from the DeviceManagers registeredServices.
//The unregisterService operation shall raise the CF InvalidObjectReference when the input
//registeredService is a nil CORBA object reference or does not exist in the DeviceManagers
//registeredServices attribute.
}


char *
DeviceManager_impl::
getComponentImplementationId (const char *componentInstantiationId)
throw (CORBA::SystemException)
{
    DEBUG(6, DeviceManager, "Entering getComponentImplementationId.");
//The getComponentImplementationId operation shall return the SPD implementation elements
//ID attribute that matches the SPD implementation element used to create the component
//identified by the input componentInstantiationId parameter.

#if 0
    DCDParser _DCDParser (_deviceConfigurationProfile);
    std::vector < char *>*LocalComponentsVector =
        _DCDParser.getLocalComponents ();
#endif

    std::cout << "If this appears look at DeviceManager_impl.cpp line 572" << std::endl;
/*for (int i = 0; i<localComponentsVector->size();i++)
   {
   //get componentInstatiationId from each loal component
   std::vector<ComponentInstantiation*> instantiations = LocalComponentsVector[i].getInstantiations();
   //assuming only one instantiation
   if( strcmp(componentInstantiationId, instantiations[0]->getID()) == 0)
   {
   SPDParser spdParser ( LocalComponentsVector[i].getSPDFile() );
   std::vector<SPDImplementation*>  implementations = spdParser.getImplementations();
   return implementations[0]->getID();
   }
} */
    return "";

//The getComponentImplementationId operation shall return an empty string when the input
//componentInstantiationId parameter does not match the ID attribute of any SPD implementation
//element used to create the component.
}

#ifdef AUTOMATIC_TEST

void DeviceManager_impl::displayRegisteredDevices (void)
{
	
    CF::Device_ptr myDev_ptr;
    if(_registeredDevices.length() > 0)
    {
	std::cout << "Devices registered in "<<  this->label() <<" Device Manager"<< std::endl;
        for (unsigned int i = 0; i < _registeredDevices.length(); i++)
        {
            myDev_ptr=CF::Device::_narrow(_registeredDevices[i]);
	    std::cout<< "\n  "<< myDev_ptr->label()<<std::endl;
            //cout<<( "\n  %s\n",_registeredDevices[i]->label() );
        }
    }
    else
    {
	std::cout<<"\nThere are no registered devices in "<<this->label()<<" Device Manager"<<std::endl;
    }
}

void DeviceManager_impl::displayRegisteredServices (void)
{
    if( _registeredServices.length() > 0 )
    {
	std::cout<<"\nServices registered in "<<this->label() <<"Device Manager"<<std::endl;
        for (unsigned int i = 0; i < _registeredServices.length(); i++)
        {
            if( CORBA::is_nil(_registeredServices[i].serviceObject) )
		std::cout<<"\nInvalid object reference for "<<_registeredServices[i].serviceName<<std::endl;
            else
		std::cout<<"\n"<<_registeredServices[i].serviceName<<std::endl;
        }
    }
    else
    {
	std::cout<<"\nThere are no registered services in "<<this->label()<<" Device Manager"<<std::endl;
    }
}
#endif
