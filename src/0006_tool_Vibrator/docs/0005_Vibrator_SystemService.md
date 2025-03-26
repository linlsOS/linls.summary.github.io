# Vibrator SystemService

Vibrator SystemService是是如何启动运行

# VIBRATOR_SERVICE register

```java
// frameworks/base/core/java/android/app/SystemServiceRegistry.java

registerService(Context.VIBRATOR_MANAGER_SERVICE, VibratorManager.class,
        new CachedServiceFetcher<VibratorManager>() {
            @Override
            public VibratorManager createService(ContextImpl ctx) {
                return new SystemVibratorManager(ctx);
            }});

registerService(Context.VIBRATOR_SERVICE, Vibrator.class,
        new CachedServiceFetcher<Vibrator>() {
    @Override
    public Vibrator createService(ContextImpl ctx) {
        return new SystemVibrator(ctx);
    }});
```

# SystemVibrator

SystemVibrator通过getSystemService()获取到了SystemVibratorManager，本质是VibratorManagerService的Binder Client

```java
// frameworks/base/core/java/android/os/SystemVibrator.java

@UnsupportedAppUsage
public SystemVibrator(Context context) {
    super(context);
    mContext = context;
    mVibratorManager = mContext.getSystemService(VibratorManager.class);
}
```

# VibratorManagerService aidl

frameworks/base/core/java/android/os/IVibratorManagerService.aidl
```java
interface IVibratorManagerService {
    int[] getVibratorIds();
    VibratorInfo getVibratorInfo(int vibratorId);
    boolean isVibrating(int vibratorId);
    boolean registerVibratorStateListener(int vibratorId, in IVibratorStateListener listener);
    boolean unregisterVibratorStateListener(int vibratorId, in IVibratorStateListener listener);
    boolean setAlwaysOnEffect(int uid, String opPkg, int alwaysOnId,
            in CombinedVibration vibration, in VibrationAttributes attributes);
    void vibrate(int uid, String opPkg, in CombinedVibration vibration,
            in VibrationAttributes attributes, String reason, IBinder token);
    void cancelVibrate(int usageFilter, IBinder token);
}
```

# SystemVibratorManager

VibratorManagerService的Binder Client
```
* frameworks/base/core/java/android/os/SystemVibratorManager.java
  └── public class SystemVibratorManager extends VibratorManager
      └── public SystemVibratorManager(Context context)
          └── mService = IVibratorManagerService.Stub.asInterface(ServiceManager.getService(Context.VIBRATOR_MANAGER_SERVICE));
```

# VibratorManagerService

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