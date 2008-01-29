DESCRIPTION = "OSSIE General Purpose Processor device"
SECTION =  "apps"
PRIORITY = "optional"
MAINTAINER = "Philip Balister philip@balister.org"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svn${SRCDATE}"

DEPENDS = "ossie-cf usrp ossie-standardinterfaces"

S = "${WORKDIR}/USRP"

SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/platform/USRP/trunk;module=USRP;proto=https"

inherit autotools

prefix = "/sdr"

FILES_${PN} = "${prefix}/dev/xml/USRP/*xml ${prefix}/dev/bin/*"
FILES_${PN}-dbg = "${prefix}/dev/bin/.debug"

BROKEN = "1"
