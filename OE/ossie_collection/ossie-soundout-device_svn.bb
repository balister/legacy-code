DESCRIPTION = "OSSIE sound playback device"
SECTION =  "apps"
PRIORITY = "optional"
MAINTAINER = "Philip Balister philip@balister.org"
LICENSE = "GPL"
PV = "0.0.0+svn${SRCDATE}"

DEPENDS = "alsa-lib ossiecf ossie-standardinterfaces"

S = "${WORKDIR}/Sound_out"

SRC_URI = "svn://ossie-dev.mprg.org/repos/ossie/platform/Sound_out/trunk;module=Sound_out;proto=https"

prefix="/sdr"

inherit autotools

EXTRA_OECONF = "--with-omniorb=${STAGING_BINDIR}/.. IDL=${STAGING_BINDIR_NATIVE}/omniidl"

FILES_${PN} += "/home/sca/xml/soundCard/*xml"

BROKEN = "1"
