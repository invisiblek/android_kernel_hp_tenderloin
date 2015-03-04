subdir-ccflags-$(CONFIG_ARCH_MSM) += -Iarch/arm/mach-msm

-include $(src)/Makefile.board

obj-y += devices_htc.o
obj-y += radio_feedback.o
obj-y += htc_port_list.o
obj-y += htc_bdaddress.o
obj-y += htc_ramdump.o
obj-y += htc_awb_cal.o
obj-y += htc_wifi_nvs.o
obj-y += htc_sysinfo.o
obj-y += emmc_partitions.o
ifeq ($(CONFIG_ARCH_MSM8X60),y)
obj-y += htc_acoustic_8x60.o
else
obj-y += htc_acoustic_8960.o
endif
obj-y += htc_lpm.o

obj-$(CONFIG_MACH_ZARA) += htc_sleep_clk.o
obj-$(CONFIG_MACH_M4_UL) += htc_sleep_clk.o
obj-$(CONFIG_MACH_M7_UL) += htc_4335_wl_reg.o

obj-$(CONFIG_HTC_BATT_CORE) += htc_battery_core.o
obj-$(CONFIG_HTC_BATT_8x60) += htc_battery_8x60.o
obj-$(CONFIG_HTC_BATT_8960) += htc_battery_8960.o htc_battery_cell.o
obj-$(CONFIG_HTC_CLK_CTRL) += htc_sleep_clk_8k.o htc_fast_clk_8k.o
obj-$(CONFIG_FORCE_FAST_CHARGE) += fastchg.o
obj-$(CONFIG_MSM_NATIVE_RESTART) += htc_restart_handler.o

obj-$(CONFIG_HTC_HEADSET_MGR) += htc_headset_mgr.o
obj-$(CONFIG_HTC_HEADSET_MISC) += htc_headset_misc.o
obj-$(CONFIG_HTC_HEADSET_PMIC) += htc_headset_pmic.o
obj-$(CONFIG_HTC_HEADSET_ONE_WIRE) += htc_headset_one_wire.o
obj-$(CONFIG_HTC_HEADSET_GPIO) += htc_headset_gpio.o
obj-$(CONFIG_HTC_HEADSET_8X60) += htc_headset_8x60.o

obj-$(CONFIG_HTC_SIMLOCK) += htc_simlock.o

obj-$(CONFIG_ARCH_MSM8930) += ../devices-8930.o ../board-8930-gpu.o

CFLAGS_devices_htc.o += -Idrivers/staging/android
