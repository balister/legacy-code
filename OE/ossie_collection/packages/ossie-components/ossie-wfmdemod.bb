DESCRIPTION = "OSSIE WFM demodulator component"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svnr${SRCREV}"

DEPENDS = "ossie-cf ossie-standardinterfaces ossie-sigproc"

S = "${WORKDIR}/WFMDemod"


SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/components/WFMDemod/trunk;module=WFMDemod;proto=https"

prefix="/sdr"

inherit autotools

FILES_${PN} += "/sdr/dom/xml/WFMdemod/*.xml"
FILES_${PN} += "/sdr/dom/bin/WFMdemod"

