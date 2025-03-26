# Android Bluetooth Enable

注意log在sys log中而不是在main log中，一度怀疑人生，log出不来

# 参考文档

* [Android 9.0 Bluetooth源码分析（一）蓝牙开启流程](https://www.jianshu.com/p/a150d55e29ca)

# enable流程

```
* frameworks/base/core/java/android/bluetooth/BluetoothAdapter.java
  └── public boolean enable()
      └── return mManagerService.enable(ActivityThread.currentPackageName());
          └── frameworks/base/services/core/java/com/android/server/BluetoothManagerService.java
              └── public boolean enable(String packageName)
                  └── sendEnableMsg(false, BluetoothProtoEnums.ENABLE_DISABLE_REASON_APPLICATION_REQUEST, packageName);
                      └── mHandler.sendMessage(mHandler.obtainMessage(MESSAGE_ENABLE, quietMode ? 1 : 0, 0));
                          └── private class BluetoothHandler extends Handler
                              └── public void handleMessage(Message msg)
                                  └── case MESSAGE_ENABLE:
                                      └── handleEnable(mQuietEnable
                                          └── mBluetooth.enable(mQuietEnable)
                                              └── packages/apps/Bluetooth/src/com/android/bluetooth/btservice/AdapterService.java
                                                  └── public boolean enable(boolean quietMode)
                                                      ├── AdapterService service = getService();
                                                      └── return service.enable(quietMode);

```

# enable过程main log信息

```
09-11 14:05:51.884405  1138  1834 E BluetoothManagerService: enable(com.android.systemui):  mBluetooth =null mBinding = false mState = OFF
09-11 14:05:51.884665  1138  1159 D BluetoothManagerService: MESSAGE_ENABLE(0): mBluetooth = null
09-11 14:05:51.884752  1138  1159 D BluetoothManagerService: binding Bluetooth service
09-11 14:05:51.884780  1138  1834 E BluetoothManagerService: enable returning
09-11 14:05:52.092065  1138  1138 D BluetoothManagerService: Bluetooth Adapter name changed to M50_CDEF
09-11 14:05:52.092726  1138  1138 D BluetoothManagerService: Stored Bluetooth name: M50_CDEF
09-11 14:05:52.122720  1138  1138 D BluetoothManagerService: BluetoothServiceConnection: com.android.bluetooth.btservice.AdapterService
09-11 14:05:52.122992  1138  1159 D BluetoothManagerService: MESSAGE_BLUETOOTH_SERVICE_CONNECTED: 1
09-11 14:05:52.124955  1138  1159 D BluetoothManagerService: Broadcasting onBluetoothServiceUp() to 4 receivers.
09-11 14:05:52.131234  1138  1159 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: OFF > BLE_TURNING_ON
09-11 14:05:52.131408  1138  1159 D BluetoothManagerService: Sending BLE State Change: OFF > BLE_TURNING_ON
09-11 14:05:52.279721  1138  1138 D BluetoothManagerService: Bluetooth Adapter address changed to 7E:41:46:B5:B6:72
09-11 14:05:52.280294  1138  1138 D BluetoothManagerService: Stored Bluetoothaddress: 7E:41:46:B5:B6:72
09-11 14:05:52.280957  1138  1138 D BluetoothManagerService: Bluetooth Adapter name changed to M50_CDEF
09-11 14:05:52.281403  1138  1138 D BluetoothManagerService: Stored Bluetooth name: M50_CDEF
09-11 14:05:52.291180  1138  1159 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: BLE_TURNING_ON > BLE_ON
09-11 14:05:52.291305  1138  1159 D BluetoothManagerService: Bluetooth is in LE only mode
09-11 14:05:52.291382  1138  1159 D BluetoothManagerService: Binding Bluetooth GATT service
09-11 14:05:52.294517  1138  1159 D BluetoothManagerService: Sending BLE State Change: BLE_TURNING_ON > BLE_ON
09-11 14:05:52.296095  1138  1138 D BluetoothManagerService: BluetoothServiceConnection: com.android.bluetooth.gatt.GattService
09-11 14:05:52.296324  1138  1159 D BluetoothManagerService: MESSAGE_BLUETOOTH_SERVICE_CONNECTED: 2
09-11 14:05:52.296416  1138  1159 D BluetoothManagerService: continueFromBleOnState()
09-11 14:05:52.297168  1138  1159 D BluetoothManagerService: isBleAppPresent() count: 0
09-11 14:05:52.298143  1138  1159 D BluetoothManagerService: Persisting Bluetooth Setting: 1
09-11 14:05:52.302650  1138  1159 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: BLE_ON > TURNING_ON
09-11 14:05:52.303261  1138  1159 D BluetoothManagerService: Sending BLE State Change: BLE_ON > TURNING_ON
09-11 14:05:52.552200  1138  1159 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: TURNING_ON > ON
09-11 14:05:52.552880  1138  1159 D BluetoothManagerService: Broadcasting onBluetoothStateChange(true) to 15 receivers.
09-11 14:05:52.553785  1138  1159 D BluetoothManagerService: Creating new ProfileServiceConnections object for profile: 1
09-11 14:05:52.576061  1138  1159 D BluetoothManagerService: Sending BLE State Change: TURNING_ON > ON
```

# disable过程main log信息

```
09-11 14:05:49.810409  1138  1206 E BluetoothManagerService: disable(com.android.systemui):  mBluetooth =android.bluetooth.IBluetooth$Stub$Proxy@784ed5 mBinding = false
09-11 14:05:49.810998  1138  1206 D BluetoothManagerService: Bluetooth persisted state: 1
09-11 14:05:49.811143  1138  1206 D BluetoothManagerService: Persisting Bluetooth Setting: 0
09-11 14:05:49.812774  1138  1206 E BluetoothManagerService: disable returning
09-11 14:05:49.814553  1138  1159 D BluetoothManagerService: MESSAGE_DISABLE: mBluetooth = android.bluetooth.IBluetooth$Stub$Proxy@784ed5, mBinding = false
09-11 14:05:50.114928  1138  1159 D BluetoothManagerService: MESSAGE_HANDLE_DISABLE_DELAYED: disabling:false
09-11 14:05:50.115257  1138  1159 D BluetoothManagerService: isBleAppPresent() count: 0
09-11 14:05:50.115352  1138  1159 D BluetoothManagerService: Sending off request.
09-11 14:05:50.120832  1138  1159 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: ON > TURNING_OFF
09-11 14:05:50.120951  1138  1159 D BluetoothManagerService: Sending BLE State Change: ON > TURNING_OFF
09-11 14:05:50.240431  1138  1159 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: TURNING_OFF > BLE_ON
09-11 14:05:50.240620  1138  1159 D BluetoothManagerService: Intermediate off, back to LE only mode
09-11 14:05:50.240659  1138  1159 D BluetoothManagerService: Sending BLE State Change: TURNING_OFF > BLE_ON
09-11 14:05:50.241766  1138  1159 D BluetoothManagerService: Broadcasting onBluetoothStateChange(false) to 15 receivers.
09-11 14:05:50.243315  1138  1159 D BluetoothManagerService: Calling sendBrEdrDownCallback callbacks
09-11 14:05:50.243522  1138  1159 D BluetoothManagerService: isBleAppPresent() count: 0
09-11 14:05:50.243648  1138  1595 E BluetoothManagerService: No proxy, unbind
09-11 14:05:50.248583  1138  1159 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: BLE_ON > BLE_TURNING_OFF
09-11 14:05:50.248754  1138  1159 D BluetoothManagerService: Sending BLE State Change: BLE_ON > BLE_TURNING_OFF
09-11 14:05:50.418951  1138  1159 D BluetoothManagerService: MESSAGE_HANDLE_DISABLE_DELAYED: disabling:true
09-11 14:05:50.419795  1138  1159 D BluetoothManagerService: Handle disable is finished
09-11 14:05:50.484786  1138  1159 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: BLE_TURNING_OFF > OFF
09-11 14:05:50.484981  1138  1159 D BluetoothManagerService: Bluetooth is complete send Service Down
09-11 14:05:50.485192  1138  1159 D BluetoothManagerService: Broadcasting onBluetoothServiceDown() to 4 receivers.
09-11 14:05:50.488094  1138  1159 D BluetoothManagerService: unbindAndFinish(): android.bluetooth.IBluetooth$Stub$Proxy@784ed5 mBinding = false mUnbinding = false
09-11 14:05:50.498945  1138  1159 D BluetoothManagerService: Sending BLE State Change: BLE_TURNING_OFF > OFF
```

# disable过程main log输出信息

注意main log不包含BluetoothManagerService的log，其存在于sys log中，搞了好久才发现

```
09-11 14:06:05.006702  1385  1630 E BluetoothAdapter: mManagerService.disalbe()
09-11 14:06:05.310439  3366  3387 D BluetoothAdapterService: disable() called with mRunningProfiles.size() = 11
09-11 14:06:05.311923  3366  3389 D BluetoothAdapterService: updateAdapterState() - Broadcasting state TURNING_OFF to 1 receivers.
09-11 14:06:05.321660  3366  3366 I BluetoothHeadsetServiceJni: cleanupNative: Cleaning up Bluetooth Handsfree Interface
09-11 14:06:05.321936  3366  3366 I BluetoothHeadsetServiceJni: cleanupNative: Cleaning up Bluetooth Handsfree callback object
09-11 14:06:05.329985  1138  1138 D BluetoothHeadset: Proxy object disconnected
09-11 14:06:05.330370  1138  1138 D BluetoothHeadset: Proxy object disconnected
09-11 14:06:05.330385  1687  1958 D BluetoothHeadset: Proxy object disconnected
09-11 14:06:05.330499  1385  1419 D BluetoothHeadset: Proxy object disconnected
09-11 14:06:05.330523  1138  1138 D BluetoothHeadset: Proxy object disconnected
09-11 14:06:05.335893  1138  1138 D BluetoothA2dp: Proxy object disconnected
09-11 14:06:05.336018  1138  1138 D BluetoothA2dp: Unbinding service...
09-11 14:06:05.338916  1138  1138 D BluetoothA2dp: Proxy object disconnected
09-11 14:06:05.339357  1138  1138 D BluetoothA2dp: Unbinding service...
09-11 14:06:05.341064  3366  3366 D BluetoothSdpJni: sdpRemoveSdpRecordNative
09-11 14:06:05.341402  3366  3398 D BluetoothActiveDeviceManager: handleMessage(MESSAGE_ADAPTER_ACTION_STATE_CHANGED): newState=13
09-11 14:06:05.341464  3366  3366 D BluetoothSdpJni: SDP Remove record success - handle: 4
09-11 14:06:05.343199  1138  1138 D BluetoothA2dp: Proxy object disconnected
09-11 14:06:05.343562  1138  1138 D BluetoothA2dp: Unbinding service...
09-11 14:06:05.352680  1385  1385 D BluetoothA2dp: Proxy object disconnected
09-11 14:06:05.352836  1385  1385 D BluetoothA2dp: Unbinding service...
09-11 14:06:05.356029  3366  3366 D BluetoothMapService: stop()
09-11 14:06:05.356379  3366  3366 D BluetoothMapService: setBluetoothMapService(): set to: null
09-11 14:06:05.357342  3366  3366 D BluetoothMapAppObserver: deinitObservers()
09-11 14:06:05.357701  3366  3366 D BluetoothMapAppObserver: removeReceiver()
09-11 14:06:05.358598  3366  3366 E BluetoothMapService: mSessionStatusHandler.sendMessage() dispatched shutdown message
09-11 14:06:05.358856  3366  3424 D BluetoothMapService: closeService() in
09-11 14:06:05.358957  3366  3424 D BluetoothMapMasInstance0: MAP Service shutdown
09-11 14:06:05.359023  3366  3424 D BluetoothSdpJni: sdpRemoveSdpRecordNative
09-11 14:06:05.359116  3366  3424 D BluetoothSdpJni: SDP Remove record success - handle: 0
09-11 14:06:05.359234  3366  3424 D BluetoothMapMasInstance0: RemoveSDPrecord returns true
09-11 14:06:05.359821  3366  3424 D BluetoothSocket: close() this: android.bluetooth.BluetoothSocket@5cdfeba, channel: 26, mSocketIS: android.net.LocalSocketImpl$SocketInputStream@a2e516b, mSocketOS: android.net.LocalSocketImpl$SocketOutputStream@64974c8mSocket: android.net.LocalSocket@98bd961 impl:android.net.LocalSocketImpl@ecd4c86 fd:java.io.FileDescriptor@5e9947, mSocketState: LISTENING
09-11 14:06:05.361074  3366  3424 D BluetoothSocket: close() this: android.bluetooth.BluetoothSocket@ee51574, channel: 4137, mSocketIS: android.net.LocalSocketImpl$SocketInputStream@fcbaa9d, mSocketOS: android.net.LocalSocketImpl$SocketOutputStream@a21ab12mSocket: android.net.LocalSocket@46182e3 impl:android.net.LocalSocketImpl@1f814e0 fd:java.io.FileDescriptor@23d5399, mSocketState: LISTENING
09-11 14:06:05.363037  1385  1385 D BluetoothHidHost: Proxy object disconnected
09-11 14:06:05.363541  1385  1385 D BluetoothHidHost: Unbinding service...
09-11 14:06:05.365197  1385  1385 D BluetoothPan: Proxy object disconnected
09-11 14:06:05.365359  1385  1385 D BluetoothPan: Unbinding service...
09-11 14:06:05.367876  1385  1385 D BluetoothMap: Proxy object disconnected
09-11 14:06:05.368006  1385  1385 D BluetoothMap: Unbinding service...
09-11 14:06:05.373734  3366  3366 V BluetoothHidDeviceServiceJni: cleanupNative enter
09-11 14:06:05.374102  3366  3366 I BluetoothHidDeviceServiceJni: Cleaning up interface
09-11 14:06:05.374260  3366  3366 I BluetoothHidDeviceServiceJni: Cleaning up callback object
```
