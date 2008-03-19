/****************************************************************************

Copyright 2004, 2007, 2008  Virginia Polytechnic Institute and State University

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

Oct/6/03	C. Neely	Created
C. Aguayo

****************************************************************************/
#include <iostream>

#include <string.h>

#include "ossie/Device_impl.h"
#include "ossie/orb_wrap.h"
#include "ossie/debug.h"

Device_impl::Device_impl (char *_id, char *lbl, char *sftwrPrfl)
{
    _label = lbl;
    _softwareProfile = sftwrPrfl;

    _usageState = CF::Device::IDLE;
    _operationalState = CF::Device::ENABLED;
    _adminState = CF::Device::UNLOCKED;
    initialConfiguration = true;
}


Device_impl::Device_impl (char *_id, char *lbl, char *sftwrPrfl, CF::Properties & capacities)
{
    _label = lbl;
    _softwareProfile = sftwrPrfl;

    _usageState = CF::Device::IDLE;
    _operationalState = CF::Device::ENABLED;
    _adminState = CF::Device::UNLOCKED;
    initialConfiguration = true;

    configure (capacities);

}


Device_impl::~Device_impl ()
{
}


/* Alternate implementation*/
CORBA::Boolean Device_impl::allocateCapacity (const CF::
Properties & capacities)
throw (CORBA::SystemException, CF::Device::InvalidCapacity,
CF::Device::InvalidState)
{
    DEBUG(3, Device, "In allocate capacities with " << capacities.length() << " values.");

    CF::Properties currentCapacities;

    bool extraCap = false; // Flag to check remaining extra capacity to allocate
    bool foundProperty; // flag to indicate if the requested property was found

    if (capacities.length() == 0) // Nothing to do, return
	return true;

    // verify that the device is in a valid state
    if (!isUnlocked () || isDisabled ()) {
	std::cout << "Cannot allocate capacity: System is either LOCKED, SHUTTING DOWN, or DISABLED." << std::endl;
        throw (CF::Device::InvalidState("Cannot allocate capacity. System is either LOCKED, SHUTTING DOWN or DISABLED."));
    }
    if (!isBusy ()) ///\todo take code from else and invert sense PJB
    {
        try // the try is just a formality in this case
        {
	    // Get Current properties
            query (currentCapacities);
//currentCapacities = propertySet;
        }
        catch (CF::UnknownProperties)
        {
        }

        { // this is here because Microsoft does not believe in following standards
/*look in propertySet for the properties requested */
            for (unsigned i = 0; i < capacities.length (); i++)
            {
                foundProperty = false;
                for (unsigned j = 0; j < currentCapacities.length (); j++)
                {
                    if (strcmp (capacities[i].id, currentCapacities[j].id) == 0)
                    {
//verify that both values have the same type
                        if (!ORB_WRAP::isValidType (currentCapacities[j].value, capacities[i].value)) {
			    std::cout << "Cannot allocate capacity: Incorrect data type." << std::endl;
                            throw (CF::Device::InvalidCapacity("Cannot allocate capacity. Incorrect Data Type.", capacities));
			}
                        else
                        {
//check for sufficient capacity and allocate it
// isCapacityAvailable( currentCapacities, capacities )
// if true, then allocate ALL capacities; otherwise, throw InvalidCapacity
//if(not enough capacity)
                            if (!allocate (currentCapacities[j].value, capacities[i].value)) {
				std::cout << "Cannnot allocate capacity: Insufficient capacity." << std::endl;
                                throw (CF::Device::InvalidCapacity("Cannot allocate capacity. Insufficient capacity.", capacities));
			    }
                        }
                        foundProperty = true;     //report that the requested property was found
                        break;

                    }
                }

                if (!foundProperty) {
		    std::cout << "Cannot allocate capacity: Invalid property ID." << std::endl;
                    throw (CF::Device::InvalidCapacity("Cannot allocate capacity. Invalid property ID", capacities));
		}
            }
        }

        {                                         // this is here because Microsoft does not believe in following standards
//Check for remaining capacity.
            for (unsigned i = 0; i < currentCapacities.length (); i++)
            {
                if (compareAnyToZero (currentCapacities[i].value) == POSITIVE)
                {
                    extraCap = true;
                    break;                        //No need to keep going. if after allocation
//there is any capacity available, the device is ACTIVE.
                }
            }
        }

//Store new capacities, here is when the allocation takes place
        configure (currentCapacities);
//propertySet = currentCapacities;

/*update usage state */
        if (!extraCap)
            setUsageState (CF::Device::BUSY);
        else
            setUsageState (CF::Device::ACTIVE);   /* Assumes it allocated something. Not considering zero allocations */

        return true;
    }
    else {
/*not sure */
	std::cout << "Cannot allocate capacity: System is BUSY" << std::endl;
        throw (CF::Device::InvalidCapacity ("Cannot allocate capacity. System is BUSY.", capacities));
    }
}


void
Device_impl::deallocateCapacity (const CF::Properties & capacities)
throw (CORBA::SystemException, CF::Device::InvalidCapacity,
CF::Device::InvalidState)
{
    DEBUG(3, Device, "In deallocate capacities with " << capacities.length() << " values.");

    if (capacities.length() == 0)
	return;

    CF::Properties currentCapacities;

    bool totalCap = true;                         /* Flag to check remaining extra capacity to allocate */
    bool foundProperty;                           /*flag to indicate if the requested property was found */
    AnyComparisonType compResult;

/*verify that the device is in a valid state */
    if (isLocked () || isDisabled ())
    {
        throw (CF::Device::
            InvalidState
            ("Cannot deallocate capacity. System is either LOCKED or DISABLED."));
        return;
    }
/*Now verify that there is capacity currently being used */
    if (!isIdle ())
    {                                             /*Get Current properties */
        query (currentCapacities);
//currentCapacities = propertySet;

        {                                         // this is here because Microsoft does not believe in following standards
/*look in propertySet for the properties requested */
            for (unsigned i = 0; i < capacities.length (); i++)
            {
                foundProperty = false;
                for (unsigned j = 0; j < currentCapacities.length (); j++)
                {
/*stringcompare???? */
                    if (strcmp (capacities[i].id, currentCapacities[j].id) == 0)
                    {
//verify that both values have the same type
                        if (!ORB_WRAP::
                            isValidType (currentCapacities[j].value,
                            capacities[i].value))
                            throw (CF::Device::
                                InvalidCapacity
                                ("Cannot allocate capacity. Incorrect Data Type.",
                                capacities));
                        else
                            deallocate (currentCapacities[j].value,
                                capacities[i].value);

                        foundProperty = true;     /*report that the requested property was found */
                        break;

                    }
                }

                if (!foundProperty)
                    throw (CF::Device::
                        InvalidCapacity
                        ("Cannot deallocate capacity. Invalid property ID",
                        capacities));
            }
        }

        {                                         // this is here because Microsoft does not believe in following standards
//Check for exceeding dealLocations and back-to-total capacity
            for (unsigned i = 0; i < currentCapacities.length (); i++)
            {
                for (unsigned j = 0; j < originalCap.length (); j++)
                {
                    if (strcmp (currentCapacities[i].id, originalCap[j].id) == 0)
                    {
                        compResult =
                            compareAnys (currentCapacities[i].value,
                            originalCap[j].value);
                        if (compResult == FIRST_BIGGER)
                            throw (CF::Device::
                                InvalidCapacity
                                ("Cannot deallocate capacity. New capacity would exceed original bound.",
                                capacities));
                        else if (compResult == SECOND_BIGGER)
                            totalCap = false;
                        break;
                    }
                }
            }
        }

/*Write new capacities */
        configure (currentCapacities);
//propertySet = currentCapacities;

/*update usage state */
        if (!totalCap)
            setUsageState (CF::Device::ACTIVE);
        else
            setUsageState (CF::Device::IDLE);     /* Assumes it allocated something. Not considering zero allocations */
// update adminState

        return;
    }
    else
    {
/*not sure */
        throw (CF::Device::InvalidCapacity ("Cannot deallocate capacity. System is IDLE.", capacities));
        return;
    }
}


bool
Device_impl::allocate (CORBA::Any & deviceCapacity,
const CORBA::Any & resourceRequest)
{
    CORBA::TypeCode_var tc1 = deviceCapacity.type ();
    CORBA::TypeCode_var tc2 = resourceRequest.type ();

    switch (tc1->kind ())
    {
        case CORBA::tk_ulong:
        {
            CORBA::ULong devCapac, rscReq;
            deviceCapacity >>= devCapac;
            resourceRequest >>= rscReq;
            if (rscReq <= devCapac)
            {
                devCapac -= rscReq;
                deviceCapacity <<= devCapac;
                return true;
            }
            else
                return false;

        }
        case CORBA::tk_long:
        {
            CORBA::Long devCapac, rscReq;
            deviceCapacity >>= devCapac;
            resourceRequest >>= rscReq;
            if (rscReq <= devCapac)
            {
                devCapac -= rscReq;
                deviceCapacity <<= devCapac;
                return true;
            }
            else
                return false;
        }
        case CORBA::tk_short:
        {
            CORBA::Short devCapac, rscReq;
            deviceCapacity >>= devCapac;
            resourceRequest >>= rscReq;
            if (rscReq <= devCapac)
            {
                devCapac -= rscReq;
                deviceCapacity <<= devCapac;
                return true;
            }
            else
                return false;
        }
        default:
            return false;
    }
//Should never reach this point
    return false;
}


void
Device_impl::deallocate (CORBA::Any & deviceCapacity,
const CORBA::Any & resourceRequest)
{
    CORBA::TypeCode_var tc1 = deviceCapacity.type ();
    CORBA::TypeCode_var tc2 = resourceRequest.type ();

    switch (tc1->kind ())
    {
        case CORBA::tk_ulong:
        {
            CORBA::ULong devCapac, rscReq;
            deviceCapacity >>= devCapac;
            resourceRequest >>= rscReq;
            devCapac += rscReq;
            deviceCapacity <<= devCapac;
            break;

        }
        case CORBA::tk_long:
        {
            CORBA::Long devCapac, rscReq;
            deviceCapacity >>= devCapac;
            resourceRequest >>= rscReq;
            devCapac += rscReq;
            deviceCapacity <<= devCapac;
            break;
        }
        case CORBA::tk_short:
        {
            CORBA::Short devCapac, rscReq;
            deviceCapacity >>= devCapac;
            resourceRequest >>= rscReq;
            devCapac += rscReq;
            deviceCapacity <<= devCapac;
            break;
        }
        default:
            break;
    }

    return;
}


// compareAnys function compares both Any type inputs
// returns FIRST_BIGGER if the first argument is bigger
// retunrs SECOND_BIGGER is the second argument is begger
// and BOTH_EQUAL if they are equal
Device_impl::AnyComparisonType Device_impl::compareAnys (CORBA::Any & first,
CORBA::Any & second)
{
    CORBA::TypeCode_var tc1 = first.type ();
    CORBA::TypeCode_var tc2 = second.type ();

    switch (tc1->kind ())
    {
        case CORBA::tk_ulong:
        {
            CORBA::ULong frst, scnd;
            first >>= frst;
            second >>= scnd;
            if (frst > scnd)
                return FIRST_BIGGER;
            else if (frst == scnd)
                return BOTH_EQUAL;
            else
                return SECOND_BIGGER;
        }
        case CORBA::tk_long:
        {
            CORBA::Long frst, scnd;
            first >>= frst;
            second >>= scnd;
            if (frst > scnd)
                return FIRST_BIGGER;
            else if (frst == scnd)
                return BOTH_EQUAL;
            else
                return SECOND_BIGGER;
        }
        case CORBA::tk_short:
        {
            CORBA::Short frst, scnd;
            first >>= frst;
            second >>= scnd;
            if (frst > scnd)
                return FIRST_BIGGER;
            else if (frst == scnd)
                return BOTH_EQUAL;
            else
                return SECOND_BIGGER;
        }
        default:
            return UNKNOWN;
    }

    return UNKNOWN;
}


// compareAnyToZero function compares the any type input to zero
// returns POSITIVE if the first argument is bigger
// retunrs NEGATIVE is the second argument is begger
// and ZERO if they are equal
Device_impl::AnyComparisonType Device_impl::compareAnyToZero (CORBA::
Any & first)
{
    CORBA::TypeCode_var tc1 = first.type ();

    switch (tc1->kind ())
    {
        case CORBA::tk_ulong:
        {
            CORBA::ULong frst;
            first >>= frst;
            if (frst > 0)
                return POSITIVE;
            else if (frst == 0)
                return ZERO;
            else
                return NEGATIVE;
        }
        case CORBA::tk_long:
        {
            CORBA::Long frst;
            first >>= frst;
            if (frst > 0)
                return POSITIVE;
            else if (frst == 0)
                return ZERO;
            else
                return NEGATIVE;
        }
        case CORBA::tk_short:
        {
            CORBA::Short frst;
            first >>= frst;
            if (frst > 0)
                return POSITIVE;
            else if (frst == 0)
                return ZERO;
            else
                return NEGATIVE;
        }
        default:
            return UNKNOWN;
    }

    return UNKNOWN;
}


void
Device_impl::setUsageState (CF::Device::UsageType newUsageState)
{
/*keep a copy of the actual usage state */
// unused PJB CF::Device::UsageType previousState = _usageState;
    _usageState = newUsageState;
//if(previousState != usageState) ;
//Send event channel here
//event(label, label, USAGE_STATE_EVENT, previousState, usageState);
}


void
Device_impl::adminState (CF::Device::AdminType _adminType)
throw (CORBA::SystemException)
{
    _adminState = _adminType;
}


bool Device_impl::isUnlocked ()
{
    if (_adminState == CF::Device::UNLOCKED)
        return true;
    else
        return false;
}


bool Device_impl::isLocked ()
{
    if (_adminState == CF::Device::LOCKED)
        return true;
    else
        return false;
}


bool Device_impl::isDisabled ()
{
    if (_operationalState == CF::Device::DISABLED)
        return true;
    else
        return false;

}


bool Device_impl::isBusy ()
{
    if (_usageState == CF::Device::BUSY)
        return true;
    else
        return false;

}


bool Device_impl::isIdle ()
{
    if (_usageState == CF::Device::IDLE)
        return true;
    else
        return false;

}


char *
Device_impl::label ()
throw (CORBA::SystemException)
{
    return CORBA::string_dup(_label.c_str());
}


char *
Device_impl::softwareProfile ()
throw (CORBA::SystemException)
{
    return CORBA::string_dup(_softwareProfile.c_str());
}


CF::Device::UsageType Device_impl::usageState ()throw (CORBA::SystemException)
{
    return _usageState;
}


CF::Device::AdminType Device_impl::adminState ()throw (CORBA::SystemException)
{
    return _adminState;
}


CF::Device::OperationalType Device_impl::operationalState ()throw (CORBA::
SystemException)
{
    return _operationalState;
}


// TODO: actually return an AggregateDevice type - aggregate devices are not supported by this version //
CF::AggregateDevice_ptr Device_impl::compositeDevice ()throw (CORBA::
SystemException)
{
    return NULL;
}


void  Device_impl::configure (const CF::Properties & capacities)
throw (CF::PropertySet::PartialConfiguration, CF::PropertySet::
InvalidConfiguration, CORBA::SystemException)
{
    if (initialConfiguration)
    {

        initialConfiguration = false;

        originalCap.length (capacities.length ());

        for (unsigned int i = 0; i < capacities.length (); i++)
        {
            originalCap[i].id = CORBA::string_dup (capacities[i].id);
            originalCap[i].value = capacities[i].value;
        }
    }

    configure(capacities);

}


#ifdef AUTOMATIC_TEST

void Device_impl::displayProperties()
{
    printf("\nCurrent Capacities: \n");
    CF::Properties capacities;
    query(capacities);
    for(unsigned int i = 0; i < capacities.length(); i++)
    {
        printf("%s ",capacities[i].id);

        CORBA::TypeCode_var tc = capacities[i].value.type();
        switch( tc->kind() )
        {
            case CORBA::tk_ulong:
            {
                CORBA::ULong cap;
                capacities[i].value >>= cap;
                printf("%u\n",cap);
                break;
            }
            case CORBA::tk_long:
            {
                CORBA::Long cap;
                capacities[i].value >>= cap;
                printf("%u\n",cap);
                break;
            }
            case CORBA::tk_short:
            {
                CORBA::Short cap;
                capacities[i].value >>= cap;
                printf("%u\n",cap);
                break;
            }
            case CORBA::tk_string:
            {
                char* cap;
                capacities[i].value >>= cap;
                printf("%s\n",cap);
                break;
            }

            default:
            {
                printf("Unknown CORBA Type\n");
                break;
            }

        }
    }
    return;
}


void Device_impl::displayProperties(CF::Properties& capacities)
{
    printf("\nInput Properties: \n");
    for(int i = 0; i < capacities.length(); i++)
    {
        printf("%s ",capacities[i].id);

        CORBA::TypeCode_var tc = capacities[i].value.type();
        switch( tc->kind() )
        {
            case CORBA::tk_ulong:
            {
                CORBA::ULong cap;
                capacities[i].value >>= cap;
                printf("%u\n",cap);
                break;
            }
            case CORBA::tk_long:
            {
                CORBA::Long cap;
                capacities[i].value >>= cap;
                printf("%u\n",cap);
                break;
            }
            case CORBA::tk_short:
            {
                CORBA::Short cap;
                capacities[i].value >>= cap;
                printf("%u\n",cap);
                break;
            }
            case CORBA::tk_string:
            {
                char* cap;
                capacities[i].value >>= cap;
                printf("%s\n",cap);
                break;
            }

            default:
            {
                printf("Unknown CORBA Type\n");
                break;
            }

        }
    }
    return;
}


void Device_impl::displayAttributes()
{
    printf("\n_label: %s\n",_label);
    printf("_Software Profile: %s\n",_softwareProfile);
    printf("_usageState is ");
    switch(_usageState)
    {
        case CF::Device::UsageType::IDLE:
        {
            printf("IDLE\n");
            break;
        }
        case CF::Device::UsageType::ACTIVE:
        {
            printf("ACTIVE\n");
            break;
        }
        case CF::Device::UsageType::BUSY:
        {
            printf("BUSY\n");
            break;
        }
    }

    printf("_operationalState is ");
    switch(_operationalState)
    {
        case CF::Device::OperationalType::ENABLED:
        {
            printf("ENABLED\n");
            break;
        }
        case CF::Device::OperationalType::DISABLED:
        {
            printf("DISABLED\n");
            break;
        }
    }
    printf("_adminState is ");
    switch(_adminState)
    {
        case CF::Device::AdminType::LOCKED:
        {
            printf("LOCKED\n");
            break;
        }
        case CF::Device::AdminType::SHUTTING_DOWN:
        {
            printf("SHUTTING_DOWN\n");
            break;
        }
        case CF::Device::AdminType::UNLOCKED:
        {
            printf("UNLOCKED\n");
            break;
        }
    }
    return;
}
#endif
