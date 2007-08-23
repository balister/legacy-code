DESCRIPTION = "Meta-package for OSSIE Software Defined Radio (SDR)"
LICENSE = "MIT"

DEPENDS = "${OSSIE_BASE} ${OSSIE_PLATFORM} ${OSSIE_COMPONENTS} ${OSSIE_WAVEFORMS} gnuradio"

PR = "r0"

PACKAGES = "task-ossie"

ALLOW_EMPTY = "1"

PACKAGE_INSTALL = "${OSSIE_BASE} ${OSSIE_PLATFORM} ${OSSIE_COMPONENTS} ${OSSIE_WAVEFORMS} gnuradio"

OSSIE_BASE = "screen procps omniorb"
OSSIE_PLATFORM = "ossiecf ossie-standardinterfaces ossie-nodebooter ossie-c-wavloader"
OSSIE_COMPONENTS = "ossie-gpp-device ossie-usrp-device"
OSSIE_WAVEFORMS = "ossie-tx-random-data"
