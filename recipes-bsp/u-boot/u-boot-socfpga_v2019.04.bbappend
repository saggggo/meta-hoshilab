#require ${COREBASE}/meta-altera/recipes-bsp/u-boot/u-boot-common-env.inc
FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"



SRC_URI += "\
      file://0001-hoshilab-custom.patch \
"
