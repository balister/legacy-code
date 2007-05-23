DESCRIPTION = "OSSIE USRP TX side control component"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"
PV = "0.0.0+svn${SRCDATE}"

DEPENDS = "ossiecf ossie-standardinterfaces"

S = "${WORKDIR}/USRP_TX_Control"


SRC_URI = "svn://oe:oe@ossie-dev.mprg.org/repos/ossie/components/USRP_TX_Control/trunk;module=USRP_TX_Control;proto=https"

prefix="/home/sca"

inherit autotools

FILES_${PN} += "/home/sca/xml/USRP_TX_Control/*.xml"

BROKEN = "1"
