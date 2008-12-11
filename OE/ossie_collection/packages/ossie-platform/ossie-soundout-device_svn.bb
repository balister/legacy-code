DESCRIPTION = "OSSIE sound playback device"
SECTION =  "apps"
PRIORITY = "optional"
MAINTAINER = "Philip Balister philip@balister.org"
LICENSE = "GPL"

PR = "r1"
PV = "0.0.0+svnr${SRCREV}"

DEPENDS = "pulseaudio-meta ossie-cf ossie-standardinterfaces"
RDEPENDS = "pulseaudio-meta"

S = "${WORKDIR}/trunk"

SRC_URI = "svn://svn.geekisp.com/opensdr/sca/platform/Sound_out;module=trunk;proto=https"

prefix="/sdr"

inherit autotools

EXTRA_OECONF = "--with-omniorb=${STAGING_BINDIR}/.. IDL=${STAGING_BINDIR_NATIVE}/omniidl OSSIE_CFLAGS=-I${STAGING_INCDIR} SI_PATH=${STAGING_INCDIR}"

FILES_${PN} = "${prefix}/dev/xml/soundCardPlayback/*xml ${prefix}/dev/bin/*"
FILES_${PN}-dbg = "${prefix}/dev/bin/.debug

