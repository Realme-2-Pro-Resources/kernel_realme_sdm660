#
# Makefile for the touchscreen drivers.
#
subdir-ccflags-$(CONFIG_TOUCHPANEL_MTK_PLATFORM) += -I$(srctree)/drivers/misc/mediatek/include/

subdir-ccflags-y := -finline-functions -ftree-vectorize -funroll-loops

obj-$(CONFIG_TOUCHPANEL_OPPO) += util_interface/
obj-$(CONFIG_TOUCHPANEL_OPPO) += touchpanel_common_driver.o
obj-$(CONFIG_TOUCHPANEL_NOVA) += Novatek/
obj-$(CONFIG_TOUCHPANEL_HIMAX) += Himax/

