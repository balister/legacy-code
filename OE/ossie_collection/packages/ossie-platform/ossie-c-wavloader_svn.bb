DESCRIPTION = "OSSIE Waveform Loader"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"
PV = "0.0.0+svnr${SRCREV}"
PR = "r0"

DEPENDS = "ossie-cf"

S="${WORKDIR}/trunk"

SRC_URI = "svn://svn.geekisp.com/opensdr/sca/platform/c_wavLoader;module=trunk;proto=https"

inherit autotools

