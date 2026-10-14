/*
 * Configuration settings for the Boundary Devices itw board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include "mx6_common.h"

#define CONFIG_MACH_TYPE	3769

#define CONFIG_SPLASH_SF_OFFSET		0x1e0000
#define CONFIG_SPLASH_SOURCE

#define CONFIG_PWM_IMX
#define CONFIG_IMX6_PWM_PER_CLK  66000000

#define CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG
#define CONFIG_ITW_ENV_SETTINGS \
	"bootargs_init=" \
		"echo dtype: ${dtype} disk: ${disk} partition: ${active_partition};" \
		"if test ${dtype} = mmc ;then " \
			"setenv bootargs root=/dev/mmcblk${disk}p${active_partition};" \
		"else " \
			"setenv bootargs root=/dev/sda${active_partition};" \
		"fi;" \
		"setenv bootargs $bootargs rw rootfstype=ext4 rootwait cpu=${cpu} board=${board} console=${console},115200 consoleblank=0;" \
		"setenv bootargs $bootargs sw_general=${sw_general} sw_lcd=${sw_lcd} sw_lvds1=${sw_lvds1} sw_lvds2=${sw_lvds2} pci=nomsi;" \
		"if itest.s x != \"x${cma}\" ; then " \
			"setenv bootargs ${bootargs} cma=${cma};" \
		"fi;" \
		"if itest.s x != \"x${disable_giga}\" ; then " \
			"setenv bootargs ${bootargs} fec.disable_giga=1;" \
		"fi;" \
		"if itest.s x != \"x${gpumem}\" ; then " \
			"setenv bootargs ${bootargs} galcore.contiguousSize=${gpumem};" \
		"fi;" \
		"\0" \
	"d_addr=10000002\0" \
	"d_error_pos_7=320 350\0" \
	"d_error_pos_11=320 350\0" \
	"d_error_pos_14=500 350\0" \
	"d_error1_size=10000\0" \
	"d_error1_show=" \
		"if sf read ${d_addr} 120000 ${d_error1_size}; then " \
			"bmp d ${d_addr} ${d_error_pos};" \
		"fi\0" \
	"d_error2_size=40000\0" \
	"d_error2_show=" \
		"if sf read ${d_addr} 130000 ${d_error2_size}; then " \
			"bmp d ${d_addr} ${d_error_pos};" \
		"fi\0" \
	"d_main_pos_7=320 200\0" \
	"d_main_pos_11=500 300\0" \
	"d_main_pos_14=500 300\0" \
	"d_main_size=cee2\0" \
	"d_main_show=" \
		"if sf read ${d_addr} 1E0000 ${d_main_size}; then " \
			"bmp d ${d_addr} ${d_main_pos};" \
		"fi\0" \
	"d_vers_pos_7=700 500\0" \
	"d_vers_pos_11=700 500\0" \
	"d_vers_pos_14=1100 700\0" \
	"d_vers_size=0x19A6\0" \
	"d_vers_show=" \
		"if sf read ${d_addr} E0000 ${d_vers_size}; then " \
			"bmp d ${d_addr} ${d_vers_pos};" \
		"fi\0" \
	"get_dtb=" \
		"if itest.s x == \"x${dtbname}\" ; then " \
			"if itest.s x6SOLO == x${cpu} ; then " \
				"setenv dtbname imx6dl-${board}.dtb;" \
			"elif itest.s x6DL == x${cpu} ; then " \
				"setenv dtbname imx6dl-${board}.dtb;" \
			"elif itest.s x6QP == x${cpu} ; then " \
				"setenv dtbname imx6qp-${board}.dtb;" \
			"else " \
				"setenv dtbname imx6q-${board}.dtb;" \
			"fi; " \
		"fi\0" \
	"get_cma=" \
		"if itest.s x == \"x${cma}\" ; then " \
			"if itest.s x6D == x${cpu} ; then " \
				"setenv cma 400M;" \
			"elif itest.s x6QP == x${cpu} ; then " \
				"setenv cma 512M;" \
			"elif itest.s x6Q == x${cpu} ; then " \
				"setenv cma 512M;" \
			"fi; " \
		"fi\0" \
	"init_display_pos=" \
		"if itest ${sw_lvds1} == 6; then " \
			"setenv d_main_pos ${d_main_pos_14};" \
			"setenv d_error_pos ${d_error_pos_14};" \
			"setenv d_vers_pos ${d_vers_pos_14};" \
		"elif itest ${sw_lvds1} == 1; then " \
			"setenv d_main_pos ${d_main_pos_11};" \
			"setenv d_error_pos ${d_error_pos_11};" \
			"setenv d_vers_pos ${d_vers_pos_11};" \
		"else " \
			"setenv d_main_pos ${d_main_pos_7};" \
			"setenv d_error_pos ${d_error_pos_7};" \
			"setenv d_vers_pos ${d_vers_pos_7};" \
		"fi\0" \
	"init_env=" \
		"if sf read 0x10800000 0xC0000 8; then " \
			"if itest.l *0x10800000 -eq 0xffffffff; then " \
				"echo setting environment to flash;" \
				"saveenv;" \
			"else " \
				"echo environment set;" \
				"md 0x10800000 1;" \
			"fi;" \
		"fi\0"  \
	"init_work=" \
		"run init_display_pos;" \
		"if sf probe; then " \
			"run init_env d_main_show d_vers_show;" \
		"fi\0" \
		"dhcp 10008000 ${bmp} && sf erase 1e0000 10000 && sf write 10008000 1e0000 ${filesize} && setenv d_main_size ${filesize};" \
		"dhcp 10008000 u-version.bmp && sf erase e0000 10000 && sf write 10008000 e0000 ${filesize} && setenv d_vers_size ${filesize};" \
		"dhcp 10008000 u-diagnostics.bmp.gz && sf erase 120000 10000 && sf write 10008000 120000 ${filesize} && setenv d_error1_size ${filesize};" \
		"dhcp 10008000 no_disk.bmp.gz && sf erase 130000 10000 && sf write 10008000 130000 ${filesize} && setenv d_error2_size ${filesize};" \
		"\0" \
	"recovery_action=none\0" \
	"version=x.x.xx\0" \
	"mac_addr_high=0x0123\0" \
	"mac_addr_low=0x456789ab\0" \
	"set_fix_live_partition=setenv active_partition 2; setenv recovery_action fix_disk; saveenv\0" \

/*
 * u-version.bmp	 6566( 19A6) bytes, offset  e0000
 * u-diagnostics.bmp	65536(10000) bytes, offset 120000
 * no_disk.bmp		65536(10000) bytes, offset 130000
 * hti8.bmp		40218( 9D1A) bytes, offset 1e0000
 * hts8.bmp		52962( CEE2) bytes, offset 1e0000
 * htx8.bmp		52962( CEE2) bytes, offset 1e0000
 */
#define CONFIG_CI_UDC
#ifdef CONFIG_MX6Q
#define CONFIG_CMD_SATA
#endif
#define CONFIG_CMD_SF
#define CONFIG_FEC_MXC_PHYADDR		4
#define CONFIG_IMX_HDMI
#define CONFIG_PHY_ATHEROS
#define CONFIG_PREBOOT	"run init_work;if kbd; then run set_fix_live_partition;fi"
#define CONFIG_SYS_FSL_USDHC_NUM	2
#define CONFIG_USB_MAX_CONTROLLER_COUNT 2
#define CONFIG_VIDEO

#define BD_BOOTCMD \
	"beep=2;" \
	"echo Hobart i.mx6 auto board programmer......;" \
	"if sf probe; then " \
		"sf erase 0x0 0xc0000;" \
		"sf erase 0xc0000 0x2000;" \
	"fi;" \
	"echo flash erased!;" \
	"mmc dev 1;" \
	"echo mmc initialized.;" \
	"if itest.s x6SOLO == x${cpu} ; then " \
		"ext2load mmc 1 10800000 u-boot-s-itw.imx;"\
		"echo u-boot solo image loaded into memory.;"\
	"elif itest.s x6D == x${cpu} ; then " \
		"ext2load mmc 1 10800000 u-boot-itw.imx;"\
		"echo u-boot dual image loaded into memory.;"\
	"elif itest.s x6Q == x${cpu} ; then " \
		"ext2load mmc 1 10800000 u-boot-q-itw.imx;"\
		"echo u-boot quad image loaded into memory.;"\
	"else " \
		"ext2load mmc 1 10800000 u-boot-qp-itw.imx;"\
		"echo u-boot quadP image loaded into memory.;"\
	"fi; " \
	"sf write 10800000 0x400 ${filesize};" \
	"echo u-boot programmed....;" \
	"if itest.s x6SOLO == x${cpu} ; then " \
		"ext2load mmc 1 10800000 /uboot/hts8.bmp;"\
		"echo u-boot branding splash loaded into memory.;"\
	"elif itest.s x6D == x${cpu} ; then " \
		"ext2load mmc 1 10800000 /uboot/hti8.bmp;"\
		"echo u-boot branding splash loaded into memory.;"\
	"elif itest.s x6Q == x${cpu} ; then " \
		"ext2load mmc 1 10800000 /uboot/htx8.bmp;"\
		"echo u-boot branding splash loaded into memory.;"\
	"else " \
		"ext2load mmc 1 10800000 /uboot/htqp8.bmp;"\
		"echo u-boot branding splash loaded into memory.;"\
	"fi; " \
	"sf write 10800000 0x1e0000 ${filesize};" \
	"echo u-boot branding splash programmed.;"\
	"ext2load mmc 1 10800000 /uboot/u-version.bmp;"\
	"sf write 10800000 0xe0000 ${filesize};" \
	"echo version splash programmed.;"\
	"ext2load mmc 1 10800000 /uboot/u-diagnostics.bmp;"\
	"sf write 10800000 0x120000 ${filesize};" \
	"echo diagnostics splash programmed.;"\
	"fuse read 0 5;" \
	"if itest.s 00000000 == ${fuse_read_val0} ; then " \
		"echo programming efuses boot.;"\
		"fuse prog 0 5 0x18000030;"\
		"fuse prog 0 6 0x00000010;"\
	"else " \
		"echo boot efuses already programmed!;"\
	"fi; " \
	"fuse read 4 3;" \
	"if itest.s 00000000 == ${fuse_read_val0} ; then " \
		"echo programming efuses MAC.;"\
		"fuse prog 4 3 ${mac_addr_high};" \
		"fuse prog 4 2 ${mac_addr_low};" \
		"echo MAC programming of efuses finished.;"\
	"else " \
		"echo mac efuses already programmed!;"\
	"fi; " \
	"echo erasing environment variable section.;"\
	"sf erase 0xc0000 0x2000;"\
	"echo board programmed successfully.;"\
	"run d_error${beep}_show;" \
	"itw_beep ${beep};" \

#define BD_I2C_MASK	7
#define BD_NOVIDEO_CONSOLE

#include "boundary.h"
#define CONFIG_EXTRA_ENV_SETTINGS BD_BOUNDARY_ENV_SETTINGS \
	CONFIG_ITW_ENV_SETTINGS \
	"bootcmd_std=" BD_BOOTCMD_STD "\0" \

#endif	       /* __CONFIG_H */
