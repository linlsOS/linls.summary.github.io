# android bt 代码栈

 android bt 代码栈

# 参考文档

* [Android 蓝牙启动流程(以及设置蓝牙为作为sink模式 & 接收端模式)](https://blog.csdn.net/ChaoLi_Chen/article/details/108285847)
* [system/bt目录内容解析](https://blog.csdn.net/weixin_40537714/article/details/121861858)

# 蓝牙的总体流程

![0002_bluetooth_flow.png](images/0002_bluetooth_flow.png)

# enable bluetooth
```
* packages/modules/Bluetooth/framework/java/android/bluetooth/BluetoothAdapter.java 
  └── public boolean enable()
      └── return mManagerService.enable(mAttributionSource);
          └── packages/modules/Bluetooth/service/java/com/android/server/bluetooth/BluetoothManagerService.java
              └── sendEnableMsg(false, BluetoothProtoEnums.ENABLE_DISABLE_REASON_APPLICATION_REQUEST, packageName); 
                  └── mHandler.sendMessage(mHandler.obtainMessage(MESSAGE_ENABLE, quietMode ? 1 : 0, 0));
                      ├── handleEnable(mQuietEnable);
                      │   └── if (!doBind(i, mConnection, Context.BIND_AUTO_CREATE | Context.BIND_IMPORTANT, UserHandle.CURRENT)) 
                      │       └── public void onServiceConnected(ComponentName componentName, IBinder service)
                      │           └── Message msg = mHandler.obtainMessage(MESSAGE_BLUETOOTH_SERVICE_CONNECTED);
                      │               ├── mBluetooth = IBluetooth.Stub.asInterface(Binder.allowBlocking(service));//获取的是 IBluetooth 对象, 而AdapterService.java 内部类 AdapterServiceBinder  实现了 IBluetooth 
                      │               ├── mBluetooth.registerCallback(mBluetoothCallback, mContext.getAttributionSource()); 
                      │               └── if (!mBluetooth.enable(mQuietEnable, mContext.getAttributionSource()))
                      │                   └── packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/AdapterService.java
                      │                       └── service.enable(quietMode)
                      │                           └── mAdapterStateMachine.sendMessage(AdapterState.BLE_TURN_ON);
                      │                               └── transitionTo(mTurningBleOnState);
                      │                                   ├── sendMessageDelayed(BREDR_START_TIMEOUT, BREDR_START_TIMEOUT_DELAY);
                      │                                   └── mAdapterService.startProfileServices();
                      │                                       ├── Class[] supportedProfileServices = Config.getSupportedProfiles();
                      │                                       └── setAllProfileServiceStates(supportedProfileServices, BluetoothAdapter.STATE_ON); 
                      │                                           └── setProfileServiceState(service, state);
                      │                                               ├── intent.putExtra(EXTRA_ACTION, ACTION_SERVICE_STATE_CHANGED);
                      │                                               ├── intent.putExtra(BluetoothAdapter.EXTRA_STATE, state);
                      │                                               └── startService(intent);
                      │                                                   └── packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/ProfileService.java 
                      │                                                       └── public int onStartCommand(Intent intent, int flags, int startId)
                      │                                                           ├── if (state == BluetoothAdapter.STATE_OFF)
                      │                                                           │   └── doStop(); 
                      │                                                           └── else if (state == BluetoothAdapter.STATE_ON)
                      │                                                               └── doStart();
                      │                                                                   └── mAdapterService.onProfileServiceStateChanged(this, BluetoothAdapter.STATE_ON);
                      │                                                                       ├── Message m = mHandler.obtainMessage(MESSAGE_PROFILE_SERVICE_STATE_CHANGED);
                      │                                                                       └── mHandler.sendMessage(m);
                      │                                                                           └── processProfileServiceStateChanged((ProfileService) msg.obj, msg.arg1);
                      │                                                                               └── enableNative();
                      │                                                                                   └── packages/modules/Bluetooth/android/app/jni/com_android_bluetooth_btservice_AdapterService.cpp 
                      │                                                                                       └── int ret = sBluetoothInterface->enable();
                      │                                                                                           └── packages/modules/Bluetooth/system/btif/src/bluetooth.cc
                      │                                                                                               └── stack_manager_get_interface()->start_up_stack_async();
                      │                                                                                                   └── packages/modules/Bluetooth/system/btif/src/stack_manager.cc
                      │                                                                                                       └── management_thread.DoInThread(FROM_HERE, base::Bind(event_start_up_stack, nullptr)); 
                      │                                                                                                           └── static void event_start_up_stack(UNUSED_ATTR void* context)
                      │                                                                                                               ├── ensure_stack_is_initialized();
                      │                                                                                                               │   └── static void event_init_stack(void* context) 
                      │                                                                                                               │       ├── btif_init_bluetooth();
                      │                                                                                                               │       └── bte_main_init();
                      │                                                                                                               │           ├── hci = bluetooth::shim::hci_layer_get_interface()
                      │                                                                                                               │           └── hci->set_data_cb(base::Bind(&post_to_main_message_loop));
                      │                                                                                                               ├── module_start_up(get_local_module(BTIF_CONFIG_MODULE));
                      │                                                                                                               ├── BTA_dm_init()  //搜索事件回调
                      │                                                                                                               │   └── bta_sys_register(BTA_ID_DM_SEARCH, &bta_dm_search_reg);
                      │                                                                                                               │       └── static const tBTA_SYS_REG bta_dm_search_reg = {bta_dm_search_sm_execute, bta_dm_search_sm_disable};
                      │                                                                                                               │           └── bool bta_dm_search_sm_execute(BT_HDR_RIGID* p_msg) 
                      │                                                                                                               │               └── void bta_dm_search_start(tBTA_DM_MSG* p_data) 
                      │                                                                                                               │                   └── result.status = BTM_StartInquiry(bta_dm_inq_results_cb, bta_dm_inq_cmpl_cb);
                      │                                                                                                               │                       └── static void bta_dm_inq_results_cb(tBTM_INQ_RESULTS* p_inq, const uint8_t* p_eir,uint16_t eir_len)
                      │                                                                                                               │                           └── bta_dm_search_cb.p_search_cback(BTA_DM_INQ_RES_EVT, &result);
                      │                                                                                                               ├── bta_dm_enable(bte_dm_evt);
                      │                                                                                                               └── BTA_dm_on_hw_on();
                      ├── Message enableDelayedMsg = mHandler.obtainMessage(MESSAGE_HANDLE_ENABLE_DELAYED);
                      └── mHandler.sendMessageDelayed(enableDelayedMsg, ENABLE_DISABLE_DELAY_MS);
```

# 蓝牙扫描

```
* packages/modules/Bluetooth/framework/java/android/bluetooth/BluetoothAdapter.java
  └── public boolean startDiscovery()
      └── mService.startDiscovery(mAttributionSource, recv);
          └── public void startDiscovery(AttributionSource source, SynchronousResultReceiver receiver)  
              └── receiver.send(startDiscovery(source));
                  └── private boolean startDiscovery(AttributionSource attributionSource) 
                      └── return service.startDiscovery(attributionSource);
                          └── boolean startDiscovery(AttributionSource attributionSource) 
                              └── return startDiscoveryNative();
                                  └── int ret = sBluetoothInterface->start_discovery();
                                      └── do_in_main_thread(FROM_HERE, base::BindOnce(btif_dm_start_discovery));
                                          └── packages/modules/Bluetooth/system/btif/src/btif_dm.cc
                                              └── void btif_dm_start_discovery(void) 
                                                  ├── if (bta_dm_is_search_request_queued())
                                                  │   └──  return bta_dm_search_cb.p_pending_search != NULL;
                                                  └──  BTA_DmSearch(btif_dm_search_devices_evt, is_bonding_or_sdp());
                                                      ├── void BTA_DmSearch(tBTA_DM_SEARCH_CBACK* p_cback, bool is_bonding_or_sdp)
                                                      └── static void btif_dm_search_devices_evt(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH* p_search_data)
                                                          └── invoke_device_found_cb(num_properties, properties);
                                                              └── void invoke_device_found_cb(int num_properties, bt_property_t* properties)  
```

# app 获取搜索到的设备

```
* packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/AdapterService.java 
  └── public void onCreate()
      └── initNative(mUserManager.isGuestUser(), isCommonCriteriaMode(), configCompareResult, getInitFlags(), isAtvDevice, getApplicationInfo().dataDir);
          └── packages/modules/Bluetooth/android/app/jni/com_android_bluetooth_btservice_AdapterService.cpp 
              └── int ret = sBluetoothInterface->init(&sBluetoothCallbacks, isGuest == JNI_TRUE ? 1 : 0,isCommonCriteriaMode == JNI_TRUE ? 1 : 0, configCompareResult, flags, isAtvDevice == JNI_TRUE ? 1 : 0, user_data_directory);
                  ├── static int init(bt_callbacks_t* callbacks, bool start_restricted
                  │   └── set_hal_cbacks(callbacks);
                  │       └── void set_hal_cbacks(bt_callbacks_t* callbacks) { bt_hal_cbacks = callbacks; }
                  │           └── 
                  └── static bt_callbacks_t sBluetoothCallbacks = {sizeof(sBluetoothCallbacks),adapter_state_change_callback,adapter_properties_callback,remote_device_properties_callback,device_found_callback,discovery_state_changed_callback,pin_request_callback,ssp_request_callback,bond_state_changed_callback,address_consolidate_callback,acl_state_changed_callback,callback_thread_event,dut_mode_recv_callback,le_test_mode_recv_callback,energy_info_recv_callback,link_quality_report_callback,generate_local_oob_data_callback,switch_buffer_size_callback,switch_codec_callback};
                      └── sCallbackEnv->CallVoidMethod(sJniCallbacksObj, method_deviceFoundCallback,addr.get());
                          └── method_deviceFoundCallback = env->GetMethodID(jniCallbackClass, "deviceFoundCallback", "([B)V");
                              └── packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/JniCallbacks.java
                                  └── mRemoteDevices.deviceFoundCallback(address);
                                      └── packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/RemoteDevices.java 
                                          └── void deviceFoundCallback(byte[] address) 
                                              ├── Intent intent = new Intent(BluetoothDevice.ACTION_FOUND);
                                              └── sAdapterService.sendBroadcastMultiplePermissions
                                            
```

# 蓝牙配对

```
* frameworks/base/packages/SettingsLib/src/com/android/settingslib/bluetooth/CachedBluetoothDevice.java
  └── public boolean startPairing()  
      └── if (!mDevice.createBond())
          └── packages/modules/Bluetooth/framework/java/android/bluetooth/BluetoothDevice.java 
              └── return createBond(TRANSPORT_AUTO);
                  ├── return createBondInternal(transport, null, null);
                  │   └── service.createBond(this, transport, remoteP192Data, remoteP256Data,mAttributionSource, recv);
                  │       └── packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/AdapterService.java 
                  │           └── receiver.send(createBond(device, transport, remoteP192Data, remoteP256Data, source));
                  │               └── private boolean createBond(BluetoothDevice device, int transport, OobData remoteP192Data, OobData remoteP256Data, AttributionSource attributionSource) 
                  │                   └── Message msg = mBondStateMachine.obtainMessage(BondStateMachine.CREATE_BOND);
                  │                       └── packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/BondStateMachine.java
                  │                           └── result = createBond(dev, msg.arg1, p192Data, p256Data, false);
                  │                               └── result = mAdapterService.createBondNative(addr, transport);
                  │                                   └── packages/modules/Bluetooth/android/app/jni/com_android_bluetooth_btservice_AdapterService.cpp 
                  │                                       └── int ret = sBluetoothInterface->create_bond((RawAddress*)addr, transport);
                  │                                           └── do_in_main_thread(FROM_HERE, base::BindOnce(btif_dm_create_bond, *bd_addr, transport));
                  │                                               └── void btif_dm_create_bond(const RawAddress bd_addr, int transport) 
                  │                                                   ├── btif_stats_add_bond_event(bd_addr, BTIF_DM_FUNC_CREATE_BOND,pairing_cb.state);
                  │                                                   └── btif_dm_cb_create_bond(bd_addr, transport);
                  │                                                       └── static void btif_dm_cb_create_bond(const RawAddress bd_addr,tBT_TRANSPORT transport)
                  │                                                           └── bond_state_changed(BT_STATUS_SUCCESS, bd_addr, BT_BOND_STATE_BONDING);  ///这里进行蓝牙状态变化的上报
                  │                                                               ├── invoke_bond_state_changed_cb(status, bd_addr, state, pairing_cb.fail_reason);
                  │                                                               │   └── HAL_CBACK(bt_hal_cbacks,bond_state_changed_cb, status,&bd_addr, state, fail_reason);
                  │                                                               │       └── typedef struct {... bond_state_changed_callback bond_state_changed_cb;...}
                  │                                                               │           └── static void bond_state_changed_callback(bt_status_t status, RawAddress* bd_addr,bt_bond_state_t state, int fail_reason)
                  │                                                               │               └── void bondStateChangeCallback(int status, byte[] address, int newState, int hciReason) 
                  │                                                               │                   └── mBondStateMachine.bondStateChangeCallback(status, address, newState, hciReason);// 
                  │                                                               └── BTA_DmBond(bd_addr, addr_type, transport, device_type);
                  │                                                                   └── do_in_main_thread(FROM_HERE, base::Bind(bta_dm_bond, bd_addr, addr_type, transport, device_type));
                  │                                                                       ├── BTM_SecBond(bd_addr, addr_type, transport, device_type, 0, NULL);
                  │                                                                       │   └── btm_sec_bond_by_transport(bd_addr, addr_type, transport, pin_len, p_pin);
                  │                                                                       │       ├── if (!controller_get_interface()->supports_simple_pairing()) 
                  │                                                                       │       └── btsnd_hcic_write_pin_type(HCI_PIN_TYPE_FIXED);
                  │                                                                       └── bta_dm_cb.p_sec_cback(BTA_DM_AUTH_CMPL_EVT, &sec_event);
                  ├── mRemoteDevices.setBondingInitiatedLocally(Utils.getByteAddress(device));
                  ├── cancelDiscoveryNative();
                  └── mBondStateMachine.sendMessage(msg);

```

# 开始配对后，确认是否进行配对
```
* packages/apps/Settings/src/com/android/settings/bluetooth/BluetoothPairingController.java
  └── private void onPair(String passkey)
      ├── mDevice.setPin(passkey);
      └── mDevice.setPairingConfirmation(true);
          └── packages/modules/Bluetooth/framework/java/android/bluetooth/BluetoothDevice.java
              ├── final IBluetooth service = getService();
              └── service.setPairingConfirmation(this, confirm, mAttributionSource, recv);
                  └── packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/AdapterService.java
                      └── public void setPairingConfirmation(BluetoothDevice device, boolean accept, AttributionSource source, SynchronousResultReceiver receiver) 
                          └── receiver.send(setPairingConfirmation(device, accept, source));
                              └── AdapterService service = getService();
                                  └── service.sspReplyNative(getBytesFromAddress(device.getAddress()),AbstractionLayer.BT_SSP_VARIANT_PASSKEY_CONFIRMATION, accept,0);
                                      └── packages/modules/Bluetooth/android/app/jni/com_android_bluetooth_btservice_AdapterService.cpp
                                          └── int ret = sBluetoothInterface->ssp_reply((RawAddress*)addr, (bt_ssp_variant_t)type, accept, passkey);
                                              └── packages/modules/Bluetooth/system/btif/src/bluetooth.cc
                                                  └── do_in_main_thread(FROM_HERE, base::BindOnce(btif_dm_ssp_reply, *bd_addr, variant, accept))
                                                      └── packages/modules/Bluetooth/system/btif/src/btif_dm.cc
                                                          └── void btif_dm_ssp_reply(const RawAddress bd_addr, bt_ssp_variant_t variant, uint8_t accept) 
```

# 蓝牙状态变更上报

这里以BondStateMachine为例进行代码跟踪，其他的回调也是类似
```
* packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/AdapterService.java 
  ├── classInitNative();
  │   └── packages/modules/Bluetooth/android/app/jni/com_android_bluetooth_btservice_AdapterService.cpp
  │       ├── method_stateChangeCallback = env->GetMethodID(jniCallbackClass, "stateChangeCallback", "(I)V");
  │       ├── method_deviceFoundCallback = env->GetMethodID(jniCallbackClass, "deviceFoundCallback", "([B)V");
  │       ├── method_pinRequestCallback = env->GetMethodID(jniCallbackClass, "pinRequestCallback", "([B[BIZ)V");
  │       ├── method_sspRequestCallback = env->GetMethodID(jniCallbackClass, "sspRequestCallback", "([B[BIII)V");
  │       └── method_bondStateChangeCallback = env->GetMethodID(jniCallbackClass, "bondStateChangeCallback", "(I[BII)V");
  └── public void onCreate()
      ├── mJniCallbacks = new JniCallbacks(this, mAdapterProperties);
      └── initNative(mUserManager.isGuestUser(), isCommonCriteriaMode(), configCompareResult, getInitFlags(), isAtvDevice, getApplicationInfo().dataDir);
          └── int ret = sBluetoothInterface->init(&sBluetoothCallbacks, isGuest == JNI_TRUE ? 1 : 0,isCommonCriteriaMode == JNI_TRUE ? 1 : 0, configCompareResult, flags,isAtvDevice == JNI_TRUE ? 1 : 0, user_data_directory);//这里将回调函数传入
              └── static bt_callbacks_t sBluetoothCallbacks = {sizeof(sBluetoothCallbacks),adapter_state_change_callback,adapter_properties_callback,remote_device_properties_callback,device_found_callback,discovery_state_changed_callback,pin_request_callback,ssp_request_callback,bond_state_changed_callback,address_consolidate_callback,acl_state_changed_callback,callback_thread_event,dut_mode_recv_callback,le_test_mode_recv_callback,energy_info_recv_callback,link_quality_report_callback,generate_local_oob_data_callback,switch_buffer_size_callback,switch_codec_callback};
                  └── static void bond_state_changed_callback(bt_status_t status, RawAddress* bd_addr, bt_bond_state_t state,int fail_reason) 
                      └── sCallbackEnv->CallVoidMethod(sJniCallbacksObj, method_bondStateChangeCallback,(jint)status, addr.get(), (jint)state,(jint)fail_reason);
                          └── packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/JniCallbacks.java
                              └── void bondStateChangeCallback(int status, byte[] address, int newState, int hciReason)
                                  └── mBondStateMachine.bondStateChangeCallback(status, address, newState, hciReason);
                                      └── packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/BondStateMachine.java
                                          └── void bondStateChangeCallback(int status, byte[] address, int newState, int hciReason) 
                                              ├── Message msg = obtainMessage(BONDING_STATE_CHANGE);
                                              └── sendMessage(msg);
                                                  └── sendIntent(dev, newState, reason, false);
                                                      └── packages/modules/Bluetooth/android/app/src/com/android/bluetooth/a2dp/A2dpService.java
                                                          └── private class BondStateChangedReceiver extends BroadcastReceiver 
                                                              └── bondStateChanged(device, state)
                                                                  └── void bondStateChanged(BluetoothDevice device, int bondState)
                                                                      └──  if (bondState != BluetoothDevice.BOND_NONE) return; //到这里最终好像也没处理什么
```
# 蓝牙配对状态改变

在userdebug版本下可以看到日志，蓝牙配对过程中状态的切换
IDLE => WAIT_PIN_REQ => WAIT_NUM_CONFIRM => WAIT_AUTH_COMPLETE => IDLE
```
03-23 17:56:52.593652  9439  9489 I bt_btm_sec: btm_sec_change_pairing_state: Pairing state changed IDLE => WAIT_PIN_REQ pairing_flags:0x5
03-23 17:56:54.060246  9439  9489 I bt_btm_sec: btm_sec_change_pairing_state: Pairing state changed WAIT_PIN_REQ => WAIT_LOCAL_IOCAPS pairing_flags:0x5
03-23 17:56:54.311093  9439  9489 I bt_btm_sec: btm_sec_change_pairing_state: Pairing state changed WAIT_LOCAL_IOCAPS => WAIT_NUM_CONFIRM pairing_flags:0x5
03-23 17:56:55.985548  9439  9489 I bt_btm_sec: btm_sec_change_pairing_state: Pairing state changed WAIT_NUM_CONFIRM => WAIT_AUTH_COMPLETE pairing_flags:0x5
03-23 17:56:56.057715  9439  9489 I bt_btm_sec: btm_sec_change_pairing_state: Pairing state changed WAIT_AUTH_COMPLETE => IDLE pairing_flags:0x5
```