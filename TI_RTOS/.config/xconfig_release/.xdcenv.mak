#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/simplelink_msp432p4_sdk_3_20_00_06/source;C:/ti/simplelink_msp432p4_sdk_3_20_00_06/kernel/tirtos/packages;D:/GDrive/ECE230/scavlanders-gamepad/TI_RTOS/.config
override XDCROOT = C:/ti/ccs1011/xdctools_3_61_02_27_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/simplelink_msp432p4_sdk_3_20_00_06/source;C:/ti/simplelink_msp432p4_sdk_3_20_00_06/kernel/tirtos/packages;D:/GDrive/ECE230/scavlanders-gamepad/TI_RTOS/.config;C:/ti/ccs1011/xdctools_3_61_02_27_core/packages;..
HOSTOS = Windows
endif
