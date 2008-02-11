DESCRIPTION = "Meta-package for OSSIE Software Defined Radio (SDR)"
LICENSE = "MIT"

RDEPENDS = "${OSSIE_BASE} ${OSSIE_PLATFORM} ${OSSIE_COMPONENTS} ${OSSIE_WAVEFORMS}"

PR = "r0"

PACKAGES = "task-ossie"

ALLOW_EMPTY = "1"

PACKAGE_INSTALL = "${OSSIE_BASE} ${OSSIE_PLATFORM} ${OSSIE_COMPONENTS} ${OSSIE_WAVEFORMS}"

OSSIE_BASE = "screen procps omniorb exmap-console exmap-server alsa-utils-aplay"
OSSIE_PLATFORM = "ossie-cf ossie-standardinterfaces ossie-nodebooter ossie-c-wavloader"
OSSIE_COMPONENTS = "ossie-gpp-device ossie-soundout-device"
OSSIE_WAVEFORMS = "ossie-demo ossie-wfmrxmono"
