DESCRIPTION = "OSSIE Demo waveform"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svn${SRCDATE}"

DEPENDS = "ossie-cf ossie-gpp-device"
RDEPENDS = "ossie-gpp-device"

S = "${WORKDIR}/default_GPP_node"


SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/nodes/default_GPP_node/trunk;module=default_GPP_node;proto=https"

prefix="/sdr"

inherit autotools

FILES_${PN} += "/sdr/dev/nodes/*.xml"

BROKEN = "1"
