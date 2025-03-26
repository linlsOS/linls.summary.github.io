# Vibrator Shell

命令行控制振动

# 触发操作

```sh
adb shell
cd /sys/class/leds/vibrator
echo 10000 > duration //振动多久
echo 1 > activate     //启动振动
```