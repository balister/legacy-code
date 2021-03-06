DESCRIPTION = "OSSIE USRP TX side control component"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svnr${SRCREV}"

DEPENDS = "ossie-cf ossie-standardinterfaces"

S = "${WORKDIR}/USRP_TX_Control"


SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/components/USRP_TX_Control/trunk;module=USRP_TX_Control;proto=https"

inherit autotools

prefix = "/sdr"

FILES_${PN} += "/sdr/dom/xml/USRP_TX_Control/*.xml"
FILES_${PN} += "/sdr/dom/bin/USRP_TX_Control"

BROKEN = "1"
