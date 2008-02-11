DESCRIPTION = "OSSIE Rational resampler component"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svnr${SRCREV}"

DEPENDS = "ossie-cf ossie-standardinterfaces ossie-sigproc"

S = "${WORKDIR}/RationalResamplerComplexShort"


SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/components/RationalResamplerComplexShort/trunk;module=RationalResamplerComplexShort;proto=https"

prefix="/sdr"

inherit autotools

FILES_${PN} += "/sdr/dom/xml/RationalResamplerComplexShort/*.xml"
FILES_${PN} += "/sdr/dom/bin/RationalResamplerComplexSHort"

