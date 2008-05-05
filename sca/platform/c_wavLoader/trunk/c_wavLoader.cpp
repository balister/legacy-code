
/*******************************************************************************
 
Copyright 2006, 2007 Virginia Polytechnic Institute and State University
 
This file is part of the OSSIE  waveform loader.
 
OSSIE Waveform Loader is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
 
OSSIE Sample Waveform is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with OSSIE Waveform loader; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
 
*******************************************************************************/

#include <iostream>
#include <string>

#include "ossie/cf.h"

#include "DASParser.hh"

static CF::FileManager_ptr _DM_fm;

static void display_menu(int state, CF::FileSystem::FileInformationSequence* files)

{

  std::cout << std::endl;

  switch (state) {
  case 0:   // No installed applications
    for (unsigned int i=0; i<files->length(); i++) {
	std::cout << "  " << i+1 << ":  " << (*files)[i].name << std::endl;
    }
    std::cout << "n - Install application number n" << std::endl;
    std::cout << "x - exit" << std::endl;
    break;

  case 1:   // Application installed and not running
    std::cout << "s - start application" << std::endl;
    std::cout << "u - uninstall application" << std::endl;
    break;

  case 2:   // Application running
    std::cout << "t - stop application" << std::endl;
    break;    
  }

  std::cout << "Selection: " << std::flush;
}

static CF::DeviceAssignmentSequence *read_das(const char *name_SAD)
{

  std::string inStr = name_SAD;

  std::string::size_type pos = inStr.find(".sad.xml");
  std::string::size_type begin = inStr.find_last_of("/");

  std::cout << "pos = " << pos <<  " Base name - " << inStr.substr(begin+1,pos-begin-1) << std::endl;

  std::string DAS_name = "/waveforms/" + inStr.substr(begin+1,pos-begin-1) + "_DAS.xml";
  std::cout << "DAS name: " << DAS_name << std::endl;

  CF::File_var das_file = _DM_fm->open(DAS_name.c_str(), true);

  DASParser das(das_file);

  das_file->close();

  return das.das();

}


static CF::Application_ptr install_app (int app_no, CF::FileSystem::FileInformationSequence* files, CF::DomainManager_var domMgr)

{
    CF::Properties _appFacProps( 0 );
    CF::Application_ptr app = CF::Application::_nil();
    
    std::cout << "In install_app with app_no: " << app_no << std::endl;
    
    // find the application associated with requested SAD
    
    std::string name_SAD("/waveforms/");
    
    name_SAD += (*files)[app_no].name;
    
    // install application in DM")
    
    try {
	domMgr->installApplication( name_SAD.c_str() );
	std::cout << name_SAD << " successfully installed onto Domain Manager\n";
	
	//Specify what component should be deployed on particular devices
	
	CF::DeviceAssignmentSequence *das;
	
	das = read_das(name_SAD.c_str());
	
	// local variable for list of Application Factories returned by DM
	
	CF::DomainManager::ApplicationFactorySequence *_applicationFactories;
	// get the list of available Application Factories from DM
	_applicationFactories = domMgr->applicationFactories();
	
	// create Application from Application Factory
	std::cout << "App fact seq length = " << _applicationFactories->length() << std::endl;
	std::cout << "_applicationFactories[0]->name()" << std::endl;
	std::cout << (*_applicationFactories)[ 0 ] ->name() << std::endl;
	std::cout << "Calling appfactory->create now" << std::endl;
	
	app = (*_applicationFactories)[ 0 ] ->create( (*_applicationFactories)[ 0 ] ->name(), _appFacProps, *das );
	
	std::cout << "Application created.  Ready to run or uninstall\n";
	
	return(app);
    } catch (CF::DomainManager::ApplicationInstallationError ex) {
	std::cout << "Install application failed with message: " << ex.msg << std::endl;
	
	return(NULL);
    }
    
}

int main( int argc, char** argv )

{
  CORBA::ORB_var orb = CORBA::ORB_init( argc, argv );
  CosNaming::NamingContext_var rootContext;
  
  //   Open_DM myDomainManager( argc, argv );
  //cout << "Domain Manager successfully instantiated" << endl;
  
  try {
    CORBA::Object_var obj;
    obj = orb->resolve_initial_references("NameService");
    
    rootContext = CosNaming::NamingContext::_narrow(obj);
    if (CORBA::is_nil(rootContext)) {
      std::cerr << "Failed to narrow the root naming context." << std::endl;
    }
  } catch(CORBA::ORB::InvalidName & ) {
    std::cerr << "This shouldn't happen" << std::endl;
    exit (-1);
  }
  
  CORBA::Object_ptr obj;
  CosNaming::Name name;
  
  name.length(2);
  
  // Set the radio context first
  name[0].id = (const char *) "DomainName1";
  
  
  // Find DomainManager
  name[1].id = (const char *) "DomainManager";
  obj = rootContext->resolve(name);
  
  CF::DomainManager_var domMgr = CF::DomainManager::_narrow(obj);
  
  
  
  // access to the file manager is needed to figure out what applications are available
  _DM_fm = domMgr->fileMgr();
  
  // Generate a list of available waveforms by findind all the _SAD files
  CF::FileSystem::FileInformationSequence* _fileSeq = _DM_fm->list( "/waveforms/*.sad.xml" );
  int maximum_number_applications = _fileSeq->length();
  
  // create handle to application
  CF::Application_ptr app = CF::Application::_nil();
    
  
  // interface control loop
  
  int state = 0;

  char ch_hit = 0;
  
  do {

    if (ch_hit != 10) {
      display_menu(state, _fileSeq);
    }
    
    ch_hit = std::cin.get();
    
	
    switch (state) {
    case 0:
      if (ch_hit == 'x') {
	state = 99;
      } else if ((ch_hit >= '1') && (ch_hit <= ('1' + maximum_number_applications - 1))) {
	  if ((app = install_app(ch_hit - '1', _fileSeq, domMgr)))
	  state = 1;

      }
      break;
      
    case 1:
      if (ch_hit == 'u') {
	domMgr->uninstallApplication( app->identifier() );
	
	std::cout << "Application uninstalled from Domain Manager" << std::endl;
	
	// destroy all components associated with Application
	app->releaseObject();
	app = CF::Application::_nil();
	std::cout << "Application destroyed" << std::endl;
	state = 0;
	
      } else if (ch_hit == 's') {
	app->start();
	state = 2;
      }
      break;
      
    case 2:
      if (ch_hit == 't') {
	app->stop();
	state = 1;
      }
      break;
      
    default:
      std::cerr << "Hit default in keypress switch, this should never happen" << std::endl;
    }
  } while (state != 99);
  
  return 0;
};

