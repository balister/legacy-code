DESCRIPTION = "OSSIE Waveform Loader"
SECTION =  "apps"
PRIORITY = "optional"
LICENSE = "GPL"
PV = "0.0.0+svnr${SRCREV}"
PR = "r0"

DEPENDS = "ossie-cf"

S="${WORKDIR}/c_wavLoader"

SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/platform/c_wavLoader/trunk;module=c_wavLoader;proto=https"

inherit autotools

BROKEN = "1"
