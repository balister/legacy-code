DESCRIPTION = "OSSIE Standard port interfaces"
SECTION =  "libs"
PRIORITY = "optional"
LICENSE = "LGPL"

PR = "r1"
PV = "0.0.0+svnr${SRCREV}"

DEPENDS = "ossie-cf"

S = "${WORKDIR}/standardInterfaces"

SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/standardInterfaces/trunk;module=standardInterfaces;proto=https"

inherit autotools pkgconfig

EXTRA_OECONF = " --with-omniorb=${STAGING_BINDIR}/.. IDL=${STAGING_BINDIR_NATIVE}/omniidl OSSIE_CFLAGS=-I${STAGING_INCDIR}"

CXXFLAGS_powerpc += "-lstdc++"

do_stage () {
	autotools_stage_all
}
BROKEN = "1"
