DESCRIPTION = "OSSIE General Purpose Processor device"
SECTION =  "apps"
PRIORITY = "optional"
MAINTAINER = "Philip Balister philip@balister.org"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svnr${SRCREV}"

DEPENDS = "ossie-cf usrp ossie-standardinterfaces"

S = "${WORKDIR}/trunk"

SRC_URI = "svn://svn.geekisp.com/opensdr/sca/platform/USRP;module=trunk;proto=https"

inherit autotools

prefix = "/sdr"

FILES_${PN} = "${prefix}/dev/xml/USRP/*xml ${prefix}/dev/bin/*"
FILES_${PN}-dbg = "${prefix}/dev/bin/.debug"

