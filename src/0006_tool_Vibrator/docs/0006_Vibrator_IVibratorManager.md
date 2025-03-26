# Vibrator IVibratorManager

VibratorManagerService如何与HIDL通信？

# IVibratorManager.aidl

hardware/interfaces/vibrator/aidl/aidl_api/android.hardware.vibrator/current/android/hardware/vibrator/IVibratorManager.aidl
```java
package android.hardware.vibrator;
@VintfStability
interface IVibratorManager {
  int getCapabilities();
  int[] getVibratorIds();
  android.hardware.vibrator.IVibrator getVibrator(in int vibratorId);
  void prepareSynced(in int[] vibratorIds);
  void triggerSynced(in android.hardware.vibrator.IVibratorCallback callback);
  void cancelSynced();
  const int CAP_SYNC = 1;
  const int CAP_PREPARE_ON = 2;
  const int CAP_PREPARE_PERFORM = 4;
  const int CAP_PREPARE_COMPOSE = 8;
  const int CAP_MIXED_TRIGGER_ON = 16;
  const int CAP_MIXED_TRIGGER_PERFORM = 32;
  const int CAP_MIXED_TRIGGER_COMPOSE = 64;
  const int CAP_TRIGGER_CALLBACK = 128;
}
```

# HIDL

HIDL通过Binder Service，向VibratorManagerService提供控制接口

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

# IVibratorManager

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
