DESCRIPTION = "OSSIE Interpolator component"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"
PV = "0.0.0+svn${SRCDATE}"

DEPENDS = "ossiecf ossie-standardinterfaces ossie-sigproc"

S = "${WORKDIR}/Interpolator"


SRC_URI = "svn://ossie-dev.mprg.org/repos/ossie/components/Interpolator/trunk;module=Interpolator;proto=https"

prefix="/sdr"

inherit autotools

FILES_${PN} += "/sdr/dom/xml/Interpolator/*.xml"
FILES_${PN} += "/sdr/dom/bin/Interpolator"

BROKEN = "1"
