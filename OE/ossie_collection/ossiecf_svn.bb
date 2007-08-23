DESCRIPTION = "OSSIE Core Framework"
SECTION =  "libs"
PRIORITY = "optional"
LICENSE = "LGPL"
DEPENDS = "omniorb boost libtool-cross"
PV = "0.0.0+svn${SRCDATE}"

SRCDATE = "now"

S = "${WORKDIR}/ossie"

SRC_URI = "svn://ossie.wireless.vt.edu/repos/ossie/ossie/trunk;module=ossie;proto=https"

inherit autotools pkgconfig

EXTRA_OECONF = "--with-omniorb=${STAGING_BINDIR}/.. IDL=${STAGING_BINDIR_NATIVE}/omniidl"

#LDFLAGS += "-lstdc++"

do_stage () {
	autotools_stage_all
}

FILES_${PN} += "/sdr/dom/xml/dtd/*.dtd"

BROKEN = "1"
