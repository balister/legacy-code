DESCRIPTION = "OSSIE General Purpose Processor device"
SECTION =  "apps"
PRIORITY = "optional"
MAINTAINER = "Philip Balister philip@balister.org"
LICENSE = "GPL"
PV = "0.0.0+svn${SRCDATE}"

DEPENDS = "ossiecf"

S = "${WORKDIR}/GPP"

SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/platform/GPP/trunk;module=GPP;proto=https"

inherit autotools

prefix = "/sdr"

FILES_${PN} += "/sdr/dev/xml/GPP/*xml"
FILES_${PN} += "/sdr/dev/bin/GPP"

BROKEN = "1"
