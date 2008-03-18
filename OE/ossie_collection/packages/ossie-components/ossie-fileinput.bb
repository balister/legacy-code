DESCRIPTION = "OSSIE File Input component"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svnr${SRCREV}"

DEPENDS = "ossie-cf ossie-standardinterfaces ossie-sigproc"

S = "${WORKDIR}/FileInput"


SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/components/FileInput/trunk;module=FileInput;proto=https"

prefix="/sdr"

inherit autotools

FILES_${PN} += "/sdr/dom/xml/FileInput/*.xml"
FILES_${PN} += "/sdr/dom/bin/FileInput"

