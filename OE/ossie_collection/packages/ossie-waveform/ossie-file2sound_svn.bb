DESCRIPTION = "OSSIE broadcasr FM mono recieve waveform"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svnr${SRCREV}"

DEPENDS = "ossie-cf ossie-fileinput ossie-gpp-sound-node"
RDEPENDS = "ossie-fileinput ossie-gpp-sound-node"

S = "${WORKDIR}/File2Sound"


SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/waveforms/File2Sound/trunk;module=File2Sound;proto=https"

prefix="/sdr"

inherit autotools

FILES_${PN} += "/sdr/dom/waveforms/File2Sound*.xml"

