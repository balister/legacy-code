DESCRIPTION = "OSSIE Demo receiver component"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svnr${SRCREV}"

DEPENDS = "ossie-cf"

S = "${WORKDIR}/RxDemo"


SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/components/RxDemo/trunk;module=RxDemo;proto=https"

prefix="/sdr"

inherit autotools

FILES_${PN} += "${prefix}/dom/xml/RxDemo/*.xml ${prefix}/dom/bin"
FILES_${PN}-dbg = "${prefix}/dom/bin/.debug"

BROKEN = "1"
