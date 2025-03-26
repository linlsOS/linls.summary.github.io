# PL LK vibrator

PL/LK阶段振动

# LK振动

```diff
diff --git a/mediatek/proprietary/bootable/bootloader/lk/platform/mt6761/platform.c b/mediatek/proprietary/bootable/bootloader/lk/platform/mt6761/platform.c
index 231fdc77df..1b73d4c550 100644
--- a/mediatek/proprietary/bootable/bootloader/lk/platform/mt6761/platform.c
+++ b/mediatek/proprietary/bootable/bootloader/lk/platform/mt6761/platform.c
@@ -611,7 +611,23 @@ static void lk_vb_vfy_logo(void)
        PROFILING_END();
 #endif
 }
-
+void vibr_Enable_HW(void)
+{
+//  #ifdef CONFIG_MTK_PMIC_CHIP_MT6357 
+       pmic_set_register_value(PMIC_RG_VIBR_VOSEL_ADDR, 5);
+       mdelay(10);
+       pmic_set_register_value(PMIC_RG_LDO_VIBR_EN, 1);        
+ // #endif
+  
+  }
+ 
+  void vibr_Disable_HW(void)
+ {
+       //pmic_enable_interrupt(PMIC_RG_INT_EN_VIBR_OC_ADDR, 0, "vibr");
+ // #ifdef CONFIG_MTK_PMIC_CHIP_MT6357
+       pmic_set_register_value(PMIC_RG_LDO_VIBR_EN, 0);
+ // #endif
+  }
 void platform_init(void)
 {
        bool bearly_backlight_on = false;
@@ -703,7 +719,9 @@ void platform_init(void)
 
        /*for kpd pmic mode setting*/
        set_kpd_pmic_mode();
-
+        vibr_Enable_HW();
+        mdelay(300);
+        vibr_Disable_HW();
 #ifndef MACH_FPGA_NO_DISPLAY
        PROFILING_START("load_logo");
        logo_size = mboot_common_load_logo((unsigned long)mt_get_logo_db_addr_pa(), "logo");
```

# LK关机充电振动
```diff
diff --git a/mediatek/proprietary/bootable/bootloader/lk/platform/mt6761/platform.c b/mediatek/proprietary/bootable/bootloader/lk/platform/mt6761/platform.c
index 1b73d4c550..9f76645a53 100644
--- a/mediatek/proprietary/bootable/bootloader/lk/platform/mt6761/platform.c
+++ b/mediatek/proprietary/bootable/bootloader/lk/platform/mt6761/platform.c
@@ -860,6 +860,8 @@ void platform_init(void)
 #ifdef MTK_KERNEL_POWER_OFF_CHARGING
        if (kernel_charging_boot() == 1) {
                PROFILING_START("show logo");
+               vibr_Enable_HW();
+               vibr_Disable_HW();
 #ifdef MTK_BATLOWV_NO_PANEL_ON_EARLY
                CHARGER_TYPE CHR_Type_num = CHARGER_UNKNOWN;
                CHR_Type_num = hw_charging_get_charger_type();
```

# PL振动
```diff
diff --git a/mediatek/proprietary/bootable/bootloader/preloader/platform/mt6761/src/drivers/platform.c b/mediatek/proprietary/bootable/bootloader/preloader/platform/mt6761/src/drivers/platform.c
index 6e0f288281..2f769591b7 100644
--- a/mediatek/proprietary/bootable/bootloader/preloader/platform/mt6761/src/drivers/platform.c
+++ b/mediatek/proprietary/bootable/bootloader/preloader/platform/mt6761/src/drivers/platform.c
@@ -68,6 +68,8 @@
 #include <mtk_subpmic.h>
 #include <anti_rollback.h>
 #include <verified_boot_error.h>
+#include "upmu_hw.h"
+
 #ifdef MTK_TRNG_CALIBRATION
 #if defined MTK_TRNG_CALIBRATION_1ST_ROUND
 #define TRNG_CALIB_BUF_SIZE  (28 + 24 * 1066)
@@ -1440,7 +1442,27 @@ void platform_pre_init(void)
 #endif //no PTP in MT6761
 }
 
+#define pmic_set_register_value(flagname, val) \
+        pmic_config_interface(flagname##_ADDR, (val), flagname##_MASK, flagname##_SHIFT)
+
+#define pmic_get_register_value(flagname) \
+({      \
+        unsigned int val = 0;   \
+        pmic_read_interface(flagname##_ADDR, &val, flagname##_MASK, flagname##_SHIFT);  \
+        val;    \
+})
+
+void vibr_Enable_HW(void)
+{
+    pmic_set_register_value(PMIC_RG_VIBR_VOSEL, 5);
+    mdelay(10);
+    pmic_set_register_value(PMIC_RG_LDO_VIBR_EN, 1);
+}
 
+void vibr_Disable_HW(void)
+{
+    pmic_set_register_value(PMIC_RG_LDO_VIBR_EN, 0);
+}
 
 static void peribus_init(void)
 {
@@ -1500,8 +1522,12 @@ void platform_init(void)
 #endif
     BOOTING_TIME_PROFILING_LOG("rtc_bbpu_power_on");
 
+    vibr_Enable_HW();
+    mdelay(300);
+    vibr_Disable_HW();
+
 #if !CFG_FPGA_PLATFORM
     pl_battery_init(false);
 #endif
 
     enable_PMIC_kpd_clock();
```
