/****************************************************************************

Copyright 2006, 2008 Virginia Polytechnic Institute and State University
Copyright 2008       Philip Balister, philip@opensdr.com

This file is part of the OSSIE Sound_out Device.

OSSIE Sound_out Device is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE Sound_out Device is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE Sound_out Device; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


****************************************************************************/

/** \file
    The soundCardPlayback.h file contains declarations for the
    SoundCardPlayback_i classes as well as the data and control ports.
*/

#include <string>

#include <omnithread.h>
#include <alsa/asoundlib.h>

#include "ossie/cf.h"
#include "ossie/PortTypes.h"
#include "ossie/debug.h"

#include "standardinterfaces/complexShort_p.h"

/// Main Sound card device definition
class SoundCardPlayback_i : public virtual POA_CF::Device

{
  public:
    /// Initializing constructor
    SoundCardPlayback_i(char *id, char *label, char *profile);

    /// Default destructor
    ~SoundCardPlayback_i();

    static void do_play_sound(void *u) { ((SoundCardPlayback_i *)u)->play_sound(); };

    // Device methods
    CF::Device::UsageType usageState ()
        throw (CORBA::SystemException);
    CF::Device::AdminType adminState ()
        throw (CORBA::SystemException);
    CF::Device::OperationalType operationalState ()
        throw (CORBA::SystemException);
    CF::AggregateDevice_ptr compositeDevice ()
        throw (CORBA::SystemException);
    void adminState (CF::Device::AdminType _adminType)
        throw (CORBA::SystemException);
    void deallocateCapacity (const CF::Properties & capacities) 
        throw (CF::Device::InvalidState, CF::Device::InvalidCapacity,
               CORBA::SystemException);
    CORBA::Boolean allocateCapacity (const CF::Properties & capacities)
        throw (CF::Device::InvalidState, CF::Device::InvalidCapacity,
               CORBA::SystemException);
    
    char *label () throw (CORBA::SystemException);
    char *softwareProfile () throw (CORBA::SystemException);

    // Resource methods
    /// Does nothing
    void start()
        throw (CF::Resource::StartError, CORBA::SystemException);

    /// Does nothing
    void stop()
        throw (CF::Resource::StopError, CORBA::SystemException);

    char *identifier () throw (CORBA::SystemException);

    // Life Cycle methods
    /// Checks if sound card device is not already in use
    void initialize()
        throw (CF::LifeCycle::InitializeError, CORBA::SystemException);
    /// Does nothing
    void releaseObject () throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);


    // Property Set methods
    /// Sets the following properties (not currently read from .prf.xml)
    ///   - number of channels (default is 2 for stereo)
    ///   - sampling rate (default is 16kHz)
    ///   - periodsize
    ///   - buffer size
    void configure(const CF::Properties &props)
        throw (CORBA::SystemException,
               CF::PropertySet::InvalidConfiguration,
               CF::PropertySet::PartialConfiguration);

    void query (CF::Properties & configProperties)
        throw (CF::UnknownProperties, CORBA::SystemException);

    // Port Supplier interfaces
    CORBA::Object* getPort (const char *) throw (CF::PortSupplier::UnknownPort, 
CORBA::SystemException);

    // TestableObject Interfaces
    void runTest (CORBA::ULong TestID, CF::Properties & testValues)
        throw (CF::UnknownProperties, CF::TestableObject::UnknownTest,
        CORBA::SystemException);

    // --- Sound playback variables ---

    /// Sound playback thread
    omni_thread *play_sound_thread;

  private:
    /// Disallowing default constructor
    SoundCardPlayback_i();

    /// Disallowing copy constructor
    SoundCardPlayback_i(SoundCardPlayback_i&);

    CF::Device::AdminType dev_adminState;
    CF::Device::UsageType dev_usageState;
    CF::Device::OperationalType dev_operationalState;

    std::string dev_id;
    std::string dev_label;
    std::string dev_profile;

    // Ports
    standardInterfaces_i::complexShort_p* sound_out_port;

    /// Main processing playback loop
    void play_sound();

};

