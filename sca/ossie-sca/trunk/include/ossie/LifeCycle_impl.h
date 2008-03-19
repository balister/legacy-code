/****************************************************************************

Copyright 2004, Virginia Polytechnic Institute and State University

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

#ifndef LIFECYCLE_IMPL_H
#define LIFECYCLE_IMPL_H

#include "ossiecf.h"

#ifdef MS_dotNET
#pragma warning( disable : 4275 ) // this could be something but i'm not sure	
#pragma warning( disable : 4290 ) // there's an annoying VC7 warning that I wanted to make go away
#pragma warning( disable : 4251 ) // this is for dll interfaces
#endif

#include "cf.h"


/**
The LifeCycle interface defines generic operations for component
initialization and and releasing instantiated objects.

 */

class OSSIECF_API LifeCycle_impl:public virtual
POA_CF::LifeCycle
{
    public:LifeCycle_impl ()
    {
    };

    /// Override this method with component specific initialization.
    void initialize ()
        throw (CF::LifeCycle::InitializeError, CORBA::SystemException);

    /// Override this method with the code require to release the object.
    void releaseObject ()
        throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);
};
#endif                                            /*  */
