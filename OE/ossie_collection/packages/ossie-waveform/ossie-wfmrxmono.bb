DESCRIPTION = "OSSIE broadcasr FM mono recieve waveform"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svnr${SRCREV}"

DEPENDS = "ossie-cf ossie-wfmdemod ossie-rationalresamplercomplexshort ossie-soundout-device ossie-gpp-device"
RDEPENDS = "ossie-wfmdemod ossie-rationalresamplercomplexshort ossie-soundout-device ossie-gpp-device"

S = "${WORKDIR}/WFMRxMono"


SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/waveforms/WFMRxMono/trunk;module=WFMRxMono;proto=https"

prefix="/sdr"

inherit autotools

FILES_${PN} += "/sdr/dom/waveforms/WFMMono*.xml"

