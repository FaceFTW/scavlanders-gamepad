#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/simplelink_msp432e4_sdk_3_20_00_10/source;C:/ti/simplelink_msp432e4_sdk_3_20_00_10/kernel/tirtos/packages;C:/ti/simplelink_msp432p4_sdk_3_40_01_02/source;C:/ti/simplelink_msp432p4_sdk_3_40_01_02/kernel/tirtos/packages;C:/ti/simplelink_sdk_ble_plugin_3_20_00_24/source;D:/GDrive/ECE230/scavlanders-gamepad/firmware/.config
override XDCROOT = C:/ti/ccs1011/xdctools_3_61_02_27_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/simplelink_msp432e4_sdk_3_20_00_10/source;C:/ti/simplelink_msp432e4_sdk_3_20_00_10/kernel/tirtos/packages;C:/ti/simplelink_msp432p4_sdk_3_40_01_02/source;C:/ti/simplelink_msp432p4_sdk_3_40_01_02/kernel/tirtos/packages;C:/ti/simplelink_sdk_ble_plugin_3_20_00_24/source;D:/GDrive/ECE230/scavlanders-gamepad/firmware/.config;C:/ti/ccs1011/xdctools_3_61_02_27_core/packages;..
HOSTOS = Windows
endif
