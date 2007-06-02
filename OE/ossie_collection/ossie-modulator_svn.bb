DESCRIPTION = "OSSIE Modulator component"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"
PV = "0.0.0+svn${SRCDATE}"

DEPENDS = "ossiecf ossie-standardinterfaces ossie-sigproc"

S = "${WORKDIR}/Modulator"


SRC_URI = "svn://oe:oe@ossie-dev.mprg.org/repos/ossie/components/Modulator/trunk;module=Modulator;proto=https"

prefix="/sdr"

inherit autotools

FILES_${PN} += "/sdr/dom/xml/Modulator/*.xml"
FILES_${PN} += "/sdr/dom/bin/Modulator"

BROKEN = "1"
