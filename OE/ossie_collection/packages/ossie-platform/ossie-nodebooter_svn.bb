DESCRIPTION = "OSSIE Node Booter"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"

PR = "r2"
PV = "0.0.0+svnr${SRCREV}"

DEPENDS = "ossie-cf"

S = "${WORKDIR}/trunk"

SRC_URI = "svn://svn.geekisp.com/opensdr/sca/platform/nodebooter;module=trunk;proto=https"

inherit autotools

FILES_${PN} += "/sdr/dom/domain/DomainManager*"

