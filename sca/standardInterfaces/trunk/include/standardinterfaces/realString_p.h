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

#ifndef REALSTRING_P_H
#define REALSTRING_P_H

#include <string>
#include <vector>

#include <standardinterfaces/realString.h>

namespace realString {
  class providesPort;
}

namespace standardInterfaces_i {
  class realString_p {
    friend class realString::providesPort;

  public:
    realString_p(const char* portName, unsigned int bufLen = 5);
    realString_p(const char* portName, const char* domainName, unsigned int bufLen = 5);
    ~realString_p();

    CORBA::Object_ptr getPort(const char* portName);

    void getData(std::string *I);
    void bufferEmptied();

  private:
    realString_p();
    realString_p(const realString_p &);

    std::string portName;

    // Provides port
    realString::providesPort *data_servant;
    standardInterfaces::realString_var data_servant_var;

    // Buffer storage
    unsigned int bufferLength;
    unsigned int rdPtr;
    unsigned int wrPtr;
    std::vector <std::string> I_buf;
 
    // Semaphores for synchronization
    omni_semaphore *data_ready;  // Ready to process data
    omni_semaphore *ready_for_input; // Ready to receive more data

 };

}

namespace realString {

  class providesPort : public POA_standardInterfaces::realString {
  public:
    providesPort(standardInterfaces_i::realString_p* _base);
    ~providesPort();

    void pushPacket(const char* in);

  private:
    standardInterfaces_i::realString_p* base;
  };
}


#endif
