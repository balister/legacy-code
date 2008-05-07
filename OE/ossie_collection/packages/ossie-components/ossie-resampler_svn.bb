DESCRIPTION = "OSSIE Rational resampler component"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svnr${SRCREV}"

DEPENDS = "ossie-cf ossie-standardinterfaces ossie-sigproc"

S = "${WORKDIR}/trunk"


SRC_URI = "svn://svn.geekisp.com/opensdr/sca/components/resampler;module=trunk;proto=https"

prefix="/sdr"

inherit autotools

FILES_${PN} += "/sdr/dom/xml/resampler/*.xml"
FILES_${PN} += "/sdr/dom/bin/resampler"

