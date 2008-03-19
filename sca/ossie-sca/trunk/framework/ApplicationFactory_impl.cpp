/****************************************************************************
 
Copyright 2004, 2005, 2006, 2007, 2008 Virginia Polytechnic Institute and State University
 
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

#include <iostream>
#include <sstream>

#ifdef MS_dotNET
#include <process.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "ossie/debug.h"
#include "ossie/ossieSupport.h"
#include "ossie/portability.h"
#include "ossie/ApplicationFactory_impl.h"

#include <sstream>

ApplicationFactory_impl::ApplicationFactory_impl (SADParser *sadParser, CF::FileManager_ptr _fileMgr, CF::DomainManager::ApplicationSequence *_appseq)
{
    orb = new ossieSupport::ORB();

    ///\todo Pass in profile file name and store in _softwareprofile
    
    // Get appication factory data for private variables
    fileMgr = _fileMgr;
    _sadParser = sadParser;
    _name = _sadParser->getName();
    _identifier = _sadParser->getID();
   
    appseq = _appseq;
   
    // Get an object reference for the domain manager
    CORBA::Object_ptr obj = orb->get_object_from_name("DomainName1/DomainManager");
    dmnMgr = CF::DomainManager::_narrow (obj);
    
#ifdef HAVE_OMNIEVENTS
    
    ApplicationFactoryEventHandler *_evtHandler =
        new ApplicationFactoryEventHandler (this);
        
    dmnMgr->registerWithEventChannel (_evtHandler->_this (),
                                      CORBA::string_dup (_identifier),
                                      CORBA::string_dup ("ODM_Channel"));
#endif
}

ApplicationFactory_impl::~ApplicationFactory_impl ()

{
    delete orb;
}


CF::Application_ptr ApplicationFactory_impl::create (const char *name,
        const CF::Properties & initConfiguration,
        const CF::DeviceAssignmentSequence & deviceAssignments)
throw (CORBA::SystemException, CF::ApplicationFactory::CreateApplicationError,
       CF::ApplicationFactory::CreateApplicationRequestError,
       CF::ApplicationFactory::InvalidInitConfiguration)
{

    DEBUG(3, ApplicationFactory, "Entering create");
    
    // establish naming context for this waveform
    std::string waveformName = name;

    //Check if waveform name already exists
    for (unsigned int i = 0; i < appseq->length(); ++i) {
	if (waveformName == (*appseq)[i]->name()) {
	    DEBUG(5, ApplicationFactory, "Error: Waveform name already exists.");
	    throw CF::ApplicationFactory::CreateApplicationError(CF::CFEEXIST, "Application name already exists.");
	}
    }

    CosNaming::NamingContext_ptr waveformContext;

    create_waveform_nc_from_name(waveformName, waveformContext);
    
    DEBUG(4, ApplicationFactory, "Creating waveform: " << waveformName);

    ///\todo Remove the dependency on the waveform naming context.
    // the reason why it needs to be re-parsed is to populate the vector with the new waveform naming context

    DEBUG(9, ApplicationFactory, "Parsing component data.");

    if (requiredComponents.size() == 0)
        getRequiredComponents();
    
    
    // If there is a device assignment sequence, verify its validity
    // otherwise generate deployment information automatically
    ///\todo Write dynamic component deployment routine

    DEBUG(9, ApplicationFactory, "Verifying DAS.");

    if (deviceAssignments.length() > 0)
        verifyDeviceAssignments(deviceAssignments);
    else {
        std::cout << "No Device Assignment Sequence, write dynamic deployment code" << std::endl;
        throw CF::ApplicationFactory::CreateApplicationRequestError();
    }
    
    // allocate space for device assignment
    // (assume maximum length of 10 - will make dynamic in later version)
    CF::DeviceAssignmentSequence* _availableDevs =
        new CF::DeviceAssignmentSequence(deviceAssignments);
        
    //_availableDevs->length (0);
    
    PROC_ID_SEQ* _pidSeq = new PROC_ID_SEQ (30);
    
    _pidSeq->length (0);
    
    
    if (registeredDevices.size()==0)
        getRegisteredDevices(); //populate registeredDevices vector
    
    DEBUG(2, AppFact, "requiredComponents size - " << requiredComponents.size())

    for (unsigned int rc_idx = 0; rc_idx < requiredComponents.size (); rc_idx++) {
        ossieSupport::ComponentInfo *component = requiredComponents[rc_idx];
        
        DEBUG(1, AppFact, "Component - " << component->getName() << "   Assigned device - |" << component->getAssignedDeviceId() <<"|")
        
        CF::ExecutableDevice_ptr device = find_device_from_id(component->getAssignedDeviceId());
        
        DEBUG(1, AppFact, "Host is " << device->label () << " Local file name is " << component->getLocalFileName())
        
        //now we have a pointer to the required device
        //Get allocation properties
        //allocate
        device->allocateCapacity (component->getAllocationCapacities());
        
        //Get file name
        //load if it is not empty
        if (strlen (component->getLocalFileName()) >  0) {
            //load it
            //TODO: Validate that the intended device supports the
            //      LoadableDevice interface
     
            DEBUG(1, AppFact, "loading " << component->getLocalFileName() << " onto " << device->label());
            
	    CF::FileSystem_ptr fs = CF::FileSystem::_narrow(dmnMgr->fileMgr());
            device->load (fs, component->getLocalFileName(), component->getCodeType());
            
            //execute when necesary
            if (component->getCodeType() == CF::LoadableDevice::EXECUTABLE
                    || component->getCodeType() == CF::LoadableDevice::SHARED_LIBRARY
                    && strcmp (component->getEntryPoint(), "") != 0) {
                //execute
                //TODO: Validate that the intended device supports
                //      the LoadableDevice interface
                
                DEBUG(1, AppFact, "executing-> " << component->getLocalFileName())

		CF::DataType dt;

		CORBA::String_var waveformNameContextIOR = orb->orb->object_to_string(waveformContext);
		DEBUG(5, ApplicationFactory, "Passing: " << waveformNameContextIOR << " to component: " << component->getNamingServiceName());

		dt.id = "NAMING_CONTEXT_IOR";
		dt.value <<= waveformNameContextIOR;
		component->addExecParameter(&dt);

		dt.id = "NAME_BINDING";
		dt.value <<= component->getNamingServiceName();
		component->addExecParameter(&dt);

		dt.id = "COMPONENT_IDENTIFIER";
		dt.value <<= component->getIdentifier();
		component->addExecParameter(&dt);
                
                CF::ExecutableDevice::ProcessID_Type tempPid = device->
                        execute (component->getLocalFileName(),
                                 component->getOptions(),
                                 component->getExecParameters());
                if (tempPid < 0) {
                    //throw exception here
                } else {
		    component->setDeployedOnExecutableDevice(true);
		    component->setPID(tempPid);
		    component->setDevicePtr(device);

                    _pidSeq->length (_pidSeq->length() + 1);
                    (* _pidSeq)[_pidSeq->length() - 1].processId = tempPid;
                    (* _pidSeq)[_pidSeq->length() - 1].componentId =
                        CORBA::string_dup(component->getIdentifier());
                }
                //execDev->execute( LoadInfoVector[componentIndex]->entryPoint, LoadInfoVector[componentIndex]->options, LoadInfoVector[componentIndex]->parameters );
                //NOTE: The PID returned by execute is not handled
                //An exception shall be thrown when PID = -1
            }
        }
    }
    
    /// \todo Move this code into above loop


    CF::Resource_ptr _assemblyController;
    
    ELEM_SEQ* _namingCtxSeq = new ELEM_SEQ ();
    _namingCtxSeq->length (requiredComponents.size ());
    
    ELEM_SEQ* _implSeq = new ELEM_SEQ ();
    _implSeq->length (requiredComponents.size ());
    
    
    {  // this is here because Microsoft does not believe in following standards
    
        // install the different components in the system
        for (unsigned int rc_idx = 0; rc_idx < requiredComponents.size (); rc_idx++) {
	    ossieSupport::ComponentInfo *component = requiredComponents[rc_idx];

	    // this makes sure that the resource factory
	    // is not handled like a resource
            if (!component->getIsResource ())
                continue;
            
            (*_namingCtxSeq)[rc_idx].componentId =
                CORBA::string_dup (component->getIdentifier ());
                
            (*_implSeq)[rc_idx].componentId =
                CORBA::string_dup (component->getIdentifier ());
                
            // assuming 1 instantiation for each componentplacement
            if (component->getNamingService ()) {
                // this is for the naming-service based configuration
                // it assumes that the component already exists (like a device)
		std::string _lookupName = component->getNamingServiceName ();
		std::string tmpStr = waveformName + "/" + _lookupName;

                (*_namingCtxSeq)[rc_idx].elementId = CORBA::string_dup(tmpStr.c_str());

                CORBA::Object_ptr _obj = CORBA::Object::_nil ();
                
                // Wait for component to start
                for (unsigned int delay(50000);;) {
		    try {
			_obj = orb->get_object_from_name(waveformContext, _lookupName.c_str());
		    } catch (CosNaming::NamingContext::NotFound) {};

		    if (!CORBA::is_nil(_obj))
			break;
		    
		    ///\todo Figure out how to get rid of this sleep
		    ossieSupport::nsleep(0, delay);
		    
		    if (delay < 1000000)
			delay *= 2;
                }
                
                // check to see if the resource is the assembly controller
                //      either way, the resource is initialized and configured
		CF::Resource_ptr _rsc = CF::Resource::_narrow (_obj);
		component->setResourcePtr(_rsc);

                if (component->getIsAssemblyController()) {
                    _assemblyController = _rsc;
                    _assemblyController->initialize ();
                    _assemblyController->configure (initConfiguration);
                    _assemblyController->configure (component->getConfigureProperties());
                } else {
                    if (component->getIsResource () && component->getIsConfigurable ()) {
                        _rsc->initialize ();
                        _rsc->configure (component->getConfigureProperties());
                    }
                }
            }
#if 0 ///\todo Add support for resource factories 
	    else if ((*ComponentInstantiationVector)[0]->isResourceFactoryRef ()) {
                // resource-factory based component instantiation
                SADComponentInstantiation* _resourceFacInstantiation = NULL;
                
                int tmpCnt = 0;
                
                // figure out which resource factory is used
                while (_resourceFacInstantiation == NULL) {
                    _resourceFacInstantiation =
                        (*ComponentsVector)[tmpCnt]->
                        getSADInstantiationById ((*ComponentInstantiationVector)
                                                 [0]->getResourceFactoryRefId ());
                    tmpCnt++;
                }
                
                (*_namingCtxSeq)[i].elementId =
                    CORBA::string_dup (_resourceFacInstantiation->
                                       getFindByNamingServiceName ());
                                       
                CORBAOBJ _obj =
                    orb->get_object_from_name (_resourceFacInstantiation->
                                               getFindByNamingServiceName ());
                                               
                CF::ResourceFactory_ptr _resourceFactory =
                    CF::ResourceFactory::_narrow (_obj);
                    
                // configure factory
                /*std::vector < InstantiationProperty * >*_factoryInstantiationProps
                    = (*ComponentInstantiationVector)[0]->getFactoryProperties ();
                 
                CF::Properties _factoryProperties (_factoryInstantiationProps->
                    size ());
                 
                for (unsigned int j = 0; j < _factoryInstantiationProps->size ();
                    j++)
                {
                    _factoryProperties[j].id
                        =
                CORBA::string_dup ((*_factoryInstantiationProps)[j]->
                getID ());
                _factoryProperties[j].
                value <<= CORBA::
                string_dup ((*_factoryInstantiationProps)[j]->getValue ());
                }*/
                
                // unused PJB                   char *value = LoadInfoVector[vector_access]->name;
                
                // instantiate resource
                // (mechanism for this is left up to the factory's implementation)
                
                CF::Resource_ptr _resourceCreated =
                    _resourceFactory->
                    createResource (LoadInfoVector[vector_access]->name,
                                    LoadInfoVector[vector_access]->factoryParameters);
                                    
                if (CORBA::is_nil (_resourceCreated))
                    throw CF::ApplicationFactory::CreateApplicationError ();
                    
                // check to see if the resource is the assembly controller
                //      either way, the resource is initialized and configured
                if (strcmp (_assemblyControllerProfile->getID (),
                            (*ComponentInstantiationVector)[0]->getID ()) == 0) {
                    _assemblyController = _resourceCreated;
                    _assemblyController->initialize ();
                    _assemblyController->configure (initConfiguration);
                    _assemblyController->
                    configure (LoadInfoVector[vector_access]->configureProperties);
                } else {
                    _resourceCreated->initialize ();
                    _resourceCreated->
                    configure (LoadInfoVector[vector_access]->configureProperties);
                }
                vector_access++;
            } else {
                const char* _id, *_usagename;
                
                _id = LoadInfoVector[i]->implId;
                
                _usagename = LoadInfoVector[i]->name;
                
                char* _lookup = new char[strlen (_usagename) + strlen (_id) + 1];
                
                strcpy (_lookup, _usagename);
                strcat (_lookup, "_");
                strcat (_lookup, _id);
                
                (*_namingCtxSeq)[i].elementId = CORBA::string_dup (_lookup);
                
                CORBA::Object_ptr _obj = orb->get_object_from_name(_lookup);
                
                if (_assemblyControllerProfile == (*ComponentInstantiationVector)[0]) {
                    _assemblyController = CF::Resource::_narrow (_obj);
                    _assemblyController->initialize ();
                    _assemblyController->configure (initConfiguration);
                    _assemblyController->configure (LoadInfoVector[i]->
                                                    configureProperties);
                } else {
                    CF::Resource_ptr _rsc = CF::Resource::_narrow (_obj);
                    _rsc->initialize ();
                    _rsc->configure (LoadInfoVector[i]->configureProperties);
                }
                
                delete _lookup;
                _lookup = NULL;
            }
#endif // End resource factory support
        }                                         // end for()
    }
    
    std::vector < Connection * >*_connection = _sadParser->getConnections ();
    if (connectionData.size()!=0)
    	connectionData.clear();
    
    // create all resource connections
    for (int c_idx = _connection->size () - 1; c_idx >= 0; c_idx--) {
	Connection *connection = (*_connection)[c_idx];

	DEBUG(3, AppFact, "Processing connection " << connection->getID())

	// Process provides port
        CORBA::Object_var provides_port_ref;

	ProvidesPort* _providesPortParser;// = connection->getProvidesPort (); Not needed CRA
	FindBy * _findProvidesPortBy;
        CORBA::Object_var _providesObj;

        if (connection->isProvidesPort ()) {
	    DEBUG(3, AppFact, "Provides Port is provides port") 
            _providesPortParser = connection->getProvidesPort ();
	    if (_providesPortParser->isFindBy()) {
		DEBUG(3, AppFact, "Provides port is find by component name")
		FindBy* _findProvidesPortBy = _providesPortParser->getFindBy ();
		if (_findProvidesPortBy->isFindByNamingService ()) {
		    _providesObj = find_object_ref_from_sad_namingservice_name(_findProvidesPortBy->getFindByNamingServiceName(), waveformContext);
		}
	    } else if (_providesPortParser->isComponentInstantiationRef()) {
	        DEBUG(3, AppFact, "Provides port is find by componentinstantiationref")
		_providesObj = GetComponentReferenceFromInstanceRefId(
				_providesPortParser->getComponentInstantiationRefID());
#if 0
		DEBUG(3, AppFact, "Provides port is find by componentinstantiationref")
		for (unsigned int i=0; i < requiredComponents.size(); i++) {
		    ossieSupport::ComponentInfo *component = requiredComponents[i];
		    DEBUG(3, AppFact, "Looking for provides port component ID " << component->getIdentifier() << " ID from SAD " << _providesPortParser->getComponentInstantiationRefID())
		    if (strcmp(component->getIdentifier(), _providesPortParser->getComponentInstantiationRefID()) == 0) {
			_providesObj = component->getResourcePtr();
			break;
		    }
		    if (i == (requiredComponents.size() - 1)) {
			std::cout << "Provides port component not found" << std::endl;
			///\todo throw exception
		    }
		}
#endif
	    }
	}
        else if (connection->isFindBy ()) {
	    DEBUG(3, AppFact, "Provides Port is FindBy port name")
		_findProvidesPortBy = connection->getFindBy ();
	    if (_findProvidesPortBy->isFindByNamingService ()) {
		provides_port_ref = find_object_ref_from_sad_namingservice_name(_findProvidesPortBy->getFindByNamingServiceName (), waveformContext);
	    } else {
		std::cout << "Cannot find naming service name for FindBy provides port" << std::endl;
		/// \todo throw an exception?
	    }
	} else if(connection->isComponentSupportedInterface()){
            DEBUG(3, AppFact, "Provides Port is ComponentSupportedInterface")
            ComponentSupportedInterface* supportedInterface = connection->getComponentSupportedInterface();
            std::string supportedInterfaceId = supportedInterface->getID();
            DEBUG(3, AppFact, "Supported Interface is: " << supportedInterfaceId)
            //I am assuming that we have to compare this id with the contents of the SCD file
            // and that eventually we will have to narrow to that interface?
            if(supportedInterface->isFindBy()){
                ///\TODO Add support for FindBy option in ComponentSupportedInterface Connection
		
            } else {
		provides_port_ref = GetComponentReferenceFromInstanceRefId(
				supportedInterface->getComponentInstantiationRefId() );
            }
        }
       else {
	   std::cout << "Cannot find port information for provides port" << std::endl;
            /// \todo throw an exception?
        }
        
        
 
	// Find object ref for uses port
	// Process uses port

        UsesPort* _usesPortParser = connection->getUsesPort ();

	DEBUG(3, AppFact, "Uses port Identifier - " << _usesPortParser->getID())

        CORBA::Object_var _usesObj;

	if (_usesPortParser->isFindBy()) {
	    FindBy* _findUsesPortBy = _usesPortParser->getFindBy ();
	    if (_findUsesPortBy->isFindByNamingService ()) {
		_usesObj = find_object_ref_from_sad_namingservice_name(_findUsesPortBy->getFindByNamingServiceName(), waveformContext);
	    }
	} else if (_usesPortParser->isComponentInstantiationRef()) {
	    for (unsigned int i=0; i < requiredComponents.size(); i++) {
		ossieSupport::ComponentInfo *component = requiredComponents[i];
		if (strcmp(component->getIdentifier(), _usesPortParser->getComponentInstantiationRefID()) == 0) {
		    _usesObj = component->getResourcePtr();
		    break;
		}
		if (i == requiredComponents.size()) {
		    std::cout << "Uses port component not found" << std::endl;
		    ///\todo throw exception
		}
	    }
	} else {
	    std::cout << "Did not find method to get uses port" << std::endl;
	    ///\todo throw exception
	}
	
        
        /**************************************/
        /*                                    */
        /* Connection Establishment Rewritten */
        /*                                    */
        /**************************************/
        
        // CORBA object reference
        CORBA::Object_var uses_port_ref;
        
        // Output Uses Port Name
        const char* portName = _usesPortParser->getID();
        DEBUG(3, AppFact, "Getting Uses Port " << portName)
        
        // Get Uses Port
	CF::Resource_var _usesComp = CF::Resource::_narrow(_usesObj);
        uses_port_ref = _usesComp->getPort (_usesPortParser->getID());
	DEBUG(3, AppFact, "back from getport")
        CF::Port_ptr usesPort = CF::Port::_narrow (uses_port_ref);
	DEBUG(3, AppFact, "result from getport narrowed")
        if (CORBA::is_nil (usesPort)) {
	    std::cout << "getPort returned nil reference" << std::endl;
            throw CF::ApplicationFactory::CreateApplicationError();
        }
        DEBUG(3, AppFact, "Done with uses port")
        // Output Provides Port Name

	    DEBUG(3, AppFact, "Getting Provides Port ")
        
        // Get Provides Port
        // Note: returned object reference is NOT narrowed to a CF::Port
	//if (!connection->isFindBy ()){
        if (connection->isProvidesPort ()){
	    CF::Resource_ptr _providesResource;
	    DEBUG(3, AppFact, "Narrowing provides resource")
	    _providesResource = CF::Resource::_narrow (_providesObj);
	    DEBUG(3, AppFact, "Getting provides port with id - " << _providesPortParser->getID())
	    provides_port_ref = _providesResource->getPort (_providesPortParser->getID());
	}

        if (CORBA::is_nil (provides_port_ref)) {
	    std::cerr << "getPort returned nil or non-port reference" << std::endl;
            throw CF::ApplicationFactory::CreateApplicationError();
        }
        
        // Output ConnectionID
        DEBUG(3, AppFact, "Creating Connection " << connection->getID());
        
        // Create connection
        usesPort->connectPort (provides_port_ref, connection->getID());
        
	connectionData.push_back(new ossieSupport::ConnectionInfo(usesPort, connection->getID()));
    }
    
    // We are assuming that all components and their resources are collocated.
    // This means that we assume the SAD <partitioning> element contains the
    // <hostcollocation> element.
    Application_impl* _application = new Application_impl (_identifier.c_str(),
                                     name,
                                     _softwareProfile.c_str(),
                                     _availableDevs,     // need to provide support for device searching
                                     _implSeq,
                                     _namingCtxSeq,
                                     _pidSeq,            // need to provide support for PID SEQ
				     waveformName,
				     connectionData,
				     requiredComponents,
				     CF::Resource::_duplicate (_assemblyController),
                                     appseq,
				     _sadParser,
                                     waveformContext);
                                     
                                     
    /// \todo Pass object ref for application when application servant/ref stuff sorted out
    ossieSupport::sendObjAdded_event(_identifier.c_str(), _application->identifier(), _application->name(), (CORBA::Object_var) NULL, StandardEvent::APPLICATION);

    // Add a reference to the new application to the ApplicationSequence in DomainManager
    int old_length = appseq->length();
    appseq->length(old_length+1);
    (*appseq)[old_length] = CF::Application::_duplicate (_application->_this ());

    return CF::Application::_duplicate (_application->_this ());
    
}
; /* END ApplicationFactory_impl::create() */




CORBA::Object_ptr ApplicationFactory_impl::GetComponentReferenceFromInstanceRefId( const char* refId )
{
    DEBUG(3, AppFact, "Searching instantiationRefID for: " << refId)
    for (unsigned int i=0; i < requiredComponents.size(); i++) {
        ossieSupport::ComponentInfo *component = requiredComponents[i];
        DEBUG(3, AppFact, "Comparing with component instantiation ID: " << component->getIdentifier() )
     
	if (strcmp(component->getIdentifier(), refId) == 0) {
            return component->getResourcePtr();
        }
    }
    
    DEBUG(3, AppFact, "Component instantiation reference id NOT found")
    return CORBA::Object::_nil();
}


// Verify each component from the SAD exits in the device assignment sequence
void ApplicationFactory_impl::verifyDeviceAssignments (const CF::DeviceAssignmentSequence& _deviceAssignments)
{
    CF::DeviceAssignmentSequence badAssignments;
    badAssignments.length(requiredComponents.size());
    
    unsigned int notFound = 0;
    
    for (unsigned int i = 0; i < requiredComponents.size (); i++) {
        bool found = false;
        for (unsigned int j = 0; j < _deviceAssignments.length (); j++) {
            DEBUG(4, AppFact, "ComponentID - " << _deviceAssignments[j].componentId << "   Component - " << requiredComponents[i]->getIdentifier())
            if (strcmp (_deviceAssignments[j].componentId, requiredComponents[i]->getIdentifier()) == 0) {
                found = true;
                requiredComponents[i]->setAssignedDeviceId(_deviceAssignments[j].assignedDeviceId);
            }
        }
        if (!found) {
            std::cout << "ERROR: cannot find assignment " << requiredComponents[i]->getName() << std::endl;
            badAssignments[notFound++].componentId = CORBA::string_dup(requiredComponents[i]->getName());
        }
            
    }
    
    if (notFound > 0) {
	std::cout << "Device Assignment Sequence does not validate against the .sad file" << std::endl;
        throw CF::ApplicationFactory::CreateApplicationRequestError(badAssignments);
    }
        
}

void ApplicationFactory_impl::getRequiredComponents()
{

    std::vector <SADComponentPlacement*> *componentsFromSAD = _sadParser->getComponents();

    const char *assemblyControllerRefId = _sadParser->getAssemblyControllerRefId();

    for (unsigned int i = 0; i < componentsFromSAD->size(); i++) {
	SADComponentPlacement *component = (*componentsFromSAD)[i];
        ossieSupport::ComponentInfo *newComponent = new ossieSupport::ComponentInfo();



        // Extract required data from SPD file
	std::string SPDFileName = _sadParser->getSPDById(component->getFileRefId());
	DEBUG(6, ApplicationFactory, "About to parse SPD File: " << SPDFileName);
	CF::File_var spdFile = fileMgr->open(SPDFileName.c_str(), true);
        SPDParser spd(spdFile);
	spdFile->close();

	std::string scdPath;
	ossieSupport::spd_rel_file(SPDFileName.c_str(),spd.getSCDFile(), scdPath);
	DEBUG(6, ApplicationFactory, "About to parse SCD file: " << scdPath);
	CF::File_var scdFile = fileMgr->open(scdPath.c_str(), true);
        SCDParser scd(scdFile);
	scdFile->close();

        newComponent->setName(spd.getSoftPkgName());
	newComponent->setIsResource(scd.isResource());
	newComponent->setIsConfigurable(scd.isConfigurable());
        // Extract implementation data from SPD file
	std::vector <SPDImplementation *> *spd_i = spd.getImplementations();
        
        // Assume only one implementation, use first available result [0]
        newComponent->setImplId((*spd_i)[0]->getID());
        newComponent->setCodeType((*spd_i)[0]->getCodeType());
	newComponent->setLocalFileName((*spd_i)[0]->getCodeFile());
        newComponent->setEntryPoint((*spd_i)[0]->getEntryPoint());
        
        // Extract Properties
	std::string prfPath;
	CF::File_var prfFile = fileMgr->open(ossieSupport::spd_rel_file(SPDFileName.c_str(), spd.getPRFFile(), prfPath), true);

	DEBUG(6, ApplicationFactory, "About to parse PRF file: " << prfPath);

        PRFParser prf(prfFile);
	prfFile->close();
        
	DEBUG(6, ApplicationFactory, "PRF File parsed.");

        std::vector <PRFProperty *> *prop = prf.getFactoryParamProperties();
        for (unsigned int i=0; i < prop->size(); i++) {
            newComponent->addFactoryParameter((*prop)[i]->getDataType());
        }
        
        prop = prf.getExecParamProperties();
        for (unsigned int i=0; i < prop->size(); i++) {
            newComponent->addExecParameter((*prop)[i]->getDataType());
        }
        
        prop = prf.getMatchingProperties();
        for (unsigned int i=0; i < prop->size(); i++) {
            newComponent->addAllocationCapacity((*prop)[i]->getDataType());
        }
        
        prop = prf.getConfigureProperties();
        for (unsigned int i=0; i < prop->size(); i++) {
            newComponent->addConfigureProperty((*prop)[i]->getDataType());
        }
        

	// Extract Instantiation data from SAD
	DEBUG(6, ApplicationFactory, "Extract instantiation data from SAD.");

	// This is wrong, there can be more than one instantiation per placement
	// Basic fix, iterate over instantiations
	///\todo Fix for multiple instantiations per component
	std::vector <SADComponentInstantiation *> *instantiations = component->getSADInstantiations();
	
	SADComponentInstantiation *instance = (*instantiations)[0];

	newComponent->setIdentifier(instance->getID());

	if (strcmp(newComponent->getIdentifier(), assemblyControllerRefId) == 0)
	    newComponent->setIsAssemblyController(true);

	newComponent->setNamingService(instance->isNamingService());
	if (newComponent->getNamingService()) {
	    newComponent->setNamingServiceName(instance->getFindByNamingServiceName());
	}
    
	newComponent->setUsageName(instance->getUsageName());
	
	std::vector <InstantiationProperty *> *ins_prop = instance->getProperties();
	for (unsigned int i = 0; i < ins_prop->size(); ++i) {
	    DEBUG(3, AppFact, "Instantiation property id = " << (*ins_prop)[i]->getID())
		std::vector <std::string> ins_values = (*ins_prop)[i]->getValues();
	    newComponent->overrideProperty((*ins_prop)[i]->getID(), (*ins_prop)[i]->getValues());
	}


        requiredComponents.push_back(newComponent);
    }
    
}

void ApplicationFactory_impl::getRegisteredDevices ()
{

    CF::DomainManager::DeviceManagerSequence* devMgrs;
    devMgrs = dmnMgr->deviceManagers ();
    
    for (unsigned int i=0; i<devMgrs->length(); i++) {
        CF::DeviceSequence* devices = (*devMgrs)[i]->registeredDevices();
        for (unsigned int j=0; j<devices->length(); j++) {
            registeredDevices.push_back((*devices)[j]);
        }
    }
}

CF::ExecutableDevice_ptr ApplicationFactory_impl::find_device_from_id(const char *device_id)
{

    for (unsigned int i=0; i<registeredDevices.size(); i++) {
	DEBUG(5, ApplicationFactory, "Checking for match with device id : |" << registeredDevices[i]->identifier() << "|");
        if (strcmp(registeredDevices[i]->identifier(), device_id) == 0) {
            return CF::ExecutableDevice::_narrow(registeredDevices[i]);
        }
    }
    DEBUG(1, ApplicationFactory, "Device not found, this should never happen");
    assert(0); // Force program termination
    return 0; // Shut up warning
}


void ApplicationFactory_impl::create_waveform_nc_from_name(std::string &waveformName, CosNaming::NamingContext_ptr &waveformContext)
{

    std::string tmpStr = "DomainName1/" + waveformName;
    CosNaming::Name_var cosName = orb->string_to_CosName(tmpStr.c_str());

    DEBUG(5, ApplicationFactory, "Creating new waveform context: " << waveformName);

    waveformContext = orb->inc->bind_new_context(cosName);
}

/***
  If the name contains a /, resolve the full name using the root naming context.If there is no / in the name, resolve the name relative to the waveform naming context.  
*/

CORBA::Object_ptr ApplicationFactory_impl::find_object_ref_from_sad_namingservice_name(const char *name,  CosNaming::NamingContext_ptr &waveformContext)
{
    CORBA::Object_var obj;

    DEBUG(5, ApplicationFactory, "Looking SAD naming service name: " << name);

    if (strstr(name, "/") == NULL) 
	obj = orb->get_object_from_name(waveformContext, name);
    else
	obj = orb->get_object_from_name(name);

    return obj._retn();
}

char* ApplicationFactory_impl::softwareProfile () throw (CORBA::SystemException)
{
    std::string result;

    ossieSupport::createProfileFromFileName(_softwareProfile , result);
    return CORBA::string_dup(result.c_str());
}

#ifdef HAVE_OMNIEVENTS
ApplicationFactoryEventHandler::
ApplicationFactoryEventHandler (ApplicationFactory_impl * _appFac)
{
    appFactory = _appFac;
}


void
ApplicationFactoryEventHandler::push (const CORBA::Any & _any)
throw (CORBA::SystemException, CosEventComm::Disconnected)
{}


void
ApplicationFactoryEventHandler::disconnect_push_consumer ()
throw (CORBA::SystemException)
{}
#endif
