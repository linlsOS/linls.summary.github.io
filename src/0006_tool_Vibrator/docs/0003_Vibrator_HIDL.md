# Vibrator HIDL

分析Vibrator HIDL启动、调用

# hidl

log

```
09-14 16:29:13.807226   614   614 D Vibrator: Vibrator perform 2, 0
09-14 16:29:13.807358   614   614 I Vibrator: Vibrator on for timeoutMs: 30
09-14 16:29:13.808649   614   614 D Vibrator: Vibrator using LED trigger
09-14 16:29:13.840869   614   614 D Vibrator: Vibrator off
09-14 16:29:13.841120   614   614 D Vibrator: Vibrator using LED trigger
```

vendor/mediatek/proprietary/hardware/libvibrator/aidl/default/vibrator-mtk-default.rc
```
service vendor.vibrator-default /vendor/bin/hw/android.hardware.vibrator-service.mediatek
    class hal
    user system
    group system
```

vendor/mediatek/proprietary/hardware/libvibrator/aidl/default/Vibrator.cpp
```cpp
// ...省略
static const char LED_DEVICE[] = "/sys/class/leds/vibrator";

static int write_led_file(const char *file, const char *value)
{
    char file_str[50];

    int bytes = snprintf(file_str, sizeof(file_str), "%s/%s", LED_DEVICE, file);
    if (bytes >= sizeof(file_str)) return -EINVAL;
    return write_value(file_str, value);
}

static bool vibra_led_exists()
{
    char file_str[50];

    int bytes = snprintf(file_str, sizeof(file_str), "%s/%s", LED_DEVICE, "activate");
    if (bytes >= sizeof(file_str)) return -EINVAL;
    return device_exists(file_str);
}

// ...省略

ndk::ScopedAStatus Vibrator::off() {
    ALOGD("Vibrator off");
    if (vibra_exists()) {
        ALOGD("Vibrator using timed_output");
        vibra_off();
    } else if (vibra_led_exists()) {
        ALOGD("Vibrator using LED trigger");
        vibra_led_off();
    } else {
        ALOGI("Vibrator device does not exist. Cannot start vibrator");
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Vibrator::on(int32_t timeoutMs,
                                const std::shared_ptr<IVibratorCallback>& callback) {
    ALOGI("Vibrator on for timeoutMs: %d", timeoutMs);

    if (vibra_exists()) {
        ALOGD("Vibrator using timed_output");
        vibra_on(timeoutMs);
    } else if (vibra_led_exists()) {
        ALOGD("Vibrator using LED trigger");
        vibra_led_on(timeoutMs);
    } else {
        ALOGI("Vibrator device does not exist. Cannot start vibrator");
    }
    if (callback != nullptr) {
#ifndef VIBR_EFFECT_SUPPORT
        return ndk::ScopedAStatus(AStatus_fromExceptionCode(EX_UNSUPPORTED_OPERATION));
#else
        std::thread([=] {
            ALOGD("Starting on on another thread");
            usleep(timeoutMs * 1000);
            ALOGD("Notifying on complete");
            if (!callback->onComplete().isOk()) {
                ALOGI("Failed to call onComplete");
            }
        }).detach();
    return ndk::ScopedAStatus::ok();
#endif
    } else {
        return ndk::ScopedAStatus::ok();
    }

}
```