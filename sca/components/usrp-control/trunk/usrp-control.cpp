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


#include <string>
#include <iostream>

#include "ossie/cf.h"
#include "ossie/PortTypes.h"
#include "ossie/debug.h"
#include "ossie/Resource_impl.h"

#include "usrp-control.h"

usrp_control_i::usrp_control_i(const char *uuid, omni_condition *condition) :
    Resource_impl(uuid),
    component_running(condition),
    tx_interp(0),
    tx_freq(0),
    tx_gain(0),
    tx_start(false),
    rx_decim(0),
    rx_freq1(0),
    rx_freq2(0),
    rx_gain(0),
    rx_size(0),
    rx_start(true)
{
    // Create port instances
    TXControl = new standardInterfaces_i::TX_Control_u("TX_Control");
    RXControl = new standardInterfaces_i::RX_Control_u("RX_Control");
    data_control = new standardInterfaces_i::Resource_u("Data_Control");
}

CORBA::Object_ptr usrp_control_i::getPort( const char* portName )
    throw (CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, usrp-control, "getPort called with : " << portName)
    
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

    DEBUG(3, usrp-control, "getPort() unknown port: " << portName);
    throw CF::PortSupplier::UnknownPort();

    // This will never happen, but gcc complains if nothing is returned at this point
    return p;
}

void usrp_control_i::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    DEBUG(3, usrp-control, "start called")

    // Call start on remote resources
    CF::Resource_var r = data_control->getRef();
    if (!CORBA::is_nil(r))
        r->start();


    // Set transmit configurable properties not included in Radio_Control idl
    CF::Properties tx_config;
    tx_config.length(1);

    // Set automatic transmit/receive mode on
    tx_config[0].id = CORBA::string_dup("SET_AUTO_TR_1");
    tx_config[0].value <<= (CORBA::ULong) 1;

    TXControl->set_values(tx_config);


    // Set transmit configurable properties not included in Radio_Control idl
    CF::Properties rx_config;
    rx_config.length(1);

    // Set rx antenna
    rx_config[0].id = CORBA::string_dup("SET_RX_ANT_1");
    rx_config[0].value <<= (CORBA::ULong) 0;

    RXControl->set_values(rx_config);

    if (tx_start) {
        DEBUG(3, USRP_Commander, "starting USRP transmit process...");
	setup_tx_usrp();
        TXControl->start(DEFAULT_USRP_TX_CHANNEL);
    }

    if (rx_start) {
        DEBUG(3, USRP_Commander, "starting USRP receive process...");
	setup_rx_usrp();
        RXControl->start(DEFAULT_USRP_RX_CHANNEL);
    }

}

void usrp_control_i::stop() throw (CORBA::SystemException, CF::Resource::StopError) 
{  
    DEBUG(3, usrp-control, "stop called")

    // Call stop on remote resources
    CF::Resource_var r = data_control->getRef();
    if (!CORBA::is_nil(r))
        r->stop();

    RXControl->stop(DEFAULT_USRP_RX_CHANNEL);
    TXControl->stop(DEFAULT_USRP_TX_CHANNEL);
}

void usrp_control_i::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    DEBUG(3, USRP_Commander, "releaseObject called")
    
    component_running->signal();
}

void usrp_control_i::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException)
{
    DEBUG(3, usrp-control, "initialize called")
    
}



void usrp_control_i::configure(const CF::Properties& props)
    throw (CORBA::SystemException,
           CF::PropertySet::InvalidConfiguration,
           CF::PropertySet::PartialConfiguration)
{
    DEBUG(3, usrp-control, "configure called")
    
    DEBUG(3, usrp-control, "props length : " << props.length())

    for (unsigned int i = 0; i < props.length(); i++) {
        DEBUG(3, usrp-control, "Property id : " << props[i].id)

        if (strcmp(props[i].id, "DCE:3efc3930-2739-40b4-8c02-ecfb1b0da9ee") == 0) {
            // RX Frequency 1
            CORBA::Float F;
            props[i].value >>= F;
            DEBUG(3, usrp-control, "RX Frequency 1 property= " << F)
            rx_freq1 = F;
	} else if (strcmp(props[i].id, "DCE:e07f8d62-3063-4ccd-8a8c-220c4490fc0a") == 0) {
            // RX Frequency 2
            CORBA::Float F;
            props[i].value >>= F;
            DEBUG(3, usrp-control, "RX Frequency 2 property= " << F)
            rx_freq2 = F;
        } else if (strcmp(props[i].id, "DCE:6a2d6952-ca11-4787-afce-87a89b882b7b") == 0) {
            // TX Frequency
            CORBA::Float F;
            props[i].value >>= F;
            DEBUG(3, usrp-control, "TX Frequency property= " << F)
            tx_freq = F;
        } else if (strcmp(props[i].id, "DCE:9ca12c0e-ba65-40cf-9ef3-6e7ac671ab5d") == 0) {
            // Transmitter Interpolation Factor
            CORBA::Short M;
            props[i].value >>= M;
            DEBUG(3, usrp-control, "TX Interpolation Factor property= " << M)
            tx_interp = M;
        } else if (strcmp(props[i].id, "DCE:92ec2b80-8040-47c7-a1d8-4c9caa4a4ed2") == 0) {
            // RX Decimation factor
            CORBA::Short D;
            props[i].value >>= D;
            DEBUG(3, usrp-control, "RX Decimation Factor property= " << D)
            rx_decim = D;
        } else if (strcmp(props[i].id, "DCE:58eaebdc-7f4d-416a-8c2f-1e82c93c11ca") == 0) {
            // Number of RX channels
            CORBA::Short nchan;
            props[i].value >>= nchan;
	    rx_nchan = nchan;
            DEBUG(3, usrp-control, "Number of RX channels property= " << nchan)
        } else if (strcmp(props[i].id, "DCE:93324adf-14f6-4406-ba92-a3650089857f") == 0) {
            // RX Data Packet size
            CORBA::ULong L;
            props[i].value >>= L;
            DEBUG(3, usrp-control, "RX Data Packet size property= " << L)
            rx_size = L;
        } else if (strcmp(props[i].id, "DCE:99d586b6-7764-4dc7-83fa-72270d0f1e1b") == 0) {
            //Rx Gain
            CORBA::Float G;
            props[i].value >>= G;
            DEBUG(3, usrp-control, "RX Gain property= " << G)
            rx_gain = G;
        } else if (strcmp(props[i].id, "DCE:fd42344f-4d87-465b-9e6f-e1d7ae48afd6") == 0) {
            // rx_start
            CORBA::Short v;
            props[i].value >>= v;
            if (v)
		rx_start = true;
	    else
		rx_start = false;
            DEBUG(3, usrp-control, "RX start set  to " << rx_start)
        } else if (strcmp(props[i].id, "DCE:0a9b8c8c-f130-4a8f-9ef8-bba023128a4b") == 0) {
            // tx_start
            CORBA::Short v;
            props[i].value >>= v;
            if (v)
		tx_start = true;
	    else
		tx_start = false;
            DEBUG(3, usrp-control, "TX Start set to " << tx_start)
        } else {
            std::cerr << "ERROR: usrp_control::configure(), unknown property: " << props[i].id << std::endl;
            throw CF::PropertySet::InvalidConfiguration();
        }
    }

}

void usrp_control_i::query (CF::Properties & configProperties) throw (CORBA::SystemException, CF::UnknownProperties)
{
}

void usrp_control_i::runTest (CORBA::ULong _number, CF::Properties & _props) throw (CORBA::SystemException, CF::TestableObject::UnknownTest, CF::UnknownProperties)
{
}

void usrp_control_i::setup_rx_usrp()
{

    if (rx_freq1 > 0)
	RXControl->set_frequency(0, rx_freq1);

    if (rx_freq2 > 0)
	RXControl->set_frequency(1, rx_freq2);

    if (rx_decim > 0)
	RXControl->set_decimation_rate(DEFAULT_USRP_RX_CHANNEL, rx_decim);

    if (rx_nchan > 0)
	RXControl->set_number_of_channels(rx_nchan);

    if (rx_size > 0)
	RXControl->set_data_packet_size(DEFAULT_USRP_RX_CHANNEL, rx_size);

    if (rx_gain > -999)
	RXControl->set_gain(DEFAULT_USRP_RX_CHANNEL, rx_gain);
}

void usrp_control_i::setup_tx_usrp()
{

    if (tx_freq > 0)
	TXControl->set_frequency(DEFAULT_USRP_TX_CHANNEL, tx_freq);

    if (tx_interp > 0)
	TXControl->set_interpolation_rate(DEFAULT_USRP_TX_CHANNEL, tx_interp);
}
