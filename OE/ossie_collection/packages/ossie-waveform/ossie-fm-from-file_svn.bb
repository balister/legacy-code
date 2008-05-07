DESCRIPTION = "OSSIE broadcasr FM mono recieve waveform"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svnr${SRCREV}"

DEPENDS = "ossie-cf ossie-file-input ossie-fm-demod ossie-resampler ossie-gpp-sound-node ossie-nodebooter ossie-c-wavloader"
RDEPENDS = "ossie-file-input ossie-fm-demod ossie-resampler ossie-gpp-sound-node ossie-nodebooter ossie-c-wavloader"

S = "${WORKDIR}/trunk"


SRC_URI = "svn://svn.geekisp.com/opensdr/sca/waveforms/fm-from-file/;module=trunk;proto=https"

prefix="/sdr"

inherit autotools

FILES_${PN} += "/sdr/dom/waveforms/fm-from-file*.xml"

