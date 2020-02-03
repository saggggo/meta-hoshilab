FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI += "\
      file://fragment.cfg \
      file://socfpga_cyclone5_sockit.dts \
"

KERNEL_DEVICETREE = "socfpga_cyclone5_sockit.dtb"

do_copy_defconfig_append() {
  cat ${WORKDIR}/*.cfg >> ${B}/.config
  cat ${WORKDIR}*.cfg >> ${B}/../defconfig
}
