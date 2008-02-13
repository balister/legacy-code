DESCRIPTION = "OSSIE General Purpose Processor device"
SECTION =  "apps"
PRIORITY = "optional"
MAINTAINER = "Philip Balister philip@balister.org"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svnr${SRCREV}"

DEPENDS = "ossie-cf"

S = "${WORKDIR}/GPP"

SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/platform/GPP/trunk;module=GPP;proto=https"

inherit autotools

prefix = "/sdr"

FILES_${PN} = "${prefix}/dev/xml/GPP/*xml ${prefix}/dev/bin"
FILES_${PN}-dbg = "${prefix}/dev/bin/.debug"

BROKEN = "1"