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
#include <iostream>
#include <string>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifndef HAVE_UNISTD_H
#include <process.h>
#endif

#include "ossie/debug.h"
#include "ossie/Application_impl.h"
#include "ossie/ossieSupport.h"
#include "ossie/portability.h"
#include "ossie/SADParser.h"

Application_impl::Application_impl (const char *_id,
const char *_name,
const char *_profile,
DEV_SEQ * _devSeq,
ELEM_SEQ * _implSeq,
ELEM_SEQ * _ncSeq,
PROC_ID_SEQ * _pidSeq,
std::string _waveformName,
std::vector <ossieSupport::ConnectionInfo *> _connectionData,
std::vector <ossieSupport::ComponentInfo *> _components,
CF::Resource_ptr _controller,
CF::DomainManager::ApplicationSequence *_appseq,
SADParser *sadParser, CosNaming::NamingContext_ptr wnc)
{
    id = _id;
    appName = _name;
    sadProfile = _profile;
    _sadParser = sadParser;
    connectionData = _connectionData;
    components = _components;
    orb = new ossieSupport::ORB();
    waveformName = _waveformName;
    waveformContext = wnc;

    appseq = _appseq;

    if (_devSeq != NULL)
    {
        this->appComponentDevices = new DEV_SEQ ();
        this->appComponentDevices->length (_devSeq->length ());

        for (unsigned i = 0; i < _devSeq->length (); i++)
            (*appComponentDevices)[i] = (*_devSeq)[i];
    }
    else
        this->appComponentDevices = new DEV_SEQ (0);

    if (_implSeq != NULL)
    {
        this->appComponentImplementations = new ELEM_SEQ ();
        this->appComponentImplementations->length (_implSeq->length ());

        for (unsigned int i = 0; i < _implSeq->length (); i++)
            (*appComponentImplementations)[i] = (*_implSeq)[i];
    }
    else
        this->appComponentImplementations = new ELEM_SEQ (0);

    if (_ncSeq != NULL)
    {
        this->appComponentNamingContexts = new ELEM_SEQ ();
        this->appComponentNamingContexts->length (_ncSeq->length ());

        for (unsigned int i = 0; i < _ncSeq->length (); i++)
            (*appComponentNamingContexts)[i] = (*_ncSeq)[i];
    }
    else
        this->appComponentNamingContexts = new ELEM_SEQ (0);

    if (_pidSeq != NULL)
    {
        this->appComponentProcessIds = new PROC_ID_SEQ ();
        this->appComponentProcessIds->length (_pidSeq->length ());

        for (unsigned int i = 0; i < _pidSeq->length (); i++)
            (*appComponentProcessIds)[i] = (*_pidSeq)[i];
    }
    else
        this->appComponentProcessIds = new PROC_ID_SEQ (0);

    this->assemblyController = _controller;
};

Application_impl::~Application_impl ()
{

    delete this->appComponentDevices;
    this->appComponentDevices = NULL;

    delete this->appComponentImplementations;
    this->appComponentImplementations = NULL;

    delete this->appComponentNamingContexts;
    this->appComponentNamingContexts = NULL;

    delete this->appComponentProcessIds;
    this->appComponentProcessIds = NULL;
};

void
Application_impl::start ()
throw (CORBA::SystemException, CF::Resource::StartError)
{
    assemblyController->start ();
}


void
Application_impl::stop ()
throw (CORBA::SystemException, CF::Resource::StopError)
{
    assemblyController->stop ();
}


void Application_impl::configure (const CF::Properties & configProperties) throw (CORBA::SystemException, CF::PropertySet::InvalidConfiguration, CF::PropertySet::PartialConfiguration)
{
}

void Application_impl::query (CF::Properties & configProperties) throw (CORBA::SystemException, CF::UnknownProperties)
{
}



void
Application_impl::initialize ()
throw (CORBA::SystemException, CF::LifeCycle::InitializeError)
{
    assemblyController->initialize ();
}


CORBA::Object_ptr
Application_impl::getPort (const char* _id)
throw (CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    return assemblyController->getPort (_id);
}


void
Application_impl::runTest (CORBA::ULong _testId, CF::Properties& _props)
throw (CORBA::SystemException, CF::UnknownProperties, CF::TestableObject::UnknownTest)

{
    assemblyController->runTest (_testId, _props);
}

/*
#ifdef  MS_dotNET
///\bug It is necessary to have this class because under VC7.1, self-destruct before the CORBA call is complete cannot be implemented

class ApplicationKiller : public ACE_Task_Base
{
    protected:
        Application_impl *local_pApp;

    public:
        virtual int svc (void)
        {
            ossieSupport::sleep(1);
            delete local_pApp;
            return 0;
        }

        void get_address(Application_impl *pApp)
        {
            local_pApp = pApp;
        }
};
#endif
*/

void
Application_impl::releaseObject ()
throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    DEBUG(3, Application, "Entering releaseObject");

    // Remove reference for this application from the ApplicationSequence in DomainManager
    int old_length = appseq->length();
    int index_found_app = -1;
    for (int i=0; i<old_length; i++) {
    	if (!strcmp((*(*appseq)[i]->componentNamingContexts())[0].elementId, (*appComponentNamingContexts)[0].elementId)) {
		index_found_app = i;
		break;
	}
    }
    if (index_found_app == -1) {
	std::cout << "The DomainManager's application list is corrupt" << std::endl;
    } else {
        for (int i=index_found_app; i<old_length-1; i++) {
    	    (*appseq)[i] = (*appseq)[i+1];
        }
        appseq->length(old_length-1);
    }

    std::vector <CF::Device_ptr> registeredDevices;

    assemblyController = CF::Resource::_nil ();

// break all connections in the application
    for (unsigned int i = 0; i < connectionData.size(); i++) {
	CF::Port_var port = connectionData[i]->getPort();
	DEBUG(5, Application, "Disconnecting port: " << connectionData[i]->getID());
	port->disconnectPort(connectionData[i]->getID());
    }

    DEBUG(4, Application, "app->releaseObject finished disconnecting ports")

// release all resources
    //Before releasing the components, all executed processes should be terminated,
    //all software components unloaded, and all capacities deallocated
    //search thru all waveform components
    //if there is a match with the deviceAssignmentSequence, then get a reference to the device
    //unload and deallocate

    CF::DomainManager_ptr dmnMgr;
    CORBA::Object_ptr obj = orb->get_object_from_name("DomainName1/DomainManager");
     dmnMgr = CF::DomainManager::_narrow (obj);

     CF::DomainManager::DeviceManagerSequence* devMgrs;
     devMgrs = dmnMgr->deviceManagers ();
     
     for (unsigned int i=0; i<devMgrs->length(); i++) {
         CF::DeviceSequence* devices = (*devMgrs)[i]->registeredDevices();
         for (unsigned int j=0; j<devices->length(); j++) {
             registeredDevices.push_back((*devices)[j]);
         }
     }

     // OLD COMMENT
//search thru all waveform components
//if there is a match with the deviceAssignmentSequence, then get a reference to the device
//unload and deallocate

     for (unsigned int i(0); i < components.size(); ++i) {
	 ossieSupport::ComponentInfo *component = components[i];

	 if (component->getDeployedOnResourceFactory()) {
	     ///\todo Write code for resource factory case
	     DEBUG(4, Application, "Releasing component deployed on resource factory.");

	 } else if (component->getDeployedOnLoadableDevice()) {
	     ///\todo Write code for loadable device case
	     DEBUG(4, Application, "Releasing component deployed on loadable device.");

	 } else if (component->getDeployedOnExecutableDevice()) {
	     DEBUG(4, Application, "Releasing component deployed on executable device.");

	     CF::Resource_ptr rsc = components[i]->getResourcePtr();
	     DEBUG(4, Application, "about to release Object");

	     rsc->releaseObject ();

	     CF::ExecutableDevice_var execDev = CF::ExecutableDevice::_narrow(component->getDevicePtr());

	     DEBUG(4, Application, "Unloading: " << component->getLocalFileName());
	     execDev->unload(component->getLocalFileName());

	     DEBUG(4, Application, "Terminating PID: " << component->getPID());
	     execDev->terminate (component->getPID());
	     execDev->deallocateCapacity(component->getAllocationCapacities());
	     
	 }
	 
	 DEBUG(4, App, "Finished releasing " << component->getUsageName());
	 
     }
     DEBUG(4, Application, "End of component list");

    
    std::string tc("DomainName1");
    CosNaming::Name_var rootNc = orb->string_to_CosName(tc.c_str());
    CORBA::Object_var rootObj = orb->inc->resolve(rootNc);
    CosNaming::NamingContext_var rootContext = CosNaming::NamingContext::_narrow(rootObj);
    CosNaming::Name waveformCosName;
    waveformCosName.length(1);
    waveformCosName[0].id = CORBA::string_dup(waveformName.c_str());

    DEBUG(4, Application, "Unbinding: " << waveformName);

    rootContext->unbind(waveformCosName);

    DEBUG(4, Application, "Destroying waveform naming context. ");

    waveformContext->destroy();

/*#ifdef  MS_dotNET

/// \bug this alternate structure is necessary to implement functionality that looks like a self-destruct

    ApplicationKiller * handler = new ApplicationKiller;

    handler->get_address(this);

    int result = handler->activate();

#else*/

// this is what the self-destruct should really look like
///\bug releaseObject code looks like example of what not to do pp492
//    delete this;
//#endif

}


char *Application_impl::name ()
throw (CORBA::SystemException)
{
    DEBUG(8, Application, "Returning name : " << appName);
    return CORBA::string_dup(appName.c_str());
}


char *Application_impl::identifier ()
throw (CORBA::SystemException)
{
    DEBUG(8, Application, "Returning identifier : " << id);
    return CORBA::string_dup(id.c_str());
}


char *Application_impl::profile ()
throw (CORBA::SystemException)
{
    DEBUG(8, Application, "Returning profile : " << sadProfile);
    return CORBA::string_dup(sadProfile.c_str());
}


PROC_ID_SEQ *
Application_impl::componentProcessIds ()
throw (CORBA::SystemException)
{
    CF::Application::ComponentProcessIdSequence_var result = new CF::Application::ComponentProcessIdSequence(*appComponentProcessIds);
    return result._retn();
    //return this->appComponentProcessIds;
}


ELEM_SEQ *
Application_impl::componentNamingContexts ()
throw (CORBA::SystemException)
{
    CF::Application::ComponentElementSequence_var result = new ELEM_SEQ(*appComponentNamingContexts);
    return result._retn();
    //return this->appComponentNamingContexts;
}


ELEM_SEQ *
Application_impl::componentImplementations ()
throw (CORBA::SystemException)
{
    CF::Application::ComponentElementSequence_var result = new ELEM_SEQ(*appComponentImplementations);
    return result._retn();
    //return this->appComponentImplementations;
}


CF::DeviceAssignmentSequence *
Application_impl::componentDevices ()throw (CORBA::SystemException)
{
    CF::DeviceAssignmentSequence_var result = new CF::DeviceAssignmentSequence(*appComponentDevices);
    return result._retn();
    //return this->appComponentDevices;
}
