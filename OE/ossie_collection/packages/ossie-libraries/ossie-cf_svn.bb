DESCRIPTION = "OSSIE Core Framework"
SECTION =  "libs"
PRIORITY = "optional"
LICENSE = "LGPL"
DEPENDS = "omniorb boost libtool-cross"

PR = "r1"
PV = "0.0.0+svnr${SRCREV}"

S = "${WORKDIR}/trunk"

SRC_URI = "svn://svn.geekisp.com/opensdr/sca/ossie-sca;module=trunk;proto=https"

inherit autotools pkgconfig

EXTRA_OECONF = "--with-omniorb=${STAGING_BINDIR}/.. IDL=${STAGING_BINDIR_NATIVE}/omniidl"

#EXTRA_OEMAKE = "CXXFLAGS=-DBOOST_1_34"

do_stage () {
	autotools_stage_all
}

FILES_${PN} += "/sdr/dom/xml/dtd/*.dtd"

