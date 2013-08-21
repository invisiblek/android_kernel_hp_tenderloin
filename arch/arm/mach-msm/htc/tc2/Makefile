obj-$(CONFIG_MACH_TC2) += board-tc2.o
obj-$(CONFIG_MACH_TC2) += board-tc2-regulator.o
obj-$(CONFIG_MACH_TC2) += board-tc2-gpiomux.o
obj-$(CONFIG_MACH_TC2) += board-tc2-storage.o
obj-$(CONFIG_MACH_TC2) += board-tc2-mmc.o
obj-$(CONFIG_MACH_TC2) += board-tc2-rfkill.o
obj-$(CONFIG_MACH_TC2) += board-tc2-wifi.o
obj-$(CONFIG_MACH_TC2) += board-tc2-gpu.o
#obj-$(CONFIG_MACH_TC2) += board-tc2-audio.o
obj-$(CONFIG_MACH_TC2) += board-tc2-pmic.o
obj-$(CONFIG_MACH_TC2) += board-tc2-keypad.o
ifeq ($(CONFIG_MSM_CAMERA),y)
obj-$(CONFIG_MACH_TC2) += board-tc2-camera.o
endif
obj-$(CONFIG_MACH_TC2) += display/
CFLAGS_board-tc2-display.o += -Idrivers/video
