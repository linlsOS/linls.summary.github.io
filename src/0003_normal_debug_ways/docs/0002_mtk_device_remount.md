# mtk device remount

mtk android 设备如何remount

# 方法

* 打开开发者模式
* 设置->开发者选项->oem unlocking设置未开
* adb reboot bootloader
* fastboot flashing unlock
* fastboot reboot
* adb root
* adb disable-verity
* adb reboot
* adb root
* adb remount

