# mtk vibrator

mtk vibrator

# menu

* [1 简介](#1-简介)
* [2 驱动代码栈](#2-驱动代码栈)
* [3 hidl](#3-hidl)
* [4 IVibratorManager](#4-IVibratorManager)
* [5 SystemVibratorManager](#5-SystemVibratorManager)
* [6 VibratorManagerService](#6-VibratorManagerService)
* [7 vibrator测试](#7-vibrator测试)

# 1 简介

  主要介绍vibrator从驱动到framework的代码流程及对应的测试方法


# 2 驱动代码栈

```
* drivers/misc/mediatek/vibrator/vibrator_drv.c
  └── module_init(vib_mod_init);
      └── static struct platform_driver vibrator_driver
          └── .probe = vib_probe,
              └── static int vib_probe(struct platform_device *pdev)
                  ├── ret = devm_led_classdev_register(&pdev->dev, &led_vibr);
                  │   └── static struct led_classdev led_vibr
                  │       ├── .name       = "vibrator",
                  │       └── .groups     = vibr_group,
                  │           └── static const struct attribute_group *vibr_group[]
                  │               └── &activate_group,
                  │                   └── static struct attribute_group activate_group
                  │                       └── .attrs = activate_attrs,
                  │                           └── static struct attribute *activate_attrs[]
                  │                               └── &dev_attr_activate.attr,
                  │                                   └── static DEVICE_ATTR(activate, 0644, vibr_activate_show, vibr_activate_store);
                  │                                       └── vibr_activate_store
                  │                                           └── static ssize_t vibr_activate_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
                  │                                               └── vibrator_enable(dur, activate);
                  │                                                   ├── hrtimer_cancel(&g_mt_vib->vibr_timer);
                  │                                                   ├── atomic_set(&g_mt_vib->vibr_state, 1);
                  │                                                   ├── queue_work(g_mt_vib->vibr_queue, &g_mt_vib->vibr_onwork);
                  │                                                   │   └── vibr_Enable();
                  │                                                   │       └── vibr_Enable_HW();
                  │                                                   │           └── drivers/misc/mediatek/vibrator/mt6765/vibrator.c
                  │                                                   │               └── void vibr_Enable_HW(void)
                  │                                                   │                   └── pmic_set_register_value(PMIC_RG_LDO_VIBR_EN, 1);
                  │                                                   └── hrtimer_start(&g_mt_vib->vibr_timer, ktime_set(dur / 1000, (dur % 1000) * 1000000), HRTIMER_MODE_REL);
                  │                                                       └── static enum hrtimer_restart vibrator_timer_func(struct hrtimer *timer)
                  │                                                           ├── atomic_set(&vibr->vibr_state, 0);
                  │                                                           └── queue_work(vibr->vibr_queue, &vibr->vibr_offwork);
                  ├── hrtimer_init(&vibr->vibr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
                  └── vibr->vibr_timer.function = vibrator_timer_func;
                      └── vibrator_timer_func
                          └── static enum hrtimer_restart vibrator_timer_func(struct hrtimer *timer)
                              ├── atomic_set(&vibr->vibr_state, 0);
                              └── queue_work(vibr->vibr_queue, &vibr->vibr_offwork);
```

# 3 hidl

HIDL通过Binder Service，向VibratorManagerService提供控制接口

vendor/mediatek/proprietary/hardware/libvibrator/aidl/default/vibrator-mtk-default.rc
```
service vendor.vibrator-default /vendor/bin/hw/android.hardware.vibrator-service.mediatek
    class hal
    user system
    group system
```

```
* vendor/mediatek/proprietary/hardware/libvibrator/aidl/default/main.cpp
  └── int main()
      └── auto managedVib = ndk::SharedRefBase::make<Vibrator>();
          ├── auto vibManager = ndk::SharedRefBase::make<VibratorManager>(std::move(managedVib));
          │   └── vendor/mediatek/proprietary/hardware/libvibrator/aidl/default/include/vibrator-impl/VibratorManager.h
          │       └── class VibratorManager : public BnVibratorManager
          │           ├── VibratorManager(std::shared_ptr<IVibrator> vibrator) : mDefaultVibrator(std::move(vibrator)){};
          │           ├── ndk::ScopedAStatus getCapabilities(int32_t* _aidl_return) override;
          │           ├── ndk::ScopedAStatus getVibratorIds(std::vector<int32_t>* _aidl_return) override;
          │           │   └── vendor/mediatek/proprietary/hardware/libvibrator/aidl/default/VibratorManager.cpp
          │           │       └── ndk::ScopedAStatus VibratorManager::getVibratorIds(std::vector<int32_t>* _aidl_return)
          │           │           ├── LOG(INFO) << "Vibrator manager getting vibrator ids";
          │           │           └── *_aidl_return = {kDefaultVibratorId};
          │           ├── ndk::ScopedAStatus getVibrator(int32_t vibratorId, std::shared_ptr<IVibrator>* _aidl_return) override;
          │           ├── ndk::ScopedAStatus prepareSynced(const std::vector<int32_t>& vibratorIds) override;
          │           ├── ndk::ScopedAStatus triggerSynced(const std::shared_ptr<IVibratorCallback>& callback) override;
          │           └── ndk::ScopedAStatus cancelSynced() override;
          ├── status = AServiceManager_addService(vibManager->asBinder().get(), vibManagerName.c_str());
          └── ABinderProcess_joinThreadPool();
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

# 4 IVibratorManager

mNativeWrapper就是对HIDL Binder的封装，并且封装到了Java层
```
* frameworks/base/services/core/java/com/android/server/vibrator/VibratorManagerService.java
  └── public class VibratorManagerService extends IVibratorManagerService.Stub
      └── VibratorManagerService(Context context, Injector injector)
          ├── mNativeWrapper = injector.getNativeWrapper();
          │   └── static class Injector
          │       └── NativeWrapper getNativeWrapper()
          │           └── return new NativeWrapper();
          ├── mNativeWrapper.init(listener);
          │   └── static class Injector
          │       └── NativeWrapper getNativeWrapper()
          │           └── return new NativeWrapper();
          │               └── public static class NativeWrapper
          │                   └── public void init(OnSyncedVibrationCompleteListener listener)
          │                       └── mNativeServicePtr = nativeInit(listener);
          │                           └── frameworks/base/services/core/jni/com_android_server_vibrator_VibratorManagerService.cpp
          │                               └── static const JNINativeMethod method_table[]
          │                                   └── {"nativeInit", sNativeInitMethodSignature, (void*)nativeInit},
          │                                       └── std::unique_ptr<NativeVibratorManagerService> service = std::make_unique<NativeVibratorManagerService>(env, callbackListener);
          │                                           └── frameworks/base/services/core/jni/com_android_server_vibrator_VibratorManagerService.cpp
          │                                               └── NativeVibratorManagerService(JNIEnv* env, jobject callbackListener)
          │                                                   └── mHal(std::make_unique<vibrator::ManagerHalController>())
          │                                                       └── ManagerHalController() : ManagerHalController(std::make_shared<CallbackScheduler>(), &connectManagerHal) {}
          │                                                           ├── connectManagerHal
          │                                                           │   └── sp<Aidl::IVibratorManager> hal = waitForVintfService<Aidl::IVibratorManager>();
          │                                                           │       └── vendor/mediatek/proprietary/hardware/libvibrator/aidl/default/include/vibrator-impl/VibratorManager.h
          │                                                           │           └── class VibratorManager : public BnVibratorManager
          │                                                           └── ManagerHalController(std::shared_ptr<CallbackScheduler> callbackScheduler, Connector connector) : mConnector(connector), mCallbackScheduler(callbackScheduler), mConnectedHal(nullptr) {}
          ├── int[] vibratorIds = mNativeWrapper.getVibratorIds();
          │   └── public static class NativeWrapper
          │       └── public int[] getVibratorIds()
          │           └── return nativeGetVibratorIds(mNativeServicePtr);
          │               └── frameworks/base/services/core/jni/com_android_server_vibrator_VibratorManagerService.cpp
          │                   └── static const JNINativeMethod method_table[]
          │                       └── {"nativeGetVibratorIds", "(J)[I", (void*)nativeGetVibratorIds},
          │                           └── static jintArray nativeGetVibratorIds(JNIEnv* env, jclass /* clazz */, jlong servicePtr)
          │                               ├── auto result = service->hal()->getVibratorIds();
          │                               ├── jintArray ids = env->NewIntArray(vibratorIds.size());
          │                               └── return ids;
          ├── mVibrators = new SparseArray<>(mVibratorIds.length);
          └── for (int vibratorId : vibratorIds)
              └── mVibrators.put(vibratorId, injector.createVibratorController(vibratorId, listener));
```

# 5 SystemVibratorManager

VibratorManagerService的Binder Client
```
* frameworks/base/core/java/android/os/SystemVibratorManager.java
  └── public class SystemVibratorManager extends VibratorManager
      └── public SystemVibratorManager(Context context)
          └── mService = IVibratorManagerService.Stub.asInterface(ServiceManager.getService(Context.VIBRATOR_MANAGER_SERVICE));
```

# 6 VibratorManagerService

SystemServer启动服务
```
* frameworks/base/services/java/com/android/server/SystemServer.java
  └── private void startOtherServices(@NonNull TimingsTraceAndSlog t)
      ├── t.traceBegin("StartVibratorManagerService");
      ├── mSystemServiceManager.startService(VibratorManagerService.Lifecycle.class);
      │   └── VibratorManagerService.Lifecycle.class
      │       └── frameworks/base/services/core/java/com/android/server/vibrator/VibratorManagerService.java
      │           └── public static class Lifecycle extends SystemService
      │               └── public void onStart()
      │                   ├── mService = new VibratorManagerService(getContext(), new Injector());
      │                   └── publishBinderService(Context.VIBRATOR_MANAGER_SERVICE, mService);
      └── t.traceEnd();
```

vibrate接口处理
```
* frameworks/base/services/core/java/com/android/server/vibrator/VibratorManagerService.java
  └── public class VibratorManagerService extends IVibratorManagerService.Stub
      ├── VibratorManagerService(Context context, Injector injector)
      └── public void vibrate(int uid, String opPkg, @NonNull CombinedVibration effect, @Nullable VibrationAttributes attrs, String reason, IBinder token)
          ├── Vibration vib = new Vibration(token, mNextVibrationId.getAndIncrement(), effect, attrs, uid, opPkg, reason);
          │   └── frameworks/base/services/core/java/com/android/server/vibrator/Vibration.java
          │       └── Vibration(IBinder token, int id, CombinedVibration effect, VibrationAttributes attrs, int uid, String opPkg, String reason)
          └── Vibration.Status status = startVibrationLocked(vib);
              ├── VibrationThread vibThread = new VibrationThread(vib, mVibrationSettings, mDeviceVibrationEffectAdapter, mVibrators, mWakeLock, mBatteryStatsService, mVibrationCallbacks);
              │   └── frameworks/base/services/core/java/com/android/server/vibrator/VibrationThread.java
              │       ├── VibrationThread(Vibration vib, VibrationSettings vibrationSettings, DeviceVibrationEffectAdapter effectAdapter, SparseArray<VibratorController> availableVibrators, PowerManager.WakeLock wakeLock, IBatteryStats batteryStatsService, VibrationCallbacks callbacks)
              │       │   └── mVibration = vib;
              │       └── public void run()
              │           └── playVibration();
              ├── if (mCurrentVibration == null)
              │   └── return startVibrationThreadLocked(vibThread);
              │       ├── Vibration vib = vibThread.getVibration();
              │       ├── int mode = startAppOpModeLocked(vib.uid, vib.opPkg, vib.attrs);
              │       └── switch (mode)
              │           └── case AppOpsManager.MODE_ALLOWED:
              │               ├── mCurrentVibration = vibThread;
              │               └── mCurrentVibration.start();
              └── mNextVibration = vibThread;
```


# 7 vibrator测试

```sh
adb shell
cd /sys/class/leds/vibrator
echo 10000 > duration //振动多久
echo 1 > activate     //启动振动
```
