PR = "r2"

ANGSTROM_EXTRA_INSTALL ?= ""
DISTRO_SSH_DAEMON ?= "dropbear"

IMAGE_INSTALL = "task-boot \
            task-base \
            util-linux-mount util-linux-umount \
            ${DISTRO_SSH_DAEMON} \
            task-ossie \
            angstrom-version \
           "

export IMAGE_BASENAME = "ossie-image"
IMAGE_LINGUAS = ""


inherit image

