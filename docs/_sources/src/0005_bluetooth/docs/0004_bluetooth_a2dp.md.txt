# bluetooth a2dp

bluetooth a2dp

# 参考文档

* [android -- 蓝牙 bluetooth （五）接电话与听音乐](https://blog.csdn.net/baimy1985/article/details/9275559)
* [蓝牙音乐之A2DP](https://blog.csdn.net/weixin_44260005/article/details/107225738)

* [蓝牙音乐之A2DP音频流](https://blog.csdn.net/weixin_44260005/article/details/107391323)
* [A2DP音频流在安卓系统中的实现](https://blog.csdn.net/weixin_44260005/article/details/107616222)

# a2dp

这里以BondStateMachine为例进行代码跟踪，其他的回调也是类似
```
* vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/AdapterService.java 
  ├── classInitNative();
  │   └── vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/jni/com_android_bluetooth_btservice_AdapterService.cpp 
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
                          └── vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/JniCallbacks.java
                              └── void bondStateChangeCallback(int status, byte[] address, int newState, int hciReason)
                                  └── mBondStateMachine.bondStateChangeCallback(status, address, newState, hciReason);
                                      └── vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/BondStateMachine.java
                                          └── void bondStateChangeCallback(int status, byte[] address, int newState, int hciReason) 
                                              ├── Message msg = obtainMessage(BONDING_STATE_CHANGE);
                                              └── sendMessage(msg);
                                                  └── sendIntent(dev, newState, reason, false);
                                                      └── vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/a2dp/A2dpService.java
                                                          └── private class BondStateChangedReceiver extends BroadcastReceiver 
                                                              └── bondStateChanged(device, state)
                                                                  └── void bondStateChanged(BluetoothDevice device, int bondState)
                                                                      └──  if (bondState != BluetoothDevice.BOND_NONE) return; //到这里最终好像也没处理什么
```

a2dp connect
```
* vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/a2dp/A2dpStateMachine.java 
  ├── if (!mA2dpNativeInterface.connectA2dp(mDevice))
  │   └── vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/a2dp/A2dpNativeInterface.java
  │       └── public boolean connectA2dp(BluetoothDevice device)
  │           └── return connectA2dpNative(getByteAddress(device))
  │               └── vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/jni/com_android_bluetooth_a2dp.cpp 
  │                   └── static jboolean connectA2dpNative(JNIEnv* env, jobject object, jbyteArray address)
  │                       └── bt_status_t status = sBluetoothA2dpInterface->connect(bd_addr);
  │                           ├── sBluetoothA2dpInterface = (btav_source_interface_t*)btInf->get_profile_interface(BT_PROFILE_ADVANCED_AUDIO_ID)// initNative
  │                           │   └── vendor/mediatek/proprietary/packages/modules/Bluetooth/system/service/bluetooth_interface.cc
  │                           │       └── static const void* get_profile_interface(const char* profile_id) 
  │                           │           ├── if (is_profile(profile_id, BT_PROFILE_ADVANCED_AUDIO_ID))
  │                           │           └── return btif_av_get_src_interface();
  │                           │               └── mediatek/proprietary/packages/modules/Bluetooth/system/btif/src/btif_av.cc 
  │                           │                   └── const btav_source_interface_t* btif_av_get_src_interface(void) 
  │                           │                       └── return &bt_av_src_interface;
  │                           │                           └──  static const btav_source_interface_t bt_av_src_interface = {sizeof(btav_source_interface_t),init_src, src_connect_sink,src_disconnect_sink, src_set_silence_sink,src_set_active_sink,codec_config_src,cleanup_src, };
  │                           └── vendor/mediatek/proprietary/packages/modules/Bluetooth/system/btif/src/btif_av.cc 
  │                               └── static bt_status_t src_connect_sink(const RawAddress& peer_address)
  │                                   └── return btif_queue_connect(UUID_SERVCLASS_AUDIO_SOURCE, &peer_address_copy,connect_int);     
  │                                       └── vendor/mediatek/proprietary/packages/modules/Bluetooth/system/btif/src/btif_profile_queue.cc 
  │                                           └── bt_status_t btif_queue_connect(uint16_t uuid, const RawAddress* bda, btif_connect_cb_t connect_cb)     
  │                                               └── return do_in_jni_thread(FROM_HERE,base::Bind(&queue_int_add, uuid, *bda, connect_cb));   
  │                                                   └── static void queue_int_add(uint16_t uuid, const RawAddress& bda, btif_connect_cb_t connect_cb)   
  │                                                       ├── connect_queue.push_back(param);  
  │                                                       └── btif_queue_connect_next(); 
  └── if (mA2dpService.okToConnect(mDevice, true)) 
      └── vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/a2dp/A2dpService.java 
          └── connectionPolicy = getConnectionPolicy(device);
```
# 连接状态改变，同audio 部分挂钩
```
* vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/JniCallbacks.java
  └── void bondStateChangeCallback(int status, byte[] address, int newState, int hciReason)
      └── mBondStateMachine.bondStateChangeCallback(status, address, newState, hciReason);
          └── vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/BondStateMachine.java
              └── void bondStateChangeCallback(int status, byte[] address, int newState, int hciReason) 
                  ├── Message msg = obtainMessage(BONDING_STATE_CHANGE);
                  └── sendMessage(msg);
                      └── sendIntent(dev, newState, reason, false);
                          └── vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/a2dp/A2dpService.java
                              └── private class ConnectionStateChangedReceiver extends BroadcastReceiver
                                  └── connectionStateChanged(device, fromState, toState); 
                                      └── setActiveDevice(device);
                                          ├── final LeAudioService leAudioService = mFactory.getLeAudioService(); 
                                          ├── updateAndBroadcastActiveDevice(device);
                                          │   ├── Intent intent = new Intent(BluetoothA2dp.ACTION_ACTIVE_DEVICE_CHANGED);
                                          │   └── sendBroadcast(intent, BLUETOOTH_CONNECT, Utils.getTempAllowlistBroadcastOptions());
                                          ├── if (!mA2dpNativeInterface.setActiveDevice(device)) 
                                          │   └── static jboolean setActiveDeviceNative(JNIEnv* env, jobject object, jbyteArray address)
                                          │       └── bt_status_t status = sBluetoothA2dpInterface->set_active_device(bd_addr);
                                          │           └── vendor/mediatek/proprietary/packages/modules/Bluetooth/system/btif/src/btif_av.cc 
                                          │               └── static bt_status_t src_set_active_sink(const RawAddress& peer_address) 
                                          │                   └── bt_status_t status = do_in_main_thread(FROM_HERE, base::BindOnce(&set_active_peer_int,AVDT_TSEP_SNK, peer_address, std::move(peer_ready_promise)));
                                          └── mAudioManager.handleBluetoothActiveDeviceChanged(newActiveDevice,ull,BluetoothProfileConnectionInfo.createA2dpInfo(true,rememberedVolume));
                                              └── frameworks/base/media/java/android/media/AudioManager.java
                                                  └── service.handleBluetoothActiveDeviceChanged(newDevice, previousDevice, info);
```

