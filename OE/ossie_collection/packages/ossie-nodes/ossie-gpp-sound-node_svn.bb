DESCRIPTION = "OSSIE Demo waveform"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svnr${SRCREV}"

DEPENDS = "ossie-cf ossie-gpp-device ossie-soundout-device"
RDEPENDS = "ossie-gpp-device ossie-soundout-device"

S = "${WORKDIR}/trunk"


SRC_URI = "svn://svn.geekisp.com/opensdr/sca/nodes/GPP-sound-node;module=trunk;proto=https"

prefix="/sdr"

inherit autotools

FILES_${PN} += "/sdr/dev/nodes/*.xml"

