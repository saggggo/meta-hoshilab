# require recipes-sato/images/core-image-sato-sdk.bb
#require recipes-images/angstrom/angstrom-image.bb
require recipes-images/angstrom/systemd-qt5-image.bb

DESCRIPTION = "hoshilab linux"

PACKAGE_INSTALL += " \
  packagegroup-self-hosted-debug \
  packagegroup-self-hosted-sdk \
  packagegroup-self-hosted-extended \
  packagegroup-self-hosted-host-tools \
  packagegroup-core-full-cmdline \
  packagegroup-qt5-toolchain-target \
"


LICENSE = "MIT"

#PACKAGE_INSTALL += " packagegroup-self-hosted-debug"
# IMAGE_FSTYPES = "ext4"
#

IMAGE_INSTALL += " \
  postgresql \
  python3 \
  python3-pandas \
  python3-numpy \
  python3-periphery \
  qtquickcontrols \
  nodejs \
  kernel-module-hoshilab \
"

export IMAGE_BASENAME = "hoshilab-linux"
