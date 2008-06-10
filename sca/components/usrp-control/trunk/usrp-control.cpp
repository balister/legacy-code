/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE USRP_Commander.

OSSIE USRP_Commander is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE USRP_Commander is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE USRP_Commander; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#include <string>
#include <iostream>

#include "ossie/cf.h"
#include "ossie/PortTypes.h"
#include "ossie/debug.h"

#include "USRP_Commander.h"

#include "ossie/Resource_impl.h"

USRP_Commander_i::USRP_Commander_i(const char *uuid, omni_condition *condition) :
    Resource_impl(uuid),
    component_running(condition),
    tx_interp(0),
    tx_freq(0),
    tx_gain(0),
    tx_start(false),
    rx_decim(0),
    rx_gain_max(0),
    rx_freq(0),
    rx_gain(0),
    rx_size(0),
    rx_start(false)
{
    // Create port instances
    TXControl = new standardInterfaces_i::TX_Control_u("TX_Control");
    RXControl = new standardInterfaces_i::RX_Control_u("RX_Control");
    data_control = new standardInterfaces_i::Resource_u("Data_Control");
}

CORBA::Object_ptr USRP_Commander_i::getPort( const char* portName )
    throw (CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, USRP_Commander, "getPort called with : " << portName)
    
    CORBA::Object_var p;

    // Check TXControl
    p = TXControl->getPort(portName);
    if (!CORBA::is_nil(p))
        return p._retn();

    // Check RXControl
    p = RXControl->getPort(portName);
    if (!CORBA::is_nil(p))
        return p._retn();

    // Check data_control
    p = data_control->getPort(portName);
    if (!CORBA::is_nil(p))
        return p._retn();

    std::cerr << "USRP Commander::getPort() unknown port: " << portName << std::endl;
    throw CF::PortSupplier::UnknownPort();

    // This will never happen, but gcc complains if nothing is returned at this point
    return p;
}

void USRP_Commander_i::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    DEBUG(3, USRP_Commander, "start called on USRP_Commander")

    // Call start on remote resources
    CF::Resource_var r = data_control->getRef();
    if (!CORBA::is_nil(r))
        r->start();

    //-----------------------------------------------------------------------------
    //
    // Set TX properties
    //
    //-----------------------------------------------------------------------------

    // Initialize to default TX values
    TXControl->set_number_of_channels(1);
    TXControl->set_gain(DEFAULT_USRP_TX_CHANNEL, tx_gain);
    TXControl->set_frequency(DEFAULT_USRP_TX_CHANNEL, tx_freq);
    TXControl->set_interpolation_rate(DEFAULT_USRP_TX_CHANNEL, tx_interp);

    // Set transmit configurable properties not included in Radio_Control idl
    CF::Properties tx_config;
    tx_config.length(1);

    // Set automatic transmit/receive mode on
    tx_config[0].id = CORBA::string_dup("SET_AUTO_TR_1");
    tx_config[0].value <<= (CORBA::ULong) 1;

    TXControl->set_values(tx_config);

    //-----------------------------------------------------------------------------
    //
    // Set RX properties
    //
    //-----------------------------------------------------------------------------

    // Initialize to default RX values
    RXControl->set_number_of_channels(1);
    RXControl->set_gain(DEFAULT_USRP_RX_CHANNEL, rx_gain);
    RXControl->set_frequency(DEFAULT_USRP_RX_CHANNEL, rx_freq);
    RXControl->set_decimation_rate(DEFAULT_USRP_RX_CHANNEL, rx_decim);
    RXControl->set_data_packet_size(DEFAULT_USRP_RX_CHANNEL, rx_size);

    // Set transmit configurable properties not included in Radio_Control idl
    CF::Properties rx_config;
    rx_config.length(1);

    // Set rx antenna
    rx_config[0].id = CORBA::string_dup("SET_RX_ANT_1");
    rx_config[0].value <<= (CORBA::ULong) 0;

    RXControl->set_values(rx_config);

    if (tx_start) {
        DEBUG(3, USRP_Commander, "starting USRP transmit process...");
        TXControl->start(DEFAULT_USRP_TX_CHANNEL);
    }

    if (rx_start) {
        RXControl->start(DEFAULT_USRP_RX_CHANNEL);
        DEBUG(3, USRP_Commander, "starting USRP receive process...");
    }

}

void USRP_Commander_i::stop() throw (CORBA::SystemException, CF::Resource::StopError) 
{  
    DEBUG(3, USRP_Commander, "stop called on USRP_Commander")

    // Call stop on remote resources
    CF::Resource_var r = data_control->getRef();
    if (!CORBA::is_nil(r))
        r->stop();

    RXControl->stop(DEFAULT_USRP_RX_CHANNEL);
    TXControl->stop(DEFAULT_USRP_TX_CHANNEL);
}

void USRP_Commander_i::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    DEBUG(3, USRP_Commander, "releaseObject called on USRP_Commander")
    
    component_running->signal();
}

void USRP_Commander_i::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException)
{
    DEBUG(3, USRP_Commander, "initialize called on USRP_Commander")
    
    // Initialize USRP TX properties
    tx_interp = 256;        // TX interpolation factor
    tx_freq = 475000000;    // TX frequency
    tx_gain = 1;            // TX gain
    tx_start = false;       // Start transmitter flag

    // Initialize USRP RX properties
    rx_decim = 256;         // RX decimation factor
    rx_freq = 485000000;    // RX frequency
    rx_gain = 1;            // RX gain
    rx_size = 1024;         // RX packet size
    rx_start = false;       // Start receiver flag

}



void USRP_Commander_i::configure(const CF::Properties& props)
    throw (CORBA::SystemException,
           CF::PropertySet::InvalidConfiguration,
           CF::PropertySet::PartialConfiguration)
{
    DEBUG(3, USRP_Commander, "configure called on USRP_Commander")
    
    DEBUG(3, USRP_Commander, "props length : " << props.length())

    RXControl->set_number_of_channels(1);

    for (unsigned int i = 0; i < props.length(); i++) {
        DEBUG(3, USRP_Commander, "Property id : " << props[i].id)

        if (strcmp(props[i].id, "DCE:3efc3930-2739-40b4-8c02-ecfb1b0da9ee") == 0) {
            // RX Frequency
            CORBA::Float F;
            props[i].value >>= F;
            DEBUG(3, USRP_Commander, "RX Frequency property= " << F)
            rx_freq = F;
            RXControl->set_frequency(DEFAULT_USRP_RX_CHANNEL, rx_freq);
        } else if (strcmp(props[i].id, "DCE:6a2d6952-ca11-4787-afce-87a89b882b7b") == 0) {
            // TX Frequency
            CORBA::Float F;
            props[i].value >>= F;
            DEBUG(3, USRP_Commander, "TX Frequency property= " << F)
            tx_freq = F;
            TXControl->set_frequency(DEFAULT_USRP_TX_CHANNEL, tx_freq);
        } else if (strcmp(props[i].id, "DCE:9ca12c0e-ba65-40cf-9ef3-6e7ac671ab5d") == 0) {
            // Transmitter Interpolation Factor
            CORBA::Short M;
            props[i].value >>= M;
            DEBUG(3, USRP_Commander, "TX Interpolation Factor property= " << M)
            tx_interp = M;
            TXControl->set_interpolation_rate(DEFAULT_USRP_TX_CHANNEL, tx_interp);
        } else if (strcmp(props[i].id, "DCE:92ec2b80-8040-47c7-a1d8-4c9caa4a4ed2") == 0) {
            // RX Decimation factor
            CORBA::Short D;
            props[i].value >>= D;
            DEBUG(3, USRP_Commander, "RX Decimation Factor property= " << D)
            rx_decim = D;
            RXControl->set_decimation_rate(DEFAULT_USRP_RX_CHANNEL, rx_decim);
        } else if (strcmp(props[i].id, "DCE:93324adf-14f6-4406-ba92-a3650089857f") == 0) {
            // RX Data Packet size
            CORBA::ULong L;
            props[i].value >>= L;
            DEBUG(3, USRP_Commander, "RX Data Packet size property= " << L)
            rx_size = L;
            RXControl->set_data_packet_size(DEFAULT_USRP_RX_CHANNEL, rx_size);
        } else if (strcmp(props[i].id, "DCE:99d586b6-7764-4dc7-83fa-72270d0f1e1b") == 0) {
            //Rx Gain
            CORBA::Float G;
            props[i].value >>= G;
            DEBUG(3, USRP_Commander, "RX Gain property= " << G)
            rx_gain = G;
            RXControl->set_gain(DEFAULT_USRP_RX_CHANNEL, rx_gain);
        } else if (strcmp(props[i].id, "DCE:2d9c5ee4-a6f3-4ab9-834b-2b5c95818e53") == 0) {
            //Rx Gain Max
            ///\todo check gain values, perhaps get rid of this property
            CORBA::Short v;
            props[i].value >>= v;
            DEBUG(3, USRP_Commander, "RX Gain Max property= " << v)
            DEBUG(3, USRP_Commander, "  ::::  WARNING: RX Gain Max property ignored!  ::::")
            rx_gain_max = v;
        } else if (strcmp(props[i].id, "DCE:fd42344f-4d87-465b-9e6f-e1d7ae48afd6") == 0) {
            // rx_start
            CORBA::Short v;
            props[i].value >>= v;
            rx_start = (v==0) ? false : true;
            DEBUG(3, USRP_Commander, "RX start set  to " << rx_start)
            //if (rx_start)
            //    RXControl->start(DEFAULT_USRP_RX_CHANNEL);
        } else if (strcmp(props[i].id, "DCE:0a9b8c8c-f130-4a8f-9ef8-bba023128a4b") == 0) {
            // tx_start
            CORBA::Short v;
            props[i].value >>= v;
            tx_start = (v==0) ? false : true;
            DEBUG(3, USRP_Commander, "TX Start set to " << tx_start)
            //if (tx_start)
            //    TXControl->start(DEFAULT_USRP_TX_CHANNEL);
        } else {
            std::cerr << "ERROR: USRP Commander::configure(), unknown property: " << props[i].id << std::endl;
            throw CF::PropertySet::InvalidConfiguration();
        }
    }

}
