/****************************************************************************

Copyright 2006, 2008 Virginia Polytechnic Institute and State University

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
    The soundCardPlayback.cpp file contains definitions for the
    SoundCardPlayback_i class implementation.
*/

#include <iostream>

#include "ossie/cf.h"
#include "ossie/portability.h"

#include "soundCardPlayback.h"

// Initializing constructor
SoundCardPlayback_i::SoundCardPlayback_i(char *id, char *label, char *profile)
  : insert_idx(2), read_idx(0), length(0), data_available(&wait_for_data)
{
    DEBUG(3, SoundCardPlayback, "constructor invoked")


    dev_id = id;
    dev_label = label;
    dev_profile = profile;


    // initialize variables
    playback_buffer = NULL;

    // Start the play_sound thread
    sound_thread = new omni_thread(run, (void *) this);
    sound_thread->start();

    dev_usageState = CF::Device::IDLE;
    dev_operationalState = CF::Device::ENABLED;
    dev_adminState = CF::Device::UNLOCKED;

}

// Default destructor
SoundCardPlayback_i::~SoundCardPlayback_i()
{
    if (playback_buffer != NULL)
        delete [] playback_buffer;
}

// start data processing thread
void SoundCardPlayback_i::run( void * data )
{
    ((SoundCardPlayback_i*) data)->play_sound();
}

// Device methods
CF::Device::UsageType SoundCardPlayback_i::usageState() throw (CORBA::SystemException)
{
    return dev_usageState;
}

CF::Device::AdminType SoundCardPlayback_i::adminState() throw (CORBA::SystemException)
{
    return dev_adminState;
}

CF::Device::OperationalType SoundCardPlayback_i::operationalState() throw (CORBA::SystemException)
{
    return dev_operationalState;
}

CF::AggregateDevice_ptr SoundCardPlayback_i::compositeDevice() throw (CORBA::SystemException)
{
    return NULL;
}

void SoundCardPlayback_i::adminState (CF::Device::AdminType _adminType)
throw (CORBA::SystemException)
{
    dev_adminState = _adminType;
}

CORBA::Boolean SoundCardPlayback_i::allocateCapacity (const CF::
Properties & capacities)
throw (CORBA::SystemException, CF::Device::InvalidCapacity,
CF::Device::InvalidState)
{

    return true;
}

void SoundCardPlayback_i::deallocateCapacity (const CF::Properties & capacities)
throw (CORBA::SystemException, CF::Device::InvalidCapacity,
CF::Device::InvalidState)
{

}

char *SoundCardPlayback_i::label ()
throw (CORBA::SystemException)
{
    return CORBA::string_dup(dev_label.c_str());
}


char *SoundCardPlayback_i::softwareProfile ()
throw (CORBA::SystemException)
{
    return CORBA::string_dup(dev_profile.c_str());
}

// Resource methods
void SoundCardPlayback_i::start() throw (CF::Resource::StartError, CORBA::SystemException)

{
    DEBUG(3, SoundCardPlayback, "start() invoked")
}

void SoundCardPlayback_i::stop() throw (CF::Resource::StopError, CORBA::SystemException)

{
    DEBUG(3, SoundCardPlayback, "stop() invoked")
}

char *SoundCardPlayback_i::identifier () throw (CORBA::SystemException)
{
    return CORBA::string_dup(dev_id.c_str());
}


// PortSupplier methods
CORBA::Object_ptr SoundCardPlayback_i::getPort(const char* portName) throw(CF::PortSupplier::UnknownPort, CORBA::SystemException)

{
    DEBUG(3, SoundCardPlayback, "getPort() invoked with : " << portName)
}

// Life Cycle methods
void SoundCardPlayback_i::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException)

{
    DEBUG(3, SoundCardPlayback, "initialize() invoked")
    snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;
    char *pcm_name = "plughw:0,0";
    int rc;

    if ((rc = snd_pcm_open(&pcm_handle, pcm_name, stream, 0)) < 0) {
    DEBUG(3, SoundCardPlayback, "Failed to open pcm device " << pcm_name)
    if (rc == -EBUSY) 
        DEBUG(3, SoundCardPlayback, "Sound device in use.")
    
    //throw(CF::LifeCycle::InitializeError());
    }
}

void SoundCardPlayback_i::releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException)

{

    DEBUG(3, SoundCardPlayback, "releaseObject invoked")
}

// Property Set methods
void SoundCardPlayback_i::configure(const CF::Properties &props) throw (CORBA::SystemException, CF::PropertySet::InvalidConfiguration, CF::PropertySet::PartialConfiguration)
{
    DEBUG(3, SoundCardPlayback, "configure invoked. Number of props = " << props.length() );

    // read properties from .prf
    CORBA::ULong rate(16000); // default sampling rate

    for (unsigned int i=0; i<props.length(); i++)
    {
        DEBUG(3, SoundCardPlayback, "configure property id : " << props[i].id)

        if (strcmp(props[i].id, "DCE:98ca3738-5511-4fb1-ba00-2b86dc4e3c99")==0)
        {
            CORBA::ULong n;
            props[i].value >>= n;
            rate = n;
            DEBUG(3, SoundCardPlayback, "sample rate: " << rate << " Hz")
        }
        else
        {
            DEBUG(1, SoundCardPlayback, "ERROR: unkown configure() property id " << props[i].id)
            throw(CF::PropertySet::InvalidConfiguration());
        }
    }


    snd_pcm_hw_params_t *hwparams;
    int rc;

    snd_pcm_hw_params_alloca(&hwparams);

    if (snd_pcm_hw_params_any(pcm_handle, hwparams)) {
        DEBUG(3, SoundCardPlayback, "Can not configure this PCM device.")
        throw(CF::PropertySet::InvalidConfiguration());
    }

    if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
        DEBUG(3, SoundCardPlayback, "Error setting access mode.")
        throw(CF::PropertySet::InvalidConfiguration());
    }

#ifdef __powerpc__
    if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S16_BE) < 0) {
#else // Default endianess is little endian
    if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S16_LE) < 0) {
#endif
        DEBUG(3, SoundCardPlayback, "Error setting format.")
        throw(CF::PropertySet::InvalidConfiguration());
    }
    // Set number of channels to two for stereo
    if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, 2) < 0) {
        DEBUG(3, SoundCardPlayback, "Error setting number of channels.")
        throw(CF::PropertySet::InvalidConfiguration());
    }

    unsigned int exact_rate = rate;
    if (snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &exact_rate, 0) < 0) {
        DEBUG(3, SoundCardPlayback, "Error setting rate.")
        throw(CF::PropertySet::InvalidConfiguration());
    }

    if (rate != exact_rate) {
        DEBUG(3, SoundCardPlayback, "rate " << rate << " is not supported by your hardware. Using " << exact_rate << " instead.")
    }

    // Set up period size
    periodsize = 256;
    snd_pcm_uframes_t exactperiodsize = periodsize;
    if ((rc = snd_pcm_hw_params_set_period_size_near(pcm_handle, hwparams, &exactperiodsize, 0)) < 0) {
        DEBUG(3, SoundCardPlayback, "Error setting period size." << snd_strerror(rc))
        throw(CF::PropertySet::InvalidConfiguration());
    }

    if (periodsize != exactperiodsize) {
        DEBUG(3, SoundCardPlayback, "Period size set to " << exactperiodsize << " not the requested " << periodsize)
        periodsize = exactperiodsize;
    }

    // Set the buffer size
    snd_pcm_uframes_t buffersize = 8 * periodsize;
    if ((rc = snd_pcm_hw_params_set_buffer_size_near(pcm_handle, hwparams, &buffersize)) < 0) {
        DEBUG(3, SoundCardPlayback, "Error setting buffer size, " << snd_strerror(rc))
        throw(CF::PropertySet::InvalidConfiguration());
    }

    // Get the buffer size
    snd_pcm_uframes_t periods = 0;
    if ((rc = snd_pcm_hw_params_get_buffer_size(hwparams, &periods)) < 0) {
        DEBUG(3, SoundCardPlayback, "Error getting the buffer size " << snd_strerror(rc))
        throw(CF::PropertySet::InvalidConfiguration());
    }

    DEBUG(3, SoundCardPlayback, "Buffer size : " << periods)

    // Apply HW settings
    if (snd_pcm_hw_params(pcm_handle, hwparams) < 0) {
        DEBUG(3, SoundCardPlayback, "Error setting HW params.")
        throw(CF::PropertySet::InvalidConfiguration());
    }

    // set up playback buffer
    length = periods * 2 * 8;
    DEBUG(3, SoundCardPlayback, "Playback buffer length = " << length)
    playback_buffer = new short[length];

}

void SoundCardPlayback_i::query (CF::Properties & configProperties)
throw (CORBA::SystemException, CF::UnknownProperties)
{

}

// TestableObject interfaces

void SoundCardPlayback_i::runTest (CORBA::ULong _number, CF::Properties & _props)
throw (CORBA::SystemException, CF::TestableObject::UnknownTest,
CF::UnknownProperties)
{

}

void SoundCardPlayback_i::play_sound()

{
    bool under_run_occured(true);

    while (1) {
        playback_mutex.lock();

        const int buf_size = 256;

        // Check if we have buf_size periods in the buffer
        // if not wait for more data, otherwise send samples to sound card

        //DEBUG(3, SoundCardPlayback, "thread: insert_idx = " << insert_idx << "  read_idx = " << read_idx)

        int len = insert_idx - read_idx;

        if ( under_run_occured && 
             ((len > 0) && (len < buf_size)) ||
             ((len < 0) && (length - insert_idx + read_idx) < buf_size)) {

            playback_mutex.unlock();

            DEBUG(1, SoundCardPlayback, "Underrun occured, waiting for more sound data")

            data_available.wait();

        } else {
            short buf[buf_size];

            under_run_occured = false;

            unsigned int buf_length = buf_size/2;
            for (unsigned int i = 0; i < buf_size; ++i) {
                buf[i] = playback_buffer[read_idx++];
                if (read_idx == length)
                    if (insert_idx != 0)
                        read_idx = 0;
                else {
                    under_run_occured = true;
                    buf_length = i/2;
                    break;
                }
                if (read_idx == insert_idx - 2) {
                    under_run_occured = true;
                    buf_length = i/2;
                    break;
                }
            }
            playback_mutex.unlock();

            // check status of pcm device
            int rc;
            if ((rc = snd_pcm_writei(pcm_handle, buf, buf_length)) != 0) {
                if (rc == -EPIPE) {
                    under_run_occured = true;
                    DEBUG(3, SoundCardPlayback, "Sound card under run occured.")
                    snd_pcm_prepare(pcm_handle);
                } else if (rc == -EAGAIN) {
                    DEBUG(3, SoundCardPlayback, "Sound card over run occured.")
                } else if (rc < 0) {
                    DEBUG(3, SoundCardPlayback, "Sound write error, " << snd_strerror(rc))
                } else {
                    if (rc != (buf_length))
                    DEBUG(3, SoundCardPlayback, rc << " frames written, buffer contained " << buf_length)
                }
            }
        }
    }
}

