DESCRIPTION = "SPI source/sink blocks for GNU Radio"
SECTION = "apps"
PRIORITY = "optional"
LICENSE = "GPL"
DEPENDS = "gnuradio python swig-native linux-libc-headers"
RDEPENDS = "gnuradio"

PN = "gr-spi"
PV = "0.2"

SRC_URI = "file://gr-spi-${PV}.tar.bz2"
S = "${WORKDIR}/${PN}-${PV}"

inherit autotools

export BUILD_SYS
export HOST_SYS=${MULTIMACH_TARGET_SYS}

EXTRA_OECONF += "  --with-pythondir=/usr/lib/python2.6/packages \
  PYTHON_CPPFLAGS=-I${STAGING_INCDIR}/python2.6 \
  GNURADIO_CORE_INCLUDEDIR=${STAGING_INCDIR}/gnuradio \
"

do_configure_append() {
        find ${S} -name Makefile | xargs sed -i s:'-I/usr/include':'-I${STAGING_INCDIR}':g
        find ${S} -name Makefile | xargs sed -i s:'GNURADIO_CORE_INCLUDEDIR = /usr/include/gnuradio':'GNURADIO_CORE_INCLUDEDIR = ${STAGING_INCDIR}/gnuradio':g
        find ${S} -name Makefile | xargs sed -i s:'grincludedir = $(includedir)/gnuradio':'grincludedir = ${STAGING_INCDIR}/gnuradio':g
}

