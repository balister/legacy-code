DESCRIPTION = "OSSIE Node Booter"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svn${SRCDATE}"

DEPENDS = "ossie-cf"

S = "${WORKDIR}/nodebooter"

SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/platform/nodebooter/trunk;module=nodebooter;proto=https"

inherit autotools

FILES_${PN} += "/sdr/dom/domain/DomainManager*"

BROKEN = "1"
