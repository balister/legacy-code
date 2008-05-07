DESCRIPTION = "OSSIE WFM demodulator component"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svnr${SRCREV}"

DEPENDS = "ossie-cf ossie-standardinterfaces ossie-sigproc"

S = "${WORKDIR}/trunk"


SRC_URI = "svn://svn.geekisp.com/opensdr/sca/components/fm-demod;module=trunk;proto=https"

prefix="/sdr"

inherit autotools

FILES_${PN} += "/sdr/dom/xml/fm-demod/*.xml"
FILES_${PN} += "/sdr/dom/bin/fm-demod"

