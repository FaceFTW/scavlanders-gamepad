#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/simplelink_cc2640r2_sdk_1_50_00_58/source;C:/ti/simplelink_cc2640r2_sdk_1_50_00_58/kernel/tirtos/packages;C:/ti/simplelink_cc2640r2_sdk_ble_example_pack_1_50_00_62/source;C:/ti/simplelink_msp432e4_sdk_3_20_00_10/source;C:/ti/simplelink_msp432e4_sdk_3_20_00_10/kernel/tirtos/packages;C:/ti/simplelink_msp432p4_sdk_3_40_01_02/source;C:/ti/simplelink_msp432p4_sdk_3_40_01_02/kernel/tirtos/packages;C:/ti/simplelink_sdk_ble_plugin_3_20_00_24/source;C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/packages;C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/products/tidrivers_cc13xx_cc26xx_2_21_01_01/packages;C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/products/bios_6_46_01_38/packages;C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/products/uia_2_01_00_01/packages;D:/GDrive/ECE230/scavlanders-gamepad/firmware_non_hid/.config
override XDCROOT = C:/ti/ccs1011/xdctools_3_62_00_08_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/simplelink_cc2640r2_sdk_1_50_00_58/source;C:/ti/simplelink_cc2640r2_sdk_1_50_00_58/kernel/tirtos/packages;C:/ti/simplelink_cc2640r2_sdk_ble_example_pack_1_50_00_62/source;C:/ti/simplelink_msp432e4_sdk_3_20_00_10/source;C:/ti/simplelink_msp432e4_sdk_3_20_00_10/kernel/tirtos/packages;C:/ti/simplelink_msp432p4_sdk_3_40_01_02/source;C:/ti/simplelink_msp432p4_sdk_3_40_01_02/kernel/tirtos/packages;C:/ti/simplelink_sdk_ble_plugin_3_20_00_24/source;C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/packages;C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/products/tidrivers_cc13xx_cc26xx_2_21_01_01/packages;C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/products/bios_6_46_01_38/packages;C:/ti/tirtos_cc13xx_cc26xx_2_21_01_08/products/uia_2_01_00_01/packages;D:/GDrive/ECE230/scavlanders-gamepad/firmware_non_hid/.config;C:/ti/ccs1011/xdctools_3_62_00_08_core/packages;..
HOSTOS = Windows
endif
