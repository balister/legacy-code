DESCRIPTION = "OSSIE Demo waveform"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svn${SRCDATE}"

DEPENDS = "ossie-cf ossie-channeldemo ossie-rxdemo ossie-txdemo ossie-gpp-node"
RDEPENDS = "ossie-channeldemo ossie-rxdemo ossie-txdemo ossie-gpp-node"

S = "${WORKDIR}/ossie_demo"


SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/waveforms/ossie_demo/trunk;module=ossie_demo;proto=https"

prefix="/sdr"

inherit autotools

FILES_${PN} += "/sdr/dom/waveforms/*.xml"

BROKEN = "1"
