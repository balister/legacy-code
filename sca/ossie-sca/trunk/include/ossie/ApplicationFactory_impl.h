/****************************************************************************
 
Copyright 2004, 2007 Virginia Polytechnic Institute and State University
 
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

#ifndef APPLICATIONFACTORY_H
#define APPLICATIONFACTORY_H

#include <string>

#include "cf.h"
#include "StandardEvent.h"

#include "ossiecf.h"
#include "FileSystem_impl.h"
#include "Application_impl.h"
#include "SADParser.h"
#include "SPDImplementation.h"
#include "PRFParser.h"
#include "SCDParser.h"


#ifdef HAVEOMNIORB4
#include <omniORB4/CORBA.h>
#endif
#ifdef HAVE_OMNIEVENTS
#include <CosEventComm.hh>
#endif

class OSSIECF_API ApplicationFactory_impl;

#ifdef HAVE_OMNIEVENTS
class OSSIECF_API ApplicationFactoryEventHandler:public virtual
            POA_CosEventComm::PushConsumer
{
private:
    ApplicationFactory_impl* appFactory;
    
public:
    ApplicationFactoryEventHandler (ApplicationFactory_impl* _appFac);
    virtual void push (const CORBA::Any &)
    throw (CosEventComm::Disconnected, CORBA::SystemException);
    
    virtual void disconnect_push_consumer ()
    throw (CORBA::SystemException);
};
#endif



class OSSIECF_API
            ApplicationFactory_impl:
            public
            virtual
            POA_CF::ApplicationFactory
{
#ifdef HAVE_OMNIEVENTS
    friend void ApplicationFactoryEventHandler::push (const CORBA::Any &);
    
    friend void ApplicationFactoryEventHandler::disconnect_push_consumer ();
#endif
    
private:
    ApplicationFactory_impl();  // Node default constructor
    ApplicationFactory_impl(ApplicationFactory_impl&);  // No copying
    

    
    std::string _name;
    std::string _identifier;
    std::string _softwareProfile;
    
    CF::FileManager_var fileMgr;
    SADParser* _sadParser;
    
    CF::DomainManager_ptr dmnMgr;
    std::vector <CF::Device_ptr> registeredDevices;
    std::vector <ossieSupport::ConnectionInfo *> connectionData;
    
    void verifyDeviceAssignments (const CF::DeviceAssignmentSequence & _deviceAssignments);
    void getRegisteredDevices();
    
    ossieSupport::ORB *orb;
    
    
    std::vector <ossieSupport::ComponentInfo *> requiredComponents;

    CF::DomainManager::ApplicationSequence * appseq;
    
    // Private member functions
    void getRequiredComponents();
    CF::ExecutableDevice_ptr find_device_from_id(const char *);
    void create_waveform_nc_from_name(std::string &waveformName, CosNaming::NamingContext_ptr &waveformContext);
    CORBA::Object_ptr find_object_ref_from_sad_namingservice_name(const char *name,  CosNaming::NamingContext_ptr &waveformContext);
    CORBA::Object_ptr GetComponentReferenceFromInstanceRefId( const char* );
    
public:
    ApplicationFactory_impl (SADParser *sadParser, CF::FileManager_ptr fileMgr, CF::DomainManager::ApplicationSequence *_appseq);
    ~ApplicationFactory_impl ();
    
    CF::Application_ptr create (const char *name,
                                const CF::Properties & initConfiguration,
                                const CF::DeviceAssignmentSequence & deviceAssignments)
    throw (CF::ApplicationFactory::InvalidInitConfiguration,
           CF::ApplicationFactory::CreateApplicationRequestError,
           CF::ApplicationFactory::CreateApplicationError,
           CORBA::SystemException);
           
    char* name () throw (CORBA::SystemException)
    {
        return CORBA::string_dup(_name.c_str());
    }
    
    char* identifier () throw (CORBA::SystemException)
    {
        return CORBA::string_dup(_identifier.c_str());
    }
    
    char* softwareProfile () throw (CORBA::SystemException);
};
#endif
