DESCRIPTION = "OSSIE transmit random bits waveform"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svnr${SRCREV}"

DEPENDS = "ossie-cf ossie-randombits ossie-modulator ossie-interpolator ossie-usrp-tx-control ossie-usrp-device ossie-gpp-device"
RDEPENDS = "ossie-randombits ossie-modulator ossie-interpolator ossie-usrp-tx-control ossie-usrp-device ossie-gpp-device"

S = "${WORKDIR}/TX_Random_data"


SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/waveforms/TX_Random_data/trunk;module=TX_Random_data;proto=https"

prefix="/sdr"

inherit autotools

FILES_${PN} += "/sdr/dom/waveforms/Random_BPSK*.xml"

