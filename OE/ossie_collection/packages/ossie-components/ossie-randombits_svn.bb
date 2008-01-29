DESCRIPTION = "OSSIE Random bit generator component"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svn${SRCDATE}"

DEPENDS = "ossie-cf ossie-standardinterfaces ossie-sigproc"

S = "${WORKDIR}/RandomBits"

SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/components/RandomBits/trunk;module=RandomBits;proto=https"

prefix="/sdr"

inherit autotools

FILES_${PN} += "/sdr/dom/xml/RandomBits/*.xml"
FILES_${PN} += "/sdr/dom/bin/RandomBits"

BROKEN = "1"
