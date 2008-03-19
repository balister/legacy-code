
/****************************************************************************
 
Copyright 2008 Virginia Polytechnic Institute and State University
 
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

#ifndef OSSIERESOURCE_H
#define OSSIERESOURCE_H

#include "ossie/ossieSupport.h"
#include "ossie/debug.h"


/* This template uses the command pattern to create the code required to
 * instantiate an SCA resource from the implementation class of the resource
 * typename T is replaced with the actual resource implementation. 
 */

template <typename T>
class ossieResource
{
  public:
    ossieResource(int argc, char *argv[]);

    ~ossieResource();

    void run ();

  private:
    ossieSupport::ORB *orb;
    omni_mutex component_running_mutex;
    omni_condition *component_running;

    ossieSupport::ossieComponent *resource;
    T* resource_servant;
    CF::Resource_var resource_var;


};

template <typename T>
ossieResource< T >::ossieResource(int argc, char *argv[])
{

    orb = new ossieSupport::ORB;
    component_running = new omni_condition(&component_running_mutex);

    DEBUG(5, ossieResourceT, "Creating ossieComponent.");
    resource = new ossieSupport::ossieComponent(orb, argc, argv);

    resource_servant = new T(resource->getUuid(), component_running);
    resource_var = resource_servant->_this();

    resource_servant->post_constructor(resource_var); 

    resource->bind(resource_var);


}

template <typename T>
void ossieResource< T >::run()
{
    component_running->wait();
}

template <typename T>
ossieResource< T >::~ossieResource()
{

    PortableServer::ObjectId_var servantId = orb->poa->reference_to_id(resource_var);

    resource->unbind();

    orb->poa->deactivate_object(servantId);
    orb->poa->destroy(false, false);

    delete component_running;
    delete resource_servant;
    delete resource;

    orb->orb->shutdown(0);

    delete orb;
}

#endif
