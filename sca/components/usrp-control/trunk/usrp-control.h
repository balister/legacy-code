/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University
Copyright 2008 Philip Balister, philip@opensdr.com

This file is part of the usrp-control.

usrp-control is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

usrp-control is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with usrp-control; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#ifndef USRP_CONTROL_IMPL_H
#define USRP_CONTROL_IMPL_H

#include <stdlib.h>

#include "ossie/cf.h"
#include "ossie/PortTypes.h"

#include "standardinterfaces/Radio_Control_u.h"
#include "standardinterfaces/Resource_u.h"

#include "ossie/Resource_impl.h"

#define DEFAULT_USRP_RX_CHANNEL 0
#define DEFAULT_USRP_TX_CHANNEL 0

class usrp_control_i : public virtual Resource_impl
{
  public:
    usrp_control_i(const char *uuid, omni_condition *sem);

    void start() throw (CF::Resource::StartError, CORBA::SystemException);
    void stop() throw (CF::Resource::StopError, CORBA::SystemException);

    CORBA::Object_ptr getPort( const char* portName )
        throw (CF::PortSupplier::UnknownPort, CORBA::SystemException);

    void releaseObject()
        throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);

    void initialize()
        throw (CF::LifeCycle::InitializeError, CORBA::SystemException);

    void configure(const CF::Properties&)
        throw (CORBA::SystemException,
               CF::PropertySet::InvalidConfiguration,
               CF::PropertySet::PartialConfiguration);
    void query (CF::Properties & configProperties) throw (CORBA::SystemException, CF::UnknownProperties);
    
    void runTest (CORBA::ULong _number, CF::Properties & _props) throw (CORBA::SystemException, CF::TestableObject::UnknownTest, CF::UnknownProperties);

  private:
    usrp_control_i();
    usrp_control_i(usrp_control_i&);

    void setup_rx_usrp(); // Send properties to USRP RX
    void setup_tx_usrp(); // Send properties to USRP TX

    omni_condition *component_running;  ///< for component shutdown

    omni_mutex accessPrivateData;       ///< mutex lock for accessing private data

    // USRP TX properties
    short tx_interp;    ///< TX interpolation factor
    float tx_freq;      ///< TX frequency
    float tx_gain;      ///< TX gain
    bool  tx_start;     ///< Start transmitter flag

    // USRP RX properties
    short rx_decim;     ///< RX decimation factor
    short rx_nchan;     ///< Number of active receiver channels
    float rx_freq1;     ///< RX frequency, channel 1
    float rx_freq2;     ///< RX frequency, channel 2
    float rx_gain;      ///< RX gain
    unsigned long  rx_size;      ///< RX packet size
    bool  rx_start;     ///< Start receiver flag

    
    /// Transmit control port for configuring the following on the USRP:
    ///   - transmit frequency, \ref prop_tx_freq
    ///   - transmit interpolation factor, \ref prop_tx_interp
    standardInterfaces_i::TX_Control_u *TXControl;

    /// Receive control port for configuring the following on the USRP:
    ///   - receive frequency, \ref prop_rx_freq
    ///   - receive decimator factor, \ref prop_rx_decim
    standardInterfaces_i::RX_Control_u *RXControl;

    /// Resource port for data control.  If the the assembly controller,
    /// calling start() on the USRP_Commander component will in turn
    /// call start() on all the remote resource ports
    standardInterfaces_i::Resource_u *data_control;

};
#endif

