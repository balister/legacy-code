DESCRIPTION = "OSSIE General Purpose Processor device"
SECTION =  "apps"
PRIORITY = "optional"
MAINTAINER = "Philip Balister philip@balister.org"
LICENSE = "GPL"
PV = "0.0.0+svn${SRCDATE}"

DEPENDS = "ossiecf usrp ossie-standardinterfaces"

S = "${WORKDIR}/USRP"

SRC_URI = "svn://oe:oe@ossie-dev.mprg.org/repos/ossie/platform/USRP/trunk;module=USRP;proto=https"

inherit autotools

prefix = "/sdr"

FILES_${PN} += "/sdr/dev/xml/USRP/*xml"
FILES_${PN} += "/sdr/dev/bin/USRP"

BROKEN = "1"
