# *请确认索引缺失文件*

* [0002_bluetooth.md](0002_bluetooth.md)

# bt enable performance

测试反馈通知栏打开蓝牙422s比422t慢

# 参考文档

* [0002_bluetooth.md](0002_bluetooth.md)

# 分析

这里需要看各个时间段的耗时，关注各部分

```
422 s
    Line 5073: 04-26 17:53:58.319466  1189  6094 D BluetoothManagerService: enable(com.android.systemui):  mBluetooth =android.bluetooth.IBluetooth$Stub$Proxy@6006f46 mBinding = false mState = BLE_ON
    Line 5080: 04-26 17:53:58.333393  1189  1216 D BluetoothManagerService: Sending State Change: OFF > TURNING_ON
    Line 5244: 04-26 17:53:58.786572  1189  1216 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: TURNING_ON > ON  //467.106ms
    Line 5252: 04-26 17:53:58.822839  1189  1216 D BluetoothManagerService: Sending BLE State Change: TURNING_ON > ON
    Line 5258: 04-26 17:53:58.835114  1189  1216 D BluetoothManagerService: Sending State Change: TURNING_ON > ON         //515.678ms
    
    Line 5990: 04-26 17:54:01.803336  1189  6411 D BluetoothManagerService: enable(com.android.systemui):  mBluetooth =android.bluetooth.IBluetooth$Stub$Proxy@6006f46 mBinding = false mState = BLE_ON
    Line 6014: 04-26 17:54:01.819661  1189  1216 D BluetoothManagerService: Sending State Change: OFF > TURNING_ON
    Line 6177: 04-26 17:54:02.185880  1189  1216 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: TURNING_ON > ON //382.544
    Line 6197: 04-26 17:54:02.237006  1189  1216 D BluetoothManagerService: Sending BLE State Change: TURNING_ON > ON
    Line 6200: 04-26 17:54:02.240398  1189  1216 D BluetoothManagerService: Sending State Change: TURNING_ON > ON //437.062ms
    
    Line 8161: 04-26 17:57:05.210191  1189  1318 D BluetoothManagerService: enable(com.android.systemui):  mBluetooth =android.bluetooth.IBluetooth$Stub$Proxy@6006f46 mBinding = false mState = BLE_ON
    Line 8168: 04-26 17:57:05.215918  1189  1216 D BluetoothManagerService: Sending State Change: OFF > TURNING_ON
    Line 8357: 04-26 17:57:05.626559  1189  1216 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: TURNING_ON > ON //415.368ms
    Line 8366: 04-26 17:57:05.677982  1189  1216 D BluetoothManagerService: Sending BLE State Change: TURNING_ON > ON
    Line 8367: 04-26 17:57:05.678496  1189  1216 D BluetoothManagerService: Sending State Change: TURNING_ON > ON    //468.305ms
    
422t
    Line 7152: 04-26 17:57:06.138231   963  2509 D BluetoothManagerService: enable(com.android.systemui):  mBluetooth =android.bluetooth.IBluetooth$Stub$Proxy@7b65e1f mBinding = false mState = BLE_ON
    Line 7161: 04-26 17:57:06.149779   963  1067 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: BLE_ON > TURNING_ON
    Line 7162: 04-26 17:57:06.149835   963  1067 D BluetoothManagerService: Sending BLE State Change: BLE_ON > TURNING_ON
    Line 7164: 04-26 17:57:06.150618   963  1067 D BluetoothManagerService: Sending State Change: OFF > TURNING_ON
    Line 7584: 04-26 17:57:06.854433   963  1067 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: TURNING_ON > ON//716.202ms
    Line 8036: 04-26 17:57:07.201500   963  1067 D BluetoothManagerService: Sending BLE State Change: TURNING_ON > ON
    Line 8041: 04-26 17:57:07.210329   963  1067 D BluetoothManagerService: Sending State Change: TURNING_ON > ON   //1072.098ms
```

从日上日志可以看出，时间差距在MESSAGE_BLUETOOTH_STATE_CHANGE: TURNING_ON > ON与Sending BLE State Change耗时偏多，来看看此部分的代码堆栈

通过回调通知应上层当前的bt状态发生改变

```java
      private final IBluetoothCallback mBluetoothCallback = new IBluetoothCallback.Stub() {
          @Override
          public void onBluetoothStateChange(int prevState, int newState) throws RemoteException {
              Message msg =
                      mHandler.obtainMessage(MESSAGE_BLUETOOTH_STATE_CHANGE, prevState, newState);
              mHandler.sendMessage(msg);
          }
      };
```

bt enbale 事件传递，状态切换流程

```
           +------   Off  <-----+
           |                    |
           v                    |
    TurningBleOn   TO--->   TurningBleOff
           |                  ^ ^
           |                  | |
           +----->        ----+ |
                    BleOn       |
           +------        <---+ O
           v                  | T
       TurningOn  TO---->  TurningOff
           |                    ^
           |                    |
           +----->   On   ------+
   
```

蓝牙打开代码堆栈
```
* mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/AdapterState.java 
  └── transitionTo(mTurningOnState);
      └── public void enter() 
          └── mAdapterService.startProfileServices();
              └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/AdapterService.java
                  └── void startProfileServices()
                      └── setAllProfileServiceStates(supportedProfileServices, BluetoothAdapter.STATE_ON);
                          └── for (Class service : services) 
                              └── setProfileServiceState(service, state);
                                  └── startService(intent);
                                      └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/ProfileService.java
                                          └── public int onStartCommand(Intent intent, int flags, int startId) 
                                              └── doStart();
                                                  └── mAdapterService.onProfileServiceStateChanged(this, BluetoothAdapter.STATE_ON);
                                                      └── Message m = mHandler.obtainMessage(MESSAGE_PROFILE_SERVICE_STATE_CHANGED);
                                                          └── processProfileServiceStateChanged((ProfileService) msg.obj, msg.arg1);
                                                              ├── case BluetoothAdapter.STATE_ON:
                                                              │   ├── if (GattService.class.getSimpleName().equals(profile.getName())) 
                                                              │   │   └── enableNative();
                                                              │   └── else if (mRegisteredProfiles.size() == Config.getSupportedProfiles().length && mRegisteredProfiles.size() == mRunningProfiles.size()) // 这里需要将配置中的蓝牙服务（A2dpService、AvrcpTargetService、HeadsetService、HidDeviceService）都跑起来了，才能往下走这里耗时近600ms
                                                              │       └── mAdapterStateMachine.sendMessage(AdapterState.BREDR_STARTED);
                                                              │           └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/AdapterState.java 
                                                              │               └── transitionTo(mOnState);//这里状态切换，都是继承BaseAdapterState，每次切换都会调用public void enter()
                                                              │                   └── mAdapterService.updateAdapterState(mPrevState, currState);
                                                              │                       └── mCallbacks.getBroadcastItem(i).onBluetoothStateChange(prevState, newState);
                                                              │                           ├── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/service/java/com/android/server/bluetooth/BluetoothManagerService.java 
                                                              │                           │   └── mHandler.obtainMessage(MESSAGE_BLUETOOTH_STATE_CHANGE, prevState, newState);
                                                              │                           │       └── public void handleMessage(Message msg) 
                                                              │                           │           └── bluetoothStateChangeHandler(prevState, newState);
                                                              │                           │               ├── if (newState == BluetoothAdapter.STATE_OFF)
                                                              │                           │               │   └── sendBleStateChanged(prevState, newState);
                                                              │                           │               └── } else if (intermediate_off) {
                                                              │                           │                   ├── sendBleStateChanged(prevState, newState);
                                                              │                           │                   ├── sendBluetoothStateCallback(false);
                                                              │                           │                   └── sendBrEdrDownCallback(mContext.getAttributionSource());
                                                              │                           │                       └── synchronousOnBrEdrDown(attributionSource);
                                                              │                           │                           └── sendBrEdrDownCallback(mContext.getAttributionSource());
                                                              │                           │                               ├── if (isBleAppPresent() && !mIsUserSwitch) // Need to stay at BLE ON. Disconnect all Gatt connections 所以停留在ble_on的状态不会再切换到OFF，再次开蓝牙时也从off状态开始
                                                              │                           │                               └── else 
                                                              │                           │                                   └── synchronousOnBrEdrDown(attributionSource);
                                                              │                           │                                       └── mBluetooth.onBrEdrDown(attributionSource, recv);
                                                              │                           │                                           └── service.mAdapterStateMachine.sendMessage(AdapterState.BLE_TURN_OFF);
                                                              │                           │                                               └── transitionTo(mTurningBleOffState); 
                                                              │                           └── else if (newState == BluetoothAdapter.STATE_BLE_ON && prevState != BluetoothAdapter.STATE_OFF)   
                                                              │                               └── mAdapterStateMachine.sendMessage(AdapterState.BLE_TURN_OFF);     
                                                              └── case BluetoothAdapter.STATE_OFF:
                                                                  ├── if ((mRunningProfiles.size() == 1 && (GattService.class.getSimpleName().equals(mRunningProfiles.get(0).getName()))))
                                                                  │   └── mAdapterStateMachine.sendMessage(AdapterState.BREDR_STOPPED); 
                                                                  └── else if (mRunningProfiles.size() == 0) 
                                                                      └── disableNative();
```

为什么需要根据(mRegisteredProfiles.size() == Config.getSupportedProfiles().length && mRegisteredProfiles.size() == mRunningProfiles.size()) 来确定是否发送AdapterState.BREDR_STARTED ？

* 因为需要注册如下这些服务，只有这些服务都注册完成后，才表示蓝牙enable，如下的类都是继承ProfileService.java，这样就能说明为什么dostart 需要执行10次

```
423
05-11 19:50:25.728  1284  1284 D BluetoothManagerService: BluetoothServiceConnection: com.android.bluetooth.btservice.AdapterService
05-11 19:50:26.051  1284  1284 D BluetoothManagerService: BluetoothServiceConnection: com.android.bluetooth.gatt.GattService

	Line 17908: 05-09 14:29:01.456  9917  9917 D GattService: onStartCommand()
	Line 18362: 05-09 14:29:01.739  9917  9917 D A2dpService: onStartCommand()
	Line 18701: 05-09 14:29:01.831  9917  9917 D AvrcpTargetService: onStartCommand()
	Line 18797: 05-09 14:29:01.887  9917  9917 D HeadsetService: onStartCommand()
	Line 19026: 05-09 14:29:01.930  9917  9917 D HidDeviceService: onStartCommand()
	Line 19058: 05-09 14:29:01.945  9917  9917 D HidHostService: onStartCommand()
	Line 19098: 05-09 14:29:01.954  9917  9917 D BluetoothMapService: onStartCommand()
	Line 19149: 05-09 14:29:01.994  9917  9917 D BluetoothOppService: onStartCommand()
	Line 19256: 05-09 14:29:02.117  9917  9917 D PanService: onStartCommand()
	Line 19333: 05-09 14:29:02.135  9917  9917 D BluetoothPbapService: onStartCommand()

    doStart 执行10次的原因
422
	Line 17784: 05-11 13:42:58.448 10754 10754 D GattService: onStartCommand()
	Line 18212: 05-11 13:42:58.795 10754 10754 D HeadsetService: onStartCommand()
	Line 18472: 05-11 13:42:58.919 10754 10754 D A2dpService: onStartCommand()
	Line 18786: 05-11 13:42:58.984 10754 10754 D HidHostService: onStartCommand()
	Line 18817: 05-11 13:42:58.989 10754 10754 D PanService: onStartCommand()
	Line 18879: 05-11 13:42:58.996 10754 10754 D BluetoothMapService: onStartCommand()
	Line 18888: 05-11 13:42:59.007 10754 10754 D AvrcpTargetService: onStartCommand()
	Line 18948: 05-11 13:42:59.039 10754 10754 D SapService: onStartCommand()
	Line 18974: 05-11 13:42:59.045 10754 10754 D HidDeviceService: onStartCommand()
	Line 19032: 05-11 13:42:59.055 10754 10754 D BluetoothOppService: onStartCommand()
	Line 19045: 05-11 13:42:59.066 10754 10754 D BluetoothPbapService: onStartCommand()
```

配置bt对应服务
```
* mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/Config.java 
  * private static final ProfileConfig[] PROFILE_SERVICES_AND_FLAGS ={......}
    * new ProfileConfig(A2dpService.class, A2dpService.isEnabled(),(1 << BluetoothProfile.A2DP))// 这里进行配置支持什么协议
      * return BluetoothProperties.isProfileA2dpSourceEnabled().orElse(false);
        * mssi/system/libsysprop/srcs/android/sysprop/BluetoothProperties.sysprop 
          * api_name: "isProfileA2dpSourceEnabled"
          * prop_name: "bluetooth.profile.a2dp.source.enabled"
```

在mssi/device/mediatek/system/common/device.mk中配置host支持的服务
```
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += bluetooth.profile.a2dp.source.enabled=true
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += bluetooth.profile.avrcp.target.enabled=true
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += bluetooth.profile.gatt.enabled=true
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += bluetooth.profile.hfp.ag.enabled=true
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += bluetooth.profile.hid.device.enabled=true
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += bluetooth.profile.hid.host.enabled=true
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += bluetooth.profile.map.server.enabled=true
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += bluetooth.profile.opp.enabled=true
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += bluetooth.profile.pan.nap.enabled=true
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += bluetooth.profile.pan.panu.enabled=true
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += bluetooth.profile.pbap.server.enabled=true
```
* 所以这里需要比对对比机器的对应的服务是否有支持GattService，此部分耗时会差400ms

* 从测试抓取的日志看本地更新版本抓取的日志同测试抓取日志有部分差异：
  * 测试提供的在enable时bt的state都是ble_on,而本地复现更新版本测试都是OFF状态
    
    测试提供的日志
    ```
    enable
	Line  9433: 04-26 18:10:42.120353  1189  1318 D BluetoothManagerService: enable(com.android.settings):  mBluetooth =android.bluetooth.IBluetooth$Stub$Proxy@6006f46 mBinding = false mState = BLE_ON
	Line  9438: 04-26 18:10:42.123929  1189  1216 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: BLE_ON > TURNING_ON
	Line  9593: 04-26 18:10:42.342657  1189  1216 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: TURNING_ON > ON

    disable
	Line 10121: 04-26 18:10:46.109542  1189  2961 D BluetoothManagerService: disable(com.android.settings):  mBluetooth =android.bluetooth.IBluetooth$Stub$Proxy@6006f46 mBinding = false
	Line 10127: 04-26 18:10:46.426081  1189  1216 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: ON > TURNING_OFF
	Line 10485: 04-26 18:10:46.911424  1189  1216 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: TURNING_OFF > BLE_ON
    ```
    
    本地复现日志
    ```
    enable
    05-11 20:14:13.256  1284  2853 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: OFF > BLE_TURNING_ON
    05-11 20:14:13.526  1284  2853 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: BLE_TURNING_ON > BLE_ON
    05-11 20:14:13.539  1284  2853 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: BLE_ON > TURNING_ON
    05-11 20:14:13.976  1284  2853 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: TURNING_ON > ON

    disable
    05-11 20:09:02.551  1284  2853 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: ON > TURNING_OFF
    05-11 20:09:02.868  1284  2853 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: TURNING_OFF > BLE_ON
    05-11 20:09:02.882  1284  2853 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: BLE_ON > BLE_TURNING_OFF
    05-11 20:09:03.159  1284  2853 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: BLE_TURNING_OFF > OFF

    ```
  * 测试提供的日志并没有重新绑定GattService，没有此部分耗时 

  * 日志分析确认是否有Ble应用在跑，
  ```
  04-26 18:03:58.636339  1189  1216 D BluetoothManagerService: isBleAppPresent() count: 1
  ```

# 日志关键信息
```
BluetoothManagerService: enable
BluetoothManagerService: disable
MESSAGE_BLUETOOTH_STATE_CHANGE
Sending State Change
Sending BLE State Change:
isBleAppPresent
```

# 关键耗时部分分析

* 为什么Sending State Change: BLE_ON > TURNING_ON -> MESSAGE_BLUETOOTH_STATE_CHANGE: TURNING_ON > ON每次打开都会耗时几百ms
  
  因为这里需要去启动蓝牙的各种服务，此部分耗时是由服务启动导致，只有服务都启动了才会发送mAdapterStateMachine.sendMessage(AdapterState.BREDR_STARTED);
  进行状态切换transitionTo(mOnState)
  
```
	Line   13: 05-11 19:53:42.696137  1289  2861 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: BLE_ON > TURNING_ON
	Line   14: 05-11 19:53:42.696175  1289  2861 D BluetoothManagerService: Sending BLE State Change: BLE_ON > TURNING_ON
	Line   15: 05-11 19:53:42.697033  1289  2861 D BluetoothManagerService: Sending State Change: OFF > TURNING_ON
	
	Line   18: 05-11 19:53:42.700670 10294 10294 D A2dpService: onStartCommand()
	Line  314: 05-11 19:53:42.764293 10294 10294 D AvrcpTargetService: onStartCommand()
	Line  380: 05-11 19:53:42.787679 10294 10294 D HeadsetService: onStartCommand()
	Line  587: 05-11 19:53:42.804492 10294 10294 D HidDeviceService: onStartCommand()
	Line  613: 05-11 19:53:42.820138 10294 10294 D HidHostService: onStartCommand()
	Line  637: 05-11 19:53:42.826333 10294 10294 D BluetoothMapService: onStartCommand()
	Line  658: 05-11 19:53:42.869700 10294 10294 D BluetoothOppService: onStartCommand()
	Line  753: 05-11 19:53:42.950241 10294 10294 D PanService: onStartCommand()
	Line  820: 05-11 19:53:42.959963 10294 10294 D BluetoothPbapService: onStartCommand()
	
	Line 1045: 05-11 19:53:43.007048  1289  2861 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: TURNING_ON > ON
	Line 1056: 05-11 19:53:43.007853  1289  2861 D BluetoothManagerService: bluetoothStateChangeHandler sendBluetoothStateCallback
```

* 为什么ble都是enable的，但是蓝牙打开的耗时不同

```
通过日志分析从蓝牙打开到MESSAGE_BLUETOOTH_STATE_CHANGE: TURNING_ON > ON时耗时相近，时间差异从onBluetoothStateChange拉开可以看到422循环了22次，而422t循环了173次，需要确认为什么422t会有这么多次循环

422：
    // 505
    Line  3077: 04-26 18:09:06.451084  1189  1858 D BluetoothManagerService: enable(com.android.settings):  mBluetooth =android.bluetooth.IBluetooth$Stub$Proxy@6006f46 mBinding = false mState = BLE_ON
    Line  3082: 04-26 18:09:06.461496  1189  1216 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: BLE_ON > TURNING_ON
    Line  3083: 04-26 18:09:06.461665  1189  1216 D BluetoothManagerService: Sending BLE State Change: BLE_ON > TURNING_ON
    Line  3087: 04-26 18:09:06.468746  1189  1216 D BluetoothManagerService: Sending State Change: OFF > TURNING_ON
    Line  3271: 04-26 18:09:06.963393  1189  1216 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: TURNING_ON > ON
    Line  3272: 04-26 18:09:06.972332  1189  1216 D BluetoothManagerService: Broadcasting onBluetoothStateChange(true) to 22 receivers.
    Line  3298: 04-26 18:09:07.089381  1189  1216 D BluetoothManagerService: Sending BLE State Change: TURNING_ON > ON
    Line  3299: 04-26 18:09:07.090081  1189  1216 D BluetoothManagerService: Sending State Change: TURNING_ON > ON

422t

   //1006ms

     Line  9487: 04-26 18:06:25.226434   963  5249 D BluetoothManagerService: enable(com.android.settings):  mBluetooth =android.bluetooth.IBluetooth$Stub$Proxy@7b65e1f mBinding = false mState = BLE_ON
    Line  9506: 04-26 18:06:25.236757   963  1067 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: BLE_ON > TURNING_ON
    Line  9507: 04-26 18:06:25.237509   963  1067 D BluetoothManagerService: Sending BLE State Change: BLE_ON > TURNING_ON
    Line  9508: 04-26 18:06:25.239845   963  1067 D BluetoothManagerService: Sending State Change: OFF > TURNING_ON
    Line  9961: 04-26 18:06:25.769650   963  1067 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: TURNING_ON > ON
    Line  9973: 04-26 18:06:25.773692   963  1067 D BluetoothManagerService: Broadcasting onBluetoothStateChange(true) to 173 receivers.
    Line 10796: 04-26 18:06:26.229642   963  1067 D BluetoothManagerService: Sending BLE State Change: TURNING_ON > ON
    Line 10800: 04-26 18:06:26.232176   963  1067 D BluetoothManagerService: Sending State Change: TURNING_ON > ON

```

sendBluetoothStateCallback 源码,这里如果n 越大，则for 循环耗时越多
```java
    private void sendBluetoothStateCallback(boolean isUp) {
        try {
            int n = mStateChangeCallbacks.beginBroadcast();
            if (DBG) {
                Log.d(TAG, "Broadcasting onBluetoothStateChange(" + isUp + ") to " + n
                        + " receivers.");
            }
            for (int i = 0; i < n; i++) {
                try {
                    mStateChangeCallbacks.getBroadcastItem(i).onBluetoothStateChange(isUp);
                } catch (RemoteException e) {
                    Log.e(TAG, "Unable to call onBluetoothStateChange() on callback #" + i, e);
                }
            }
        } finally {
            mStateChangeCallbacks.finishBroadcast();
        }
    }
```

for 循环的次数的来源

* 这里返回的是 mCallbacks.size()
```
* mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/service/java/com/android/server/bluetooth/BluetoothManagerService.java 
  └── int n = mStateChangeCallbacks.beginBroadcast();
      └── private final RemoteCallbackList<IBluetoothStateChangeCallback> mStateChangeCallbacks;
          └── mssi/frameworks/base/core/java/android/os/RemoteCallbackList.java 
              └── final int N = mBroadcastCount = mCallbacks.size(); 
```

* 在如下几种对象的创建过程中都会去注册回调函数，这样mCallbacks.size 就会增加，而在对象销毁时则会unregister对应的回调
```
* BluetoothHeadset.java	405 mgr.registerStateChangeCallback(mBluetoothStateChangeCallback); in BluetoothHeadset()
  └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/service/java/com/android/server/bluetooth/BluetoothManagerService.java
      └── public void registerStateChangeCallback(IBluetoothStateChangeCallback callback)
          └── Message msg = mHandler.obtainMessage(MESSAGE_REGISTER_STATE_CHANGE_CALLBACK);
              └── mStateChangeCallbacks.register(callback);
```

# 日志路径

* [0025_bt_enable_423log.txt](refers/0025_bt_enable_423log.txt)

* 分析

从日志查看enable的大的耗时主要是在TURNING_ON > ON状态的切换，而这里的耗时的大头是在A2dpService与BluetoothOppService。
```
	Line 6535: 05-30 03:58:36.611  1038  3113 D BluetoothManagerService: enable(com.android.systemui):  mBluetooth =android.bluetooth.IBluetooth$Stub$Proxy@dab56b2 mBinding = false mState = BLE_ON
	Line 6536: 05-30 03:58:36.611  1038  2576 D BluetoothManagerService: MESSAGE_ENABLE(0): mBluetooth = android.bluetooth.IBluetooth$Stub$Proxy@dab56b2
	Line 6537: 05-30 03:58:36.611  1038  2576 D BluetoothManagerService: Persisting Bluetooth Setting: 1
	Line 6538: 05-30 03:58:36.611  1038  3113 D BluetoothManagerService: enable returning
	Line 6540: 05-30 03:58:36.613  1038  2576 W BluetoothManagerService: BT Enable in BLE_ON State, going to ON
	Line 6545: 05-30 03:58:36.616  1038  2576 D BluetoothManagerService: Persisting Bluetooth Setting: 1
	Line 6547: 05-30 03:58:36.617  1038  2576 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: BLE_ON > TURNING_ON
	Line 6548: 05-30 03:58:36.617  1038  2576 D BluetoothManagerService: Sending BLE State Change: BLE_ON > TURNING_ON
	Line 6552: 05-30 03:58:36.623  1038  2576 D BluetoothManagerService: Sending State Change: OFF > TURNING_ON
	Line 6551: 05-30 03:58:36.623  8729  8729 D A2dpService: onStartCommand()
	Line 6553: 05-30 03:58:36.624  8729  8729 D ProfileService: doStart start
	Line 6891: 05-30 03:58:36.746  8729  8729 D ProfileService: doStart end
	Line 6900: 05-30 03:58:36.751  8729  8729 D AvrcpTargetService: onStartCommand()
	Line 6901: 05-30 03:58:36.752  8729  8729 D ProfileService: doStart start
	Line 6960: 05-30 03:58:36.778  8729  8729 D ProfileService: doStart end
	Line 6968: 05-30 03:58:36.780  8729  8729 D HeadsetService: onStartCommand()
	Line 6969: 05-30 03:58:36.781  8729  8729 D ProfileService: doStart start
	Line 7173: 05-30 03:58:36.795  8729  8729 D ProfileService: doStart end
	Line 7179: 05-30 03:58:36.796  8729  8729 D HidDeviceService: onStartCommand()
	Line 7183: 05-30 03:58:36.797  8729  8729 D ProfileService: doStart start
	Line 7210: 05-30 03:58:36.805  8729  8729 D ProfileService: doStart end
	Line 7213: 05-30 03:58:36.806  8729  8729 D HidHostService: onStartCommand()
	Line 7214: 05-30 03:58:36.807  8729  8729 D ProfileService: doStart start
	Line 7232: 05-30 03:58:36.810  8729  8729 D ProfileService: doStart end
	Line 7243: 05-30 03:58:36.814  8729  8729 D BluetoothMapService: onStartCommand()
	Line 7244: 05-30 03:58:36.815  8729  8729 D ProfileService: doStart start
	Line 7273: 05-30 03:58:36.847  8729  8729 D ProfileService: doStart end
	Line 7281: 05-30 03:58:36.853  8729  8729 D BluetoothOppService: onStartCommand()
	Line 7282: 05-30 03:58:36.854  8729  8729 D ProfileService: doStart start
	Line 7326: 05-30 03:58:36.928  8729  8729 D ProfileService: doStart end
	Line 7337: 05-30 03:58:36.931  8729  8729 D PanService: onStartCommand()
	Line 7338: 05-30 03:58:36.932  8729  8729 D ProfileService: doStart start
	Line 7403: 05-30 03:58:36.939  8729  8729 D ProfileService: doStart end
	Line 7420: 05-30 03:58:36.946  8729  8729 D BluetoothPbapService: onStartCommand()
	Line 7421: 05-30 03:58:36.947  8729  8729 D ProfileService: doStart start
	Line 7435: 05-30 03:58:36.966  8729  8729 D ProfileService: doStart end
	Line 7656: 05-30 03:58:36.998  1038  2576 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: TURNING_ON > ON //375ms
	Line 7669: 05-30 03:58:36.999  1038  2576 D BluetoothManagerService: Broadcasting onBluetoothStateChange(true) to 25 receivers.
	Line 7674: 05-30 03:58:37.000  1038  4165 D BluetoothManagerService: Creating new ProfileServiceConnections object for profile: 1
	Line 7690: 05-30 03:58:37.008  1038  4165 D BluetoothManagerService: send proxy for profile: 1
	Line 7702: 05-30 03:58:37.014  1038  2576 D BluetoothManagerService: send proxy for profile: 1
	Line 7705: 05-30 03:58:37.014  1038  2576 D BluetoothManagerService: send proxy for profile: 1
	Line 7707: 05-30 03:58:37.015  1038  4165 D BluetoothManagerService: send proxy for profile: 1
	Line 7719: 05-30 03:58:37.027  1038  5037 D BluetoothManagerService: send proxy for profile: 1
	Line 7733: 05-30 03:58:37.043  1038  2576 D BluetoothManagerService: Sending BLE State Change: TURNING_ON > ON
	Line 7738: 05-30 03:58:37.044  1038  2576 D BluetoothManagerService: Sending State Change: TURNING_ON > ON
```

* A2dpService 耗时分析
```
05-30 03:58:36.624  8729  8729 D ProfileService: doStart start
05-30 03:58:36.624   682   682 I BufferQueueProducer: [NotificationShade#196](this:0xb4000075ccdc1078,id:-1,api:0,p:-1,c:682) queueBuffer: fps=31.21 dur=1698.07 max=817.28 min=8.83
05-30 03:58:36.624  8729  8729 D ProfileService: doStart addProfile
05-30 03:58:36.624  8729  8729 D ProfileService: doStart registerReceiver
......
05-30 03:58:36.651  8729  8729 D ProfileService: doStart getCurrentUser //这里对应耗时27ms
05-30 03:58:36.651  8729  8729 I A2dpService: start()
05-30 03:58:36.651  8729  8729 I A2dpService: Max connected audio devices set to 5

05-30 03:32:41.565  7478  7478 D A2dpNativeInterface: init start
05-30 03:32:41.603  7478  7478 D A2dpNativeInterface: init end// 这里有将近40ms的耗时
（1）27ms 耗时对应ProfileService.java 中 getApplicationContext().registerReceiver(mUserSwitchedReceiver, filter);
    此部分耗时需要系统确认，
 (2)35ms 耗时只有是native 部分耗时
```
A2dpService native 耗时代码堆栈
```
* mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/a2dp/A2dpNativeInterface.java 
  └── initNative(maxConnectedAudioDevices, codecConfigPriorities, codecConfigOffloading);
      └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/jni/com_android_bluetooth_a2dp.cpp
          └── static void initNative(JNIEnv* env, jobject object,jint maxConnectedAudioDevices, jobjectArray codecConfigArray,jobjectArray codecOffloadingArray) 
              └── bt_status_t status = sBluetoothA2dpInterface->init( &sBluetoothA2dpCallbacks, maxConnectedAudioDevices, codec_priorities,codec_offloading);
                  └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/btif/src/btif_av.cc 
                      └── bt_status_t BtifAvSource::Init(
                          └── bta_av_co_init(codec_priorities);
                              └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/btif/co/bta_av_co.cc
                                  └── bta_av_co_cb.Init(codec_priorities);
                                      └── for (size_t i = 0; i < BTA_AV_CO_NUM_ELEMENTS(peers_); i++)
                                          └── p_peer->Init(codec_priorities);
                                              ├── codecs_ = new A2dpCodecs(codec_priorities);
                                              │   └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/stack/a2dp/a2dp_codec_config.cc 
                                              │       └── A2dpCodecs::A2dpCodecs(const std::vector<btav_a2dp_codec_config_t>& codec_priorities)
                                              │           └── for (auto config : codec_priorities) 
                                              │               └── codec_priorities_.insert(std::make_pair(config.codec_type, config.codec_priority));
                                              ├── codecs_->init();
                                              │   └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/stack/a2dp/a2dp_codec_config.cc 
                                              │       └── A2dpCodecConfig* codec_config = A2dpCodecConfig::createCodec(codec_index, codec_priority);
                                              │           └── switch (codec_index)
                                              │               ├── case BTAV_A2DP_CODEC_INDEX_SOURCE_SBC:
                                              │               │   └── codec_config = new A2dpCodecConfigSbcSource(codec_priority);
                                              │               └── case BTAV_A2DP_CODEC_INDEX_SINK_SBC:
                                              │                   └── codec_config = new A2dpCodecConfigSbcSink(codec_priority);
                                              └── A2DP_InitDefaultCodec(codec_config);
```
