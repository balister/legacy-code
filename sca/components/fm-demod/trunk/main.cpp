/****************************************************************************

Copyright 2008 Virginia Polytechnic Institute and State University

This file is part of the OSSIE WFMdemod.

OSSIE WFMdemod is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE WFMdemod is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE WFMdemod; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/

#include <iostream>

#include "ossie/ossieSupport.h"
#include "ossie/ossieResource.h"
#include "ossie/debug.h"

#include "WFMdemod.h"


int main(int argc, char* argv[])

{
    ossieDebugLevel = 3;

    DEBUG(3, WFMDemod, "Starting component.");

    ossieResource<WFMDemod_i> WFMDemod(argc, argv);

    WFMDemod.run();

}
