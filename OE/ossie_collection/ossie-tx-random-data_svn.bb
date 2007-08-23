DESCRIPTION = "OSSIE transmit random bits waveform"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"
PV = "0.0.0+svn${SRCDATE}"

DEPENDS = "ossiecf ossie-randombits ossie-modulator ossie-interpolator ossie-usrp-tx-control ossie-usrp-device ossie-gpp-device"
RDEPENDS = "ossie-randombits ossie-modulator ossie-interpolator ossie-usrp-tx-control ossie-usrp-device ossie-gpp-device"

S = "${WORKDIR}/TX_Random_data"


SRC_URI = "svn://ossie-dev.mprg.org/repos/ossie/waveforms/TX_Random_data/trunk;module=TX_Random_data;proto=https"

prefix="/sdr"

inherit autotools

FILES_${PN} += "/sdr/dom/waveforms/Random_BPSK*.xml"
FILES_${PN} += "/sdr/dev/Random_BPSK_node/*.xml"

BROKEN = "1"
