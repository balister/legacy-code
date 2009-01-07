/****************************************************************************

Copyright 2009, Philip Balister philip@opensdr.com

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

#ifndef REALSTRING_U_H
#define REALSTRING_U_H

#include <string>
#include <vector>

#ifdef HAVE_OMNIORB4
#include "omniORB4/CORBA.h"
#endif

#include <omnithread.h>

#include <ossie/cf.h>

#include <standardinterfaces/realString.h>

// Forward declaration
namespace realString {

  class usesPort;
  class ConnectionInfo;
}

namespace standardInterfaces_i {

  class realString_u {
    friend class realString::usesPort;

  public:
    realString_u(const char* portName);
    realString_u(const char* portName, const char* domainName);
    ~realString_u();

    CORBA::Object_ptr getPort(const char* portName);

    void pushPacket(std::string &I);

  private:
    realString_u();
    realString_u(const realString_u &);

    std::string portName;

    // Uses port
    realString::usesPort *data_servant;
    CF::Port_var data_servant_var;
    std::vector <realString::ConnectionInfo> dest_ports;
    omni_mutex port_mutex;

  };

}

namespace realString {

  class usesPort : public virtual POA_CF::Port {
  public:
    usesPort(standardInterfaces_i::realString_u *_base);
    ~usesPort();

    void connectPort(CORBA::Object_ptr connection, const char* connectionID);
    void disconnectPort(const char* connectionID);

  private:
    standardInterfaces_i::realString_u *base;
  };


  class ConnectionInfo
    {
    public:
      ConnectionInfo(standardInterfaces::realString_ptr _port, const char* _ID) : port_obj( _port), identifier(_ID) { };
      ConnectionInfo(const ConnectionInfo & c) { port_obj = c.port_obj; identifier = c.identifier;};
      
      void setPort(standardInterfaces::realString_ptr _port) {port_obj = _port; };
      const char *getID() {return identifier.c_str(); };

      standardInterfaces::realString_var port_obj;

    private:
      ConnectionInfo();  // No default constructor

      std::string identifier;
    };

}

#endif
