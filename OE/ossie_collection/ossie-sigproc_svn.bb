DESCRIPTION = "OSSIE Signal Processing routines"
SECTION =  "libs"
PRIORITY = "optional"
LICENSE = "LGPL"
PV = "0.0.0+svn${SRCDATE}"

DEPENDS = "ossiecf"

S = "${WORKDIR}/SigProc"

SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/SigProc/trunk;module=SigProc;proto=https"

inherit autotools pkgconfig

EXTRA_OECONF_append_arm = " --enable-fpm=arm"

CXXFLAGS_powerpc = "-lstdc++"

do_stage () {
	autotools_stage_all
}
BROKEN = "1"
