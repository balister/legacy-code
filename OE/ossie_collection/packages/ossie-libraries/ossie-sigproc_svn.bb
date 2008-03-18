DESCRIPTION = "OSSIE Signal Processing routines"
SECTION =  "libs"
PRIORITY = "optional"
LICENSE = "LGPL"

PR = "r1"
PV = "0.0.0+svnr${SRCREV}"

DEPENDS = "ossie-cf"

S = "${WORKDIR}/SigProc"

SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/SigProc/trunk;module=SigProc;proto=https"

inherit autotools pkgconfig

EXTRA_OECONF_append_arm = " --enable-fpm=arm"
EXTRA_OECONF_append_powerpc = " --enable-fpm=ppc"

do_stage () {
	autotools_stage_all
}

