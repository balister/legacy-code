DESCRIPTION = "OSSIE Demo receiver component"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"
PV = "0.0.0+svn${SRCDATE}"

DEPENDS = "ossiecf"

S = "${WORKDIR}/TxDemo"


SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/components/TxDemo/trunk;module=TxDemo;proto=https"

prefix="/sdr"

inherit autotools

FILES_${PN} += "${prefix}/dom/xml/TxDemo/*.xml ${prefix}/dom/bin"
FILES_${PN}-dbg = "${prefix}/dom/bin/.debug/"

BROKEN = "1"
