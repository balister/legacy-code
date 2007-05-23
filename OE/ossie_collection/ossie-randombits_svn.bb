DESCRIPTION = "OSSIE Random bit generator component"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"
PV = "0.0.0+svn${SRCDATE}"

DEPENDS = "ossiecf ossie-standardinterfaces ossie-sigproc"

S = "${WORKDIR}/RandomBits"

SRC_URI = "svn://oe:oe@ossie-dev.mprg.org/repos/ossie/components/RandomBits/trunk;module=RandomBits;proto=https"

prefix="/home/sca"

inherit autotools

FILES_${PN} += "/home/sca/xml/RandomBits/*.xml"

BROKEN = "1"