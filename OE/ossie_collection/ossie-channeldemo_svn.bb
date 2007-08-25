DESCRIPTION = "OSSIE Demo channel component"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"
PV = "0.0.0+svn${SRCDATE}"

DEPENDS = "ossiecf"

S = "${WORKDIR}/ChannelDemo"


SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/components/ChannelDemo/trunk;module=ChannelDemo;proto=https"

prefix="/sdr"

inherit autotools

FILES_${PN} += "${prefix}/dom/xml/ChannelDemo/*.xml ${prefix}/dom/bin"
FILES_${PN}-dbg = "${prefix}/dom/bin/.debug"

BROKEN = "1"
