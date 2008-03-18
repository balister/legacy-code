DESCRIPTION = "OSSIE Demo waveform"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svnr${SRCREV}"

DEPENDS = "ossie-cf ossie-gpp-device ossie-soundout-device"
RDEPENDS = "ossie-gpp-device ossie-soundout-device"

S = "${WORKDIR}/default_GPP_sound_node"


SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/nodes/default_GPP_sound_node/trunk;module=default_GPP_sound_node;proto=https"

prefix="/sdr"

inherit autotools

FILES_${PN} += "/sdr/dev/nodes/*.xml"

