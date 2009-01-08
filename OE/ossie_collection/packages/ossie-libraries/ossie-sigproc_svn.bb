DESCRIPTION = "OSSIE Signal Processing routines"
SECTION =  "libs"
PRIORITY = "optional"
LICENSE = "LGPL"

PR = "r2"
PV = "0.0.0+svnr${SRCREV}"

DEPENDS = "ossie-cf"

S = "${WORKDIR}/trunk"

SRC_URI = "svn://svn.geekisp.com/opensdr/sca/SigProc;module=trunk;proto=https"

inherit autotools pkgconfig

EXTRA_OECONF_append_armv7a = " --enable-fpm=arm"
EXTRA_OECONF_append_armv5 = " --enable-fpm=arm"
EXTRA_OECONF_append_powerpc = " --enable-fpm=ppc"

do_stage () {
	autotools_stage_all
}

