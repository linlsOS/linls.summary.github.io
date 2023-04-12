# bluetooth log analysis

android 日志分析

# 参考文档

* 过滤 BluetoothManagerService  


```
//打开蓝牙，蓝牙状态变更OFF -> BLE_TURNING_ON -> BLE_ON -> TURNING_ON -> ON
130|TB301XU:/ $ logcat -v threadtime |grep BluetoothManagerService                                        
04-12 09:01:52.882  1268  5342 D BluetoothManagerService: enable(com.android.systemui):  mBluetooth =null mBinding = false mState = OFF
04-12 09:01:52.883  1268  1368 D BluetoothManagerService: MESSAGE_ENABLE(0): mBluetooth = null
04-12 09:01:52.883  1268  1368 D BluetoothManagerService: Persisting Bluetooth Setting: 1
04-12 09:01:52.883  1268  5342 D BluetoothManagerService: enable returning
04-12 09:01:52.883  1268  1368 D BluetoothManagerService: binding Bluetooth service
04-12 09:01:53.152  1268  1268 D BluetoothManagerService: BluetoothServiceConnection: com.android.bluetooth.btservice.AdapterService
04-12 09:01:53.153  1268  1368 D BluetoothManagerService: MESSAGE_BLUETOOTH_SERVICE_CONNECTED: 1
04-12 09:01:53.159  1268  1368 D BluetoothManagerService: Broadcasting onBluetoothServiceUp() to 11 receivers.
04-12 09:01:53.170  1268  1368 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: OFF > BLE_TURNING_ON
04-12 09:01:53.170  1268  1368 D BluetoothManagerService: Sending BLE State Change: OFF > BLE_TURNING_ON
04-12 09:01:53.378  1268  1268 D BluetoothManagerService: Bluetooth Adapter name changed to Lenovo Tab M8 (4th Gen) 2023 by android
04-12 09:01:53.379  1268  1268 D BluetoothManagerService: Stored Bluetooth name: Lenovo Tab M8 (4th Gen) 2023
04-12 09:01:53.400  1268  1368 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: BLE_TURNING_ON > BLE_ON
04-12 09:01:53.400  1268  1368 D BluetoothManagerService: Bluetooth is in LE only mode
04-12 09:01:53.400  1268  1368 D BluetoothManagerService: Binding Bluetooth GATT service
04-12 09:01:53.402  1268  1368 D BluetoothManagerService: Sending BLE State Change: BLE_TURNING_ON > BLE_ON
04-12 09:01:53.404  1268  1268 D BluetoothManagerService: BluetoothServiceConnection: com.android.bluetooth.gatt.GattService
04-12 09:01:53.404  1268  1368 D BluetoothManagerService: MESSAGE_BLUETOOTH_SERVICE_CONNECTED: 2
04-12 09:01:53.404  1268  1368 D BluetoothManagerService: continueFromBleOnState()
04-12 09:01:53.407  1268  1368 D BluetoothManagerService: Persisting Bluetooth Setting: 1
04-12 09:01:53.409  1268  1368 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: BLE_ON > TURNING_ON
04-12 09:01:53.409  1268  1368 D BluetoothManagerService: Sending BLE State Change: BLE_ON > TURNING_ON
04-12 09:01:53.411  1268  1368 D BluetoothManagerService: Sending State Change: OFF > TURNING_ON
04-12 09:01:53.731  1268  1368 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: TURNING_ON > ON
04-12 09:01:53.734  1268  1368 D BluetoothManagerService: Broadcasting onBluetoothStateChange(true) to 32 receivers.
04-12 09:01:53.735  1268  1368 D BluetoothManagerService: Creating new ProfileServiceConnections object for profile: 1
04-12 09:01:53.742  1268  1368 D BluetoothManagerService: send proxy for profile: 1
04-12 09:01:53.754  1268  6085 D BluetoothManagerService: send proxy for profile: 1
04-12 09:01:53.767  1268  1368 D BluetoothManagerService: send proxy for profile: 1
04-12 09:01:53.768  1268  3001 D BluetoothManagerService: send proxy for profile: 1
04-12 09:01:53.784  1268  1368 D BluetoothManagerService: Sending BLE State Change: TURNING_ON > ON
04-12 09:01:53.784  1268  2979 D BluetoothManagerService: send proxy for profile: 1
04-12 09:01:53.785  1268  1368 D BluetoothManagerService: Sending State Change: TURNING_ON > ON
```

* 过滤 BluetoothAdapter

```
// 打开蓝牙
04-12 09:01:52.883  1694  1809 D framework_LocalBluetoothAdapter: setBluetoothEnabled success=true
04-12 09:01:53.068  7292  7292 D BluetoothAdapterService: onCreate()
04-12 09:01:53.128  7292  7292 I BluetoothAdapterService: Phone policy enabled
04-12 09:01:53.144  7292  7292 D BluetoothAdapterService: setAdapterService() - trying to set service to com.android.bluetooth.btservice.AdapterService@14cdc4f
04-12 09:01:53.152  7292  7292 D BluetoothAdapterService: onBind()
04-12 09:01:53.160  2906  2934 D BluetoothAdapter: onBluetoothServiceUp: android.bluetooth.IBluetooth$Stub$Proxy@5723f77
......
04-12 09:01:53.162  7292  7308 D BluetoothAdapter: onBluetoothServiceUp: com.android.bluetooth.btservice.AdapterService$AdapterServiceBinder@2f8f596
04-12 09:01:53.164  1874  2854 D BluetoothAdapter: onBluetoothServiceUp: android.bluetooth.IBluetooth$Stub$Proxy@8f442cc
04-12 09:01:53.165  1670  2155 D BluetoothAdapter: onBluetoothServiceUp: android.bluetooth.IBluetooth$Stub$Proxy@64641a
04-12 09:01:53.168  7292  7306 D BluetoothAdapterService: enable() - Enable called with quiet mode status =  false
04-12 09:01:53.170  7292  7311 D BluetoothAdapterService: updateAdapterState() - Broadcasting state BLE_TURNING_ON to 1 receivers.
04-12 09:01:53.173  1268  1268 D BluetoothAdapter: isLeEnabled(): BLE_TURNING_ON
04-12 09:01:53.174  7292  7311 D BluetoothAdapterService: bleOnProcessStart()
04-12 09:01:53.180  7292  7311 D BluetoothAdapterService: bleOnProcessStart() - Make Bond State Machine
04-12 09:01:53.399  7292  7311 D BluetoothAdapterService: updateAdapterState() - Broadcasting state BLE_ON to 1 receivers.
04-12 09:01:53.403  1268  1268 D BluetoothAdapter: isLeEnabled(): BLE_ON
04-12 09:01:53.408  7292  7311 D BluetoothAdapterService: updateAdapterState() - Broadcasting state TURNING_ON to 1 receivers.
04-12 09:01:53.409  7292  7311 D BluetoothAdapterService: startCoreServices()
04-12 09:01:53.413  1268  1268 D BluetoothAdapter: isLeEnabled(): TURNING_ON
04-12 09:01:53.418  1268  1268 D BluetoothAdapter: isLeEnabled(): TURNING_ON
04-12 09:01:53.722  7292  7292 D BluetoothAdapterService: updateUuids() - Updating UUIDs for bonded devices
04-12 09:01:53.725  7292  7292 I BluetoothAdapterService: initProfileServices: Initializing all bluetooth profile services
04-12 09:01:53.731  7292  7311 D BluetoothAdapterService: updateAdapterState() - Broadcasting state ON to 1 receivers.
04-12 09:01:53.732  7292  7311 D BluetoothAdapterService: Bluetooth ON,call initNative
04-12 09:01:53.732  7292  7311 D BluetoothAdapterService: Bluetooth ON,call initNative
04-12 09:01:53.767  1268  1268 D BluetoothAdapter: getActiveDevices(profile= A2DP)
04-12 09:01:53.785  1268  1268 D BluetoothAdapter: isLeEnabled(): ON
04-12 09:01:53.788  1268  1268 D BluetoothAdapter: isLeEnabled(): ON
04-12 09:01:53.791  1268  1401 D BluetoothAdapter: getActiveDevices(profile= HEADSET)
04-12 09:01:53.793  1268  1268 V NearbyService: Initiating BluetoothAdapter when Bluetooth is turned on.
04-12 09:01:53.800  7292  7292 D BluetoothAdapterService: isQuietModeEnabled() - Enabled = false
04-12 09:01:53.809  7292  7292 D BluetoothAdapter: listenUsingInsecureL2capOn: port=4131
04-12 09:01:53.814  1874  5201 D BluetoothAdapter: isLeEnabled(): ON
04-12 09:01:54.007  1874  6012 I Nearby  : [MBleClient] Start ble scanning from NearbyDirect:BluetoothAdapterWrapper, context tag = nearby_messages [CONTEXT service_id=49 ]
04-12 09:01:54.008  1874  6012 D BluetoothAdapter: isLeEnabled(): ON
04-12 09:02:00.014  1874  6012 I Nearby  : [MBleClient] Start ble scanning from NearbyDirect:BluetoothAdapterWrapper, context tag = nearby_messages [CONTEXT service_id=49 ]
04-12 09:02:00.016  1874  6012 D BluetoothAdapter: isLeEnabled(): ON
04-12 09:02:00.043  1874  6012 D BluetoothAdapter: isLeEnabled(): ON
04-12 09:02:19.378  6063  6063 D BluetoothAdapter: enable(): BT already enabled!
04-12 09:02:19.394  7292  7364 D BluetoothAdapterService: startDiscovery
04-12 09:02:19.784  6063  6063 D BluetoothAdapter: getActiveDevices(profile= A2DP)
04-12 09:02:19.788  6063  6063 D BluetoothAdapter: getActiveDevices(profile= HEADSET)
......
04-12 09:02:31.016  7292  7305 D BluetoothAdapterService: Cannot start RFCOMM listener: UUID c26cf572-3369-4cf2-b5cc-d2cd130f5b2c already in use.
04-12 09:02:49.406  7292  7305 D BluetoothAdapterService: startDiscovery
04-12 09:02:49.779  6063  6063 D BluetoothAdapter: getActiveDevices(profile= A2DP)
04-12 09:02:49.803  6063  6063 D BluetoothAdapter: getActiveDevices(profile= HEADSET)
......
//开始连接设备
04-12 09:02:54.713  7292  7305 D BluetoothAdapterService: cancelDiscovery
04-12 09:02:54.743  7292  7332 D BluetoothAdapterService: checkUpdateLeAudioAdressIfNeed index = -1
04-12 09:02:54.744  7292  7332 D BluetoothAdapterService: checkUpdateLeAudioAdressIfNeed not find dumo device,return
04-12 09:02:54.750  1694  1809 D BluetoothAdapter: getActiveDevices(profile= A2DP)
04-12 09:02:54.756  1694  1809 D BluetoothAdapter: getActiveDevices(profile= HEADSET)
04-12 09:02:56.339  7292  7305 D BluetoothAdapterService: Cannot start RFCOMM listener: UUID c26cf572-3369-4cf2-b5cc-d2cd130f5b2c already in use.
04-12 09:02:58.458  7292  7332 D BluetoothAdapterService: checkUpdateLeAudioAdressIfNeed index = -1
04-12 09:02:58.459  7292  7332 D BluetoothAdapterService: checkUpdateLeAudioAdressIfNeed not find dumo device,return
04-12 09:02:58.543  7292  7308 I BluetoothAdapterService: connectEnabledProfiles: Connecting Hid Host Profile
04-12 09:02:58.556  1694  1809 D BluetoothAdapter: getActiveDevices(profile= A2DP)
04-12 09:02:58.565  7292  7308 I BluetoothAdapterService: connectEnabledProfiles: Connecting Hid Host Profile
04-12 09:02:58.567  1694  1809 D BluetoothAdapter: getActiveDevices(profile= HEADSET)
04-12 09:02:58.599  6063  6063 D BluetoothAdapter: getActiveDevices(profile= A2DP)
04-12 09:02:58.601  6063  6063 D BluetoothAdapter: getActiveDevices(profile= HEADSET)
04-12 09:02:58.755  1694  1809 D BluetoothAdapter: getActiveDevices(profile= A2DP)
04-12 09:02:58.756  7292  7292 D BluetoothAdapterService: enable() - Enable called with quiet mode status =  false
04-12 09:02:58.758  1694  1809 D BluetoothAdapter: getActiveDevices(profile= HEADSET)
04-12 09:02:59.125  6063  6063 D BluetoothAdapter: getActiveDevices(profile= A2DP)
04-12 09:02:59.127  6063  6063 D BluetoothAdapter: getActiveDevices(profile= HEADSET)
```

* 过滤 BluetoothBondStateMachine
userdebug版本可以在蓝牙打开时设置“adb shell setprop persist.vendor.bluetooth.hostloglevel sqc”，
可以获取连接中状态变更的较多的日志。如下是打开蓝牙、设备连接、设备端开连接过程状态机变更

```
04-12 08:46:31.298  6978  6996 D BluetoothBondStateMachine: make
04-12 08:46:31.300  6978  7017 I BluetoothBondStateMachine: StableState(): Entering Off State
04-12 08:46:57.770  6978  7017 I BluetoothBondStateMachine: Entering PendingCommandState State
04-12 08:47:01.253  6978  7017 I BluetoothBondStateMachine: StableState(): Entering Off State
```

* 过滤BluetoothEventManager,获取搜索到的设备与连接状态改变的设备

```
04-12 08:44:18.980  6063  6063 D BluetoothEventManager: DeviceFoundHandler created new CachedBluetoothDevice XX:XX:XX:CC:C4:2C
04-12 08:44:22.294  1694  1809 D BluetoothEventManager: callerPackageName = com.android.systemui
04-12 08:44:22.295  1694  1809 W BluetoothEventManager: Got bonding state changed for DC:2C:26:EA:AF:78, but we have no record of that device.
```

* 过滤bt_btif_dm
```
04-12 09:24:52.466  8203  8253 I bt_btif_dm: btif_dm_cancel_discovery: Cancel search
04-12 09:24:53.070  8203  8227 I bt_btif_dm: BTIF_dm_disable: Stack device manager shutdown finished
04-12 09:24:55.036  8356  8405 I bt_btif_dm: BTIF_dm_enable: BTIF_dm_enable BLE Privacy: 1
04-12 09:25:03.867  8356  8405 I bt_btif_dm: get_cod: get_cod remote_cod = 0x00000540
04-12 09:25:03.868  8356  8405 I bt_btif_dm: check_cod_leaudio_device: check_cod_leaudio_device remote_cod = 0x00000540
04-12 09:25:03.868  8356  8405 I bt_btif_dm: check_cod_leaudio_device: check_cod_leaudio_device result 0
04-12 09:25:03.869  8356  8405 I bt_btif_dm: get_cod: get_cod remote_cod = 0x00000540
04-12 09:25:03.869  8356  8405 I bt_btif_dm: bond_state_changed: Bond state changed to state=0 [0:none, 1:bonding, 2:bonded], prev_state=0, sdp_attempts = 0
04-12 09:25:12.707  8356  8405 I bt_btif_dm: btif_dm_search_devices_evt: EIR UUIDS:
04-12 09:25:12.707  8356  8405 I bt_btif_dm: btif_dm_search_devices_evt:         0000110a-0000-1000-8000-00805f9b34fb
04-12 09:25:12.707  8356  8405 I bt_btif_dm: btif_dm_search_devices_evt:         0000110c-0000-1000-8000-00805f9b34fb
04-12 09:25:12.707  8356  8405 I bt_btif_dm: btif_dm_search_devices_evt:         0000110e-0000-1000-8000-00805f9b34fb
04-12 09:25:12.707  8356  8405 I bt_btif_dm: btif_dm_search_devices_evt:         00001200-0000-1000-8000-00805f9b34fb
04-12 09:25:13.172  8356  8405 I bt_btif_dm: btif_dm_search_devices_evt: EIR UUIDS:
04-12 09:25:13.172  8356  8405 I bt_btif_dm: btif_dm_search_devices_evt:         0000fdaa-0000-1000-8000-00805f9b34fb
04-12 09:25:17.476  8356  8405 I bt_btif_dm: btif_dm_search_devices_evt: EIR UUIDS:
04-12 09:25:17.476  8356  8405 I bt_btif_dm: btif_dm_search_devices_evt:         0000110e-0000-1000-8000-00805f9b34fb
04-12 09:25:17.476  8356  8405 I bt_btif_dm: btif_dm_search_devices_evt:         0000110c-0000-1000-8000-00805f9b34fb
04-12 09:25:17.476  8356  8405 I bt_btif_dm: btif_dm_search_devices_evt:         0000110a-0000-1000-8000-00805f9b34fb
04-12 09:25:17.476  8356  8405 I bt_btif_dm: btif_dm_search_devices_evt:         0000110b-0000-1000-8000-00805f9b34fb
04-12 09:25:17.476  8356  8405 I bt_btif_dm: btif_dm_search_devices_evt:         00001112-0000-1000-8000-00805f9b34fb
04-12 09:25:17.476  8356  8405 I bt_btif_dm: btif_dm_search_devices_evt:         00001108-0000-1000-8000-00805f9b34fb
04-12 09:25:22.694  8356  8405 I bt_btif_dm: btif_dm_cancel_discovery: Cancel search
04-12 09:25:22.706  8356  8405 I bt_btif_dm: btif_dm_cancel_discovery: Cancel search
04-12 09:25:22.708  8356  8405 I bt_btif_dm: get_cod: get_cod remote_cod = 0x00002540
04-12 09:25:22.708  8356  8405 I bt_btif_dm: get_cod: get_cod remote_cod = 0x00002540
04-12 09:25:22.708  8356  8405 I bt_btif_dm: get_cod: get_cod remote_cod = 0x00002540
// 蓝牙开始配对
04-12 09:25:22.708  8356  8405 I bt_btif_dm: bond_state_changed: Bond state changed to state=1 [0:none, 1:bonding, 2:bonded], prev_state=0, sdp_attempts = 0
04-12 09:25:25.100  8356  8405 I bt_btif_dm: btif_update_remote_properties: class of device (cod) is 0x002540
04-12 09:25:25.100  8356  8405 I bt_btif_dm: get_cod: get_cod remote_cod = 0x00002540
04-12 09:25:26.327  8356  8405 I bt_btif_dm: btif_update_remote_version_property: Remote version info valid:true [xx:xx:xx:xx:af:78]: 5, f, 2211
04-12 09:25:26.328  8356  8405 I bt_btif_dm: btif_dm_auth_cmpl_evt: Is LE actived 0
04-12 09:25:26.328  8356  8405 I bt_btif_dm: btif_dm_cancel_discovery: Cancel search
04-12 09:25:26.328  8356  8405 I bt_btif_dm: get_cod: get_cod remote_cod = 0x00002540
// 蓝牙配对完成
04-12 09:25:26.328  8356  8405 I bt_btif_dm: bond_state_changed: Bond state changed to state=2 [0:none, 1:bonding, 2:bonded], prev_state=1, sdp_attempts = 1
04-12 09:25:26.604  8356  8405 I bt_btif_dm: btif_dm_search_services_evt: New BLE UUIDs:
04-12 09:25:26.604  8356  8405 I bt_btif_dm: btif_dm_search_services_evt: index:0 uuid:00001000-0000-1000-8000-00805f9b34fb
04-12 09:25:26.604  8356  8405 I bt_btif_dm: btif_dm_search_services_evt: index:0 uuid:00001124-0000-1000-8000-00805f9b34fb
04-12 09:25:26.604  8356  8405 I bt_btif_dm: btif_dm_search_services_evt: index:0 uuid:00001200-0000-1000-8000-00805f9b34fb
04-12 09:25:26.604  8356  8405 I bt_btif_dm: btif_dm_search_services_evt: No well known BLE services discovered
04-12 09:25:26.604  8356  8405 I bt_btif_dm: btif_dm_search_services_evt: New UUIDs:
04-12 09:25:26.604  8356  8405 I bt_btif_dm: btif_dm_search_services_evt: index:0 uuid:00001124-0000-1000-8000-00805f9b34fb
04-12 09:25:26.604  8356  8405 I bt_btif_dm: btif_dm_search_services_evt: index:1 uuid:00000000-0000-1000-8000-00805f9b34fb
04-12 09:25:26.604  8356  8405 I bt_btif_dm: btif_dm_search_services_evt: index:2 uuid:00000000-0000-1000-8000-00805f9b34fb
04-12 09:25:26.604  8356  8405 I bt_btif_dm: btif_dm_search_services_evt: SDP search done for dc:2c:26:ea:af:78
04-12 09:25:26.604  8356  8405 I bt_btif_dm: check_cod_leaudio_device: check_cod_leaudio_device remote_cod = 0x00002540
04-12 09:25:26.604  8356  8405 I bt_btif_dm: check_cod_leaudio_device: check_cod_leaudio_device result 0
04-12 09:25:26.604  8356  8405 I bt_btif_dm: check_cod_leaudio_device: check_cod_leaudio_device remote_cod = 0x00002540
04-12 09:25:26.604  8356  8405 I bt_btif_dm: check_cod_leaudio_device: check_cod_leaudio_device result 0
04-12 09:25:26.927  8356  8385 I bt_btif_dm: get_cod: get_cod remote_cod = 0x00002540
```

* 过滤bt_stack_manager
```
130|TB301XU:/ $ logcat -v threadtime |grep bt_stack_manager 
// 关闭蓝牙                                                                                          
04-12 09:38:41.835  8356  8380 I bt_stack_manager: event_shut_down_stack: event_shut_down_stack is bringing down the stack
04-12 09:38:42.055  8356  8380 I bt_stack_manager: event_shut_down_stack: event_shut_down_stack finished
04-12 09:38:42.088  8356  8380 I bt_stack_manager: event_clean_up_stack: event_clean_up_stack is cleaning up the stack
04-12 09:38:42.090  8356  8380 I bt_stack_manager: event_clean_up_stack: event_clean_up_stack finished
04-12 09:38:42.091  8356  8380 I bt_stack: [INFO:message_loop_thread.cc(267)] Run: message loop finished for thread bt_stack_manager_thread
04-12 09:38:47.944  8926  8949 I droid.bluetooth: [0412/093847.944012:INFO:message_loop_thread.cc(234)] Run: message loop starting for thread bt_stack_manager_thread
//打开蓝牙
04-12 09:38:47.944  8926  8949 I bt_stack_manager: event_init_stack: is initializing the stack
04-12 09:38:47.949  8926  8949 I bt_stack_manager: event_init_stack: event_init_stack finished
04-12 09:38:48.052  8926  8949 I bt_stack_manager: event_start_up_stack: event_start_up_stack is bringing up the stack
04-12 09:38:48.163  8926  8949 I bt_stack_manager: event_start_up_stack: event_start_up_stack finished
```

* 过滤A2dpService

```
//关闭蓝牙
04-12 09:53:58.916  8926  8926 I A2dpService: stop()
04-12 09:53:58.971  8926  8926 W A2dpService: getA2dpService(): service is null
04-12 09:53:59.189  8926  8926 I A2dpService: cleanup()
04-12 09:53:59.219  8926  8958 W A2dpService: getA2dpService(): service is null
//打开蓝牙
04-12 09:54:07.996  9190  9190 I AdapterServiceConfig: init: profile=A2dpService, enabled=true
04-12 09:54:08.394  9190  9190 I A2dpService: create()
04-12 09:54:08.400  9190  9190 I A2dpService: start()
04-12 09:54:08.401  9190  9190 I BluetoothA2dpServiceJni: classInitNative: succeeds
04-12 09:54:08.402  9190  9190 I A2dpService: Max connected audio devices set to 5
//设备开始连接
04-12 09:54:53.446  9190  9242 I BluetoothA2dpServiceJni: connectA2dpNative: sBluetoothA2dpInterface: 0x6d266c46b0
04-12 09:54:53.626  9190  9218 I BluetoothA2dpServiceJni: bta2dp_mandatory_codec_preferred_callback
04-12 09:54:53.626  9190  9218 I BluetoothA2dpServiceJni: bta2dp_connection_state_callback
04-12 09:54:53.829  9190  9218 I BluetoothA2dpServiceJni: bta2dp_audio_config_callback
04-12 09:54:53.866  9190  9218 I BluetoothA2dpServiceJni: bta2dp_connection_state_callback
04-12 09:54:53.893  9190  9222 I BluetoothA2dpServiceJni: setActiveDeviceNative: sBluetoothA2dpInterface: 0x6d266c46b0
04-12 09:54:53.904  9190  9218 I BluetoothA2dpServiceJni: bta2dp_audio_config_callback
04-12 09:55:06.203  9190  9218 I BluetoothA2dpServiceJni: bta2dp_connection_state_callback
04-12 09:55:06.777  9190  9222 I BluetoothA2dpServiceJni: setActiveDeviceNative: sBluetoothA2dpInterface: 0x6d266c46b0

```