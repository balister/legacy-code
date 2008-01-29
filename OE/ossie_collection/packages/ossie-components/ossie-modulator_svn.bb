DESCRIPTION = "OSSIE Modulator component"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svn${SRCDATE}"

DEPENDS = "ossie-cf ossie-standardinterfaces ossie-sigproc"

S = "${WORKDIR}/Modulator"


SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/components/Modulator/trunk;module=Modulator;proto=https"

prefix="/sdr"

inherit autotools

FILES_${PN} += "/sdr/dom/xml/Modulator/*.xml"
FILES_${PN} += "/sdr/dom/bin/Modulator"

BROKEN = "1"
