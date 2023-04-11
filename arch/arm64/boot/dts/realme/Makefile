dtb-$(CONFIG_ARCH_SDM660) := sdm660-mtp-RMX1801.dtb

ifeq ($(CONFIG_ARM64),y)
always          := $(dtb-y)
always          += $(dtbo-y)
subdir-y        := $(dts-dirs)
else
targets += dtbs
targets += $(addprefix ../, $(dtb-y))

$(obj)/../%.dtb: $(src)/%.dts FORCE
	$(call if_changed_dep,dtc)

dtbs: $(addprefix $(obj)/../,$(dtb-y))
endif
clean-files := *.dtbo *.dtb
