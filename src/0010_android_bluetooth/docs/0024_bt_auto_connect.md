# bt auto connect

测试反馈蓝牙性能测试比竞品慢

# 参考文档

* [android系统蓝牙自动连接](https://blog.csdn.net/qq_34541276/article/details/105912719)
* [A2dp连接流程源码分析](https://www.lmlphp.com/user/58517/article/item/1567559/)

# 问题描述

打开蓝牙到蓝牙设备自动连接上整体耗时同竞品相差17%左右

# 日志分析

蓝牙打开状态切换耗时
```
//打开蓝牙
05-16 15:41:31.366070  1282  2937 D BluetoothManagerService: enable(com.android.settings):  mBluetooth =android.bluetooth.IBluetooth$Stub$Proxy@5521bca mBinding = false mState = BLE_ON 

//蓝牙状态切换
Line   12: 05-16 15:41:31.374898  1282  2967 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: BLE_ON > TURNING_ON
Line 1094: 05-16 15:41:32.087120  1282  2967 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: TURNING_ON > ON

//通知蓝牙状态切换
Line   13: 05-16 15:41:31.375314  1282  2967 D BluetoothManagerService: Sending BLE State Change: BLE_ON > TURNING_ON
Line   14: 05-16 15:41:31.377081  1282  2967 D BluetoothManagerService: Sending State Change: OFF > TURNING_ON
Line 1394: 05-16 15:41:32.200072  1282  2967 D BluetoothManagerService: Sending BLE State Change: TURNING_ON > ON
Line 1644: 05-16 15:41:32.226683  1282  2967 D BluetoothManagerService: Sending State Change: TURNING_ON > ON

```

蓝牙打开后，监听到BluetoothAdapter.ACTION_STATE_CHANGED，开始自动设备自动连接
```
* mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/PhonePolicy.java 
  └── private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
      └── case BluetoothAdapter.ACTION_STATE_CHANGED:
          └── mHandler.obtainMessage(MESSAGE_ADAPTER_STATE_TURNED_ON).sendToTarget();
              └── public void handleMessage(Message msg) 
                  └── case MESSAGE_ADAPTER_STATE_TURNED_ON:
                      └── autoConnect();
                          ├── autoConnectHeadset(mostRecentlyActiveA2dpDevice);
                          │   └── hsService.connect(device);
                          │       └── stateMachine.sendMessage(HeadsetStateMachine.CONNECT, device);
                          │           └── public boolean processMessage(Message message)
                          │               └── case CONNECT:
                          │                   ├── if (!mNativeInterface.connectHfp(device)) 
                          │                   └── transitionTo(mConnecting);
                          │                       └── void messageFromNative(HeadsetStackEvent stackEvent) 
                          │                           └── stateMachine.sendMessage(HeadsetStateMachine.STACK_EVENT, stackEvent);
                          │                               └── case STACK_EVENT
                          │                                   ├── case HeadsetStackEvent.EVENT_TYPE_CONNECTION_STATE_CHANGED:
                          │                                   │   └── processConnectionEvent(message, event.valueInt);
                          │                                   │       ├── case HeadsetHalConstants.CONNECTION_STATE_SLC_CONNECTED:
                          │                                   │       │   └── transitionTo(mConnected);
                          │                                   │       └── case HeadsetHalConstants.CONNECTION_STATE_DISCONNECTED:
                          │                                   │           └── transitionTo(mDisconnected);
                          │                                   └── case HeadsetStackEvent.EVENT_TYPE_VOLUME_CHANGED:
                          │                                       └── processVolumeEvent(event.valueInt, event.valueInt2);
                          └── autoConnectA2dp(mostRecentlyActiveA2dpDevice);
                              └── a2dpService.connect(device);
                                  └── smConnect.sendMessage(A2dpStateMachine.CONNECT);
                                      └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/a2dp/A2dpStateMachine.java 
                                          └── public boolean processMessage(Message message)
                                              └── case CONNECT:
                                                  ├── mA2dpNativeInterface.connectA2dp(mDevice)
                                                  └── mA2dpService.okToConnect(mDevice, true)
                                                      └── transitionTo(mConnecting);
                                                          └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/a2dp/A2dpService.java 
                                                              └── void messageFromNative(A2dpStackEvent stackEvent) 
                                                                  └── sm.sendMessage(A2dpStateMachine.STACK_EVENT, stackEvent);
                                                                      └── case STACK_EVENT
                                                                          ├── case A2dpStackEvent.EVENT_TYPE_CONNECTION_STATE_CHANGED:
                                                                          │   └── processConnectionEvent(event.valueInt);
                                                                          └── case A2dpStackEvent.EVENT_TYPE_CODEC_CONFIG_CHANGED:
                                                                              └── processCodecConfigEvent(event.codecStatus);
```

STACK_EVENT 事件上报逻辑，此部分都是通过回调进行上报
```
* mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/btif/src/btif_av.cc 
  └── static void btif_report_connection_state(const RawAddress& peer_address, btav_connection_state_t state)
      └── base::Bind(btif_av_source.Callbacks()->connection_state_cb, peer_address, state));
          └── static void bta2dp_connection_state_callback(const RawAddress& bd_addr, btav_connection_state_t state)
              └── sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onConnectionStateChanged, addr.get(), (jint)state);
                  └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/a2dp/A2dpNativeInterface.java 
                      └── private void onConnectionStateChanged(byte[] address, int state)
                          └── service.messageFromNative(event);
                              └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/a2dp/A2dpService.java 
                                  └── void messageFromNative(A2dpStackEvent stackEvent)
                                      └── sm.sendMessage(A2dpStateMachine.STACK_EVENT, stackEvent);
```

初始化过程中设置回调函数
```
* android/vendor/mediatek/proprietary/packages/apps/Bluetooth/jni/com_android_bluetooth_a2dp.cpp
  └── static void initNative(JNIEnv* env, jobject object,jint maxConnectedAudioDevices,jobjectArray codecConfigArray,jobjectArray codecOffloadingArray) 
      ├── sBluetoothA2dpInterface =(btav_source_interface_t*)btInf->get_profile_interface(BT_PROFILE_ADVANCED_AUDIO_ID);
      │   └── android/system/bt/btif/src/bluetooth.cc
      │       └── if (is_profile(profile_id, BT_PROFILE_ADVANCED_AUDIO_ID))
      │           └── return btif_av_get_src_interface();
      │               └── return &bt_av_src_interface;
      │                   └── static const btav_source_interface_t bt_av_src_interface = {...init_src...}
      └── bt_status_t status = sBluetoothA2dpInterface->init( &sBluetoothA2dpCallbacks, maxConnectedAudioDevices, codec_priorities,codec_offloading);
          └── static btav_source_callbacks_t sBluetoothA2dpCallbacks = {...bta2dp_connection_state_callback...};
              └── static void bta2dp_connection_state_callback(const RawAddress& bd_addr,btav_connection_state_t state)
                  └── sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onConnectionStateChanged,addr.get(), (jint)state);
                      └── method_onConnectionStateChanged = env->GetMethodID(clazz, "onConnectionStateChanged", "([BI)V");
```

A2DP c层连接逻辑
```
* android/vendor/mediatek/proprietary/packages/apps/Bluetooth/jni/com_android_bluetooth_a2dp.cpp 
  └── static jboolean connectA2dpNative(JNIEnv* env, jobject object,jbyteArray address)
      └── bt_status_t status = sBluetoothA2dpInterface->connect(bd_addr);
          └── static bt_status_t src_connect_sink(const RawAddress& peer_address) 
              └── return btif_queue_connect(UUID_SERVCLASS_AUDIO_SOURCE, &peer_address_copy,connect_int);
                  └── static bt_status_t connect_int(RawAddress* peer_address, uint16_t uuid) 
                      ├── peer = btif_av_source.FindOrCreatePeer(*peer_address, kBtaHandleUnknown);
                      └── peer->StateMachine().ProcessEvent(BTIF_AV_CONNECT_REQ_EVT, nullptr);
                          └── bool BtifAvStateMachine::StateIdle::ProcessEvent(uint32_t event, void* p_data)
                              └── case BTIF_AV_CONNECT_REQ_EVT:
                                  ├── btif_av_query_mandatory_codec_priority(peer_.PeerAddress());
                                  ├── BTA_AvOpen(peer_.PeerAddress(), peer_.BtaHandle(), true, peer_.LocalUuidServiceClass());
                                  │   ├── p_buf->hdr.event = BTA_AV_API_OPEN_EVT;
                                  │   └── bta_sys_sendmsg(p_buf);
                                  │       └── static void bta_av_better_stream_state_machine(tBTA_AV_SCB* p_scb, uint16_t event, tBTA_AV_DATA* p_data) 
                                  │           └── case BTA_AV_API_OPEN_EVT:
                                  │               ├── p_scb->state = BTA_AV_OPENING_SST;
                                  │               ├── event_handler1 = &bta_av_do_disc_a2dp;
                                  │               │   └── void bta_av_do_disc_a2dp(tBTA_AV_SCB* p_scb, tBTA_AV_DATA* p_data) 
                                  │               │       └── bta_av_a2dp_sdp_cback(true, &a2dp_ser, p_scb->PeerAddress());
                                  │               │           ├── p_msg->hdr.event = BTA_AV_SDP_DISC_OK_EVT;
                                  │               │           │   └── event_handler1 = &bta_av_connect_req;
                                  │               │           │       └── void bta_av_connect_req(tBTA_AV_SCB* p_scb, UNUSED_ATTR tBTA_AV_DATA* p_data)
                                  │               │           │           └── AVDT_ConnectReq(p_scb->PeerAddress(), p_scb->hdi, &bta_av_proc_stream_evt);
                                  │               │           │               ├── avdt_ccb_event(p_ccb, AVDT_CCB_API_CONNECT_REQ_EVT, &evt);
                                  │               │           │               │   ├── if (p_ccb->state != state_table[event][AVDT_CCB_NEXT_STATE]) 
                                  │               │           │               │   │   └── p_ccb->state = state_table[event][AVDT_CCB_NEXT_STATE]; // 状态及状态的切换
                                  │               │           │               │   └──  for (i = 0; i < AVDT_CCB_ACTIONS; i++)
                                  │               │           │               │       └── action = state_table[event][i];
                                  │               │           │               └── void bta_av_proc_stream_evt(uint8_t handle, const RawAddress& bd_addr,uint8_t event, tAVDT_CTRL* p_data, uint8_t scb_index) avdt_ccb_event(p_ccb, AVDT_CCB_API_CONNECT_REQ_EVT, &evt);
                                  │               │           │                   ├── p_msg->hdr.event = bta_av_stream_evt_ok[event];
                                  │               │           │                   └── bta_sys_sendmsg(p_msg);
                                  │               │           └── bta_sys_sendmsg(p_msg);
                                  │               └── event_handler1(p_scb, p_data);
                                  └── peer_.StateMachine().TransitionTo(BtifAvStateMachine::kStateOpening);
                                      ├── BtifAvStateMachine::StateOpening::OnEnter() 
                                      │   └── btif_report_connection_state(peer_.PeerAddress(), BTAV_CONNECTION_STATE_CONNECTING);
                                      └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/bta/av/bta_av_ssm.cc
                                          └── event_handler1 = &bta_av_str_opened;
                                              └── bta_av_str_opened
                                                  └── (*bta_av_cb.p_cback)(BTA_AV_OPEN_EVT, &bta_av_data);
                                                      ├── av_state = BtifAvStateMachine::kStateOpened;
                                                      └── peer_.StateMachine().TransitionTo(av_state);//状态机器由kStateOpening->kStateOpened
```

这里有个状态机的切换，由avdt_ccb_st_idle->avdt_ccb_st_opening->avdt_ccb_st_open->avdt_ccb_st_closing状态进行切换
```
* avdt_ccb_event(p_ccb, AVDT_CCB_API_CONNECT_REQ_EVT, &evt); 
  ├── p_ccb->state = state_table[event][AVDT_CCB_NEXT_STATE];
  └── action = state_table[event][i];
      └── {AVDT_CCB_SET_CONN, AVDT_CCB_CHAN_OPEN, AVDT_CCB_OPENING_ST},
          ├── avdt_ccb_set_conn
          │   └── p_ccb->p_conn_cback = p_data->connect.p_cback;
          └── avdt_ccb_chan_open
              └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/stack/avdt/avdt_ad.cc 
                  └── avdt_ad_open_req(AVDT_CHAN_SIG, p_ccb, NULL, AVDT_INT);
                      ├── if (role == AVDT_ACP) 
                      │   └── p_tbl->state = AVDT_AD_ST_ACP;
                      └── else {
                          ├── p_tbl->state = AVDT_AD_ST_CONN;
                          └── L2CA_ConnectReq2(AVDT_PSM, p_ccb->peer_addr, BTM_SEC_OUT_AUTHENTICATE);
                              └── uint16_t L2CA_ConnectReq(uint16_t psm, const RawAddress& p_bd_addr) 
```

* BTA_AV_AVDT_CONNECT_EVT怎么被抛出来的
  avdt_ccb_ll_opened是在avdt_ccb_event中状态机的切换过程中被调用
```
* void avdt_ccb_ll_opened(AvdtpCcb* p_ccb, tAVDT_CCB_EVT* p_data)
  └── (*p_ccb->p_conn_cback)(0, p_ccb->peer_addr, AVDT_CONNECT_IND_EVT, &avdt_ctrl, p_ccb->BtaAvScbIndex());
      └── void bta_av_proc_stream_evt(uint8_t handle, const RawAddress& bd_addr,uint8_t event, tAVDT_CTRL* p_data, uint8_t scb_index)
          ├── p_msg->hdr.event = bta_av_stream_evt_ok[event];
          └── bta_sys_sendmsg(p_msg);
              └── static void bta_av_better_stream_state_machine(tBTA_AV_SCB* p_scb, uint16_t event, tBTA_AV_DATA* p_data) 
                  └── event_handler1 = &bta_av_discover_req;
```

c层中状态机切换及回调的数据结构
```
 static btav_source_callbacks_t sBluetoothA2dpCallbacks = {
      sizeof(sBluetoothA2dpCallbacks),
      bta2dp_connection_state_callback,
      bta2dp_audio_state_callback,
      bta2dp_audio_config_callback,
      bta2dp_mandatory_codec_preferred_callback,
  };

 static const btav_source_interface_t bt_av_src_interface = {
     sizeof(btav_source_interface_t),
      init_src,
      src_connect_sink,
      src_disconnect_sink,
      src_set_silence_sink,
      src_set_active_sink,
      codec_config_src,
      cleanup_src,
  };

  typedef struct {
    /** set to sizeof(btav_source_interface_t) */
    size_t size;
    /**
     * Register the BtAv callbacks.
     */
    bt_status_t (*init)(
        btav_source_callbacks_t* callbacks, int max_connected_audio_devices,
        std::vector<btav_a2dp_codec_config_t>& codec_priorities,
        const std::vector<btav_a2dp_codec_config_t>& offloading_preference);
  
    /** connect to headset */
    bt_status_t (*connect)(const RawAddress& bd_addr);
  
    /** dis-connect from headset */
    bt_status_t (*disconnect)(const RawAddress& bd_addr);
  
    /** sets the connected device silence state */
    bt_status_t (*set_silence_device)(const RawAddress& bd_addr, bool silence);
  
    /** sets the connected device as active */
    bt_status_t (*set_active_device)(const RawAddress& bd_addr);
  
    /** configure the codecs settings preferences */
    bt_status_t (*config_codec)(
        const RawAddress& bd_addr,
        std::vector<btav_a2dp_codec_config_t> codec_preferences);
  
    /** Closes the interface. */
    void (*cleanup)(void);
  
  } btav_source_interface_t;

  static const uint16_t bta_av_stream_evt_ok[] = {
      BTA_AV_STR_DISC_OK_EVT,      /* AVDT_DISCOVER_CFM_EVT */
      BTA_AV_STR_GETCAP_OK_EVT,    /* AVDT_GETCAP_CFM_EVT */
      BTA_AV_STR_OPEN_OK_EVT,      /* AVDT_OPEN_CFM_EVT */
      BTA_AV_STR_OPEN_OK_EVT,      /* AVDT_OPEN_IND_EVT */
      BTA_AV_STR_CONFIG_IND_EVT,   /* AVDT_CONFIG_IND_EVT */
      BTA_AV_STR_START_OK_EVT,     /* AVDT_START_CFM_EVT */
      BTA_AV_STR_START_OK_EVT,     /* AVDT_START_IND_EVT */
      BTA_AV_STR_SUSPEND_CFM_EVT,  /* AVDT_SUSPEND_CFM_EVT */
      BTA_AV_STR_SUSPEND_CFM_EVT,  /* AVDT_SUSPEND_IND_EVT */
      BTA_AV_STR_CLOSE_EVT,        /* AVDT_CLOSE_CFM_EVT */
      BTA_AV_STR_CLOSE_EVT,        /* AVDT_CLOSE_IND_EVT */
      BTA_AV_STR_RECONFIG_CFM_EVT, /* AVDT_RECONFIG_CFM_EVT */
      0,                           /* AVDT_RECONFIG_IND_EVT */
      BTA_AV_STR_SECURITY_CFM_EVT, /* AVDT_SECURITY_CFM_EVT */
      BTA_AV_STR_SECURITY_IND_EVT, /* AVDT_SECURITY_IND_EVT */
      BTA_AV_STR_WRITE_CFM_EVT,    /* AVDT_WRITE_CFM_EVT */
      BTA_AV_AVDT_CONNECT_EVT,     /* AVDT_CONNECT_IND_EVT */
      BTA_AV_AVDT_DISCONNECT_EVT,  /* AVDT_DISCONNECT_IND_EVT */
      BTA_AV_AVDT_RPT_CONN_EVT, /* AVDT_REPORT_CONN_EVT */
      BTA_AV_AVDT_RPT_CONN_EVT, /* AVDT_REPORT_DISCONN_EVT */
      BTA_AV_AVDT_DELAY_RPT_EVT, /* AVDT_DELAY_REPORT_EVT */
      BTA_AV_AVDT_DELAY_RPT_CFM_EVT, /* AVDT_DELAY_REPORT_CFM_EVT */
  };
  
  static const uint16_t bta_av_stream_evt_fail[] = {
      BTA_AV_STR_DISC_FAIL_EVT,    /* AVDT_DISCOVER_CFM_EVT */
      BTA_AV_STR_GETCAP_FAIL_EVT,  /* AVDT_GETCAP_CFM_EVT */
      BTA_AV_STR_OPEN_FAIL_EVT,    /* AVDT_OPEN_CFM_EVT */
      BTA_AV_STR_OPEN_OK_EVT,      /* AVDT_OPEN_IND_EVT */
      BTA_AV_STR_CONFIG_IND_EVT,   /* AVDT_CONFIG_IND_EVT */
      BTA_AV_STR_START_FAIL_EVT,   /* AVDT_START_CFM_EVT */
      BTA_AV_STR_START_OK_EVT,     /* AVDT_START_IND_EVT */
      BTA_AV_STR_SUSPEND_CFM_EVT,  /* AVDT_SUSPEND_CFM_EVT */
      BTA_AV_STR_SUSPEND_CFM_EVT,  /* AVDT_SUSPEND_IND_EVT */
      BTA_AV_STR_CLOSE_EVT,        /* AVDT_CLOSE_CFM_EVT */
      BTA_AV_STR_CLOSE_EVT,        /* AVDT_CLOSE_IND_EVT */
      BTA_AV_STR_RECONFIG_CFM_EVT, /* AVDT_RECONFIG_CFM_EVT */
      0,                           /* AVDT_RECONFIG_IND_EVT */
      BTA_AV_STR_SECURITY_CFM_EVT, /* AVDT_SECURITY_CFM_EVT */
      BTA_AV_STR_SECURITY_IND_EVT, /* AVDT_SECURITY_IND_EVT */
      BTA_AV_STR_WRITE_CFM_EVT,    /* AVDT_WRITE_CFM_EVT */
      BTA_AV_AVDT_CONNECT_EVT,     /* AVDT_CONNECT_IND_EVT */
      BTA_AV_AVDT_DISCONNECT_EVT,  /* AVDT_DISCONNECT_IND_EVT */
      BTA_AV_AVDT_RPT_CONN_EVT, /* AVDT_REPORT_CONN_EVT */
      BTA_AV_AVDT_RPT_CONN_EVT, /* AVDT_REPORT_DISCONN_EVT */
      BTA_AV_AVDT_DELAY_RPT_EVT, /* AVDT_DELAY_REPORT_EVT */
      BTA_AV_AVDT_DELAY_RPT_CFM_EVT, /* AVDT_DELAY_REPORT_CFM_EVT */
  };

const tAVDT_CCB_ACTION avdt_ccb_action[] = {
    avdt_ccb_chan_open,        avdt_ccb_chan_close,
    avdt_ccb_chk_close,        avdt_ccb_hdl_discover_cmd,
    avdt_ccb_hdl_discover_rsp, avdt_ccb_hdl_getcap_cmd,
    avdt_ccb_hdl_getcap_rsp,   avdt_ccb_hdl_start_cmd,
    avdt_ccb_hdl_start_rsp,    avdt_ccb_hdl_suspend_cmd, //9
    avdt_ccb_hdl_suspend_rsp,  avdt_ccb_snd_discover_cmd,
    avdt_ccb_snd_discover_rsp, avdt_ccb_snd_getcap_cmd,
    avdt_ccb_snd_getcap_rsp,   avdt_ccb_snd_start_cmd,
    avdt_ccb_snd_start_rsp,    avdt_ccb_snd_suspend_cmd,
    avdt_ccb_snd_suspend_rsp,  avdt_ccb_clear_cmds,  //19
    avdt_ccb_cmd_fail,         avdt_ccb_free_cmd,
    avdt_ccb_cong_state,       avdt_ccb_ret_cmd,
    avdt_ccb_snd_cmd,          avdt_ccb_snd_msg,
    avdt_ccb_set_reconn,       avdt_ccb_clr_reconn,
    avdt_ccb_chk_reconn,       avdt_ccb_chk_timer,//29
    avdt_ccb_set_conn,         avdt_ccb_set_disconn,
    avdt_ccb_do_disconn,       avdt_ccb_ll_closed,
    avdt_ccb_ll_opened,        avdt_ccb_dealloc};

  /* state table */
  const tAVDT_CCB_ST_TBL avdt_ccb_st_tbl[] = {
      avdt_ccb_st_idle, avdt_ccb_st_opening, avdt_ccb_st_open,
      avdt_ccb_st_closing};  
```

```
* mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/btif/src/btif_av.cc 
  * BtifAvStateMachine(BtifAvPeer& btif_av_peer) : peer_(btif_av_peer)
    * void BtifAvStateMachine::StateIdle::OnEnter()
      *  
```
关键日志状态切换
```
	Line 11559: 05-18 14:35:54.543096  6802  6856 I bluetooth: bta_av_better_stream_state_machine: [bta_av_better_stream_state_machine][56]state[0]event[4617]BTA_AV_API_DISABLE_EVT[4608]  //BTA_AV_API_OPEN_EVT
	Line 12122: 05-18 14:35:54.674456  6802  6856 I bluetooth: bta_av_better_stream_state_machine: [bta_av_better_stream_state_machine][56]state[2]event[4628]BTA_AV_API_DISABLE_EVT[4608] //BTA_AV_SDP_DISC_OK_EVT
	Line 12355: 05-18 14:35:54.705726  6802  6856 I bluetooth: bta_av_better_stream_state_machine: [bta_av_better_stream_state_machine][56]state[2]event[4646]BTA_AV_API_DISABLE_EVT[4608]  //BTA_AV_AVDT_CONNECT_EVT
	Line 12541: 05-18 14:35:54.747720  6802  6856 I bluetooth: bta_av_better_stream_state_machine: [bta_av_better_stream_state_machine][56]state[2]event[4630]BTA_AV_API_DISABLE_EVT[4608]  //BTA_AV_STR_DISC_OK_EVT
	Line 12702: 05-18 14:35:54.779947  6802  6856 I bluetooth: bta_av_better_stream_state_machine: [bta_av_better_stream_state_machine][56]state[2]event[4632]BTA_AV_API_DISABLE_EVT[4608]  // BTA_AV_STR_GETCAP_OK_EVT
	Line 12867: 05-18 14:35:54.872051  6802  6856 I bluetooth: bta_av_better_stream_state_machine: [bta_av_better_stream_state_machine][56]state[2]event[4632]BTA_AV_API_DISABLE_EVT[4608]  //BTA_AV_STR_GETCAP_OK_EVT
	Line 13326: 05-18 14:35:54.910809  6802  6856 I bluetooth: bta_av_better_stream_state_machine: [bta_av_better_stream_state_machine][56]state[2]event[4634]BTA_AV_API_DISABLE_EVT[4608]  //BTA_AV_STR_OPEN_OK_EVT
	Line 13590: 05-18 14:35:55.011367  6802  6856 I bluetooth: bta_av_better_stream_state_machine: [bta_av_better_stream_state_machine][56]state[3]event[4645]BTA_AV_API_DISABLE_EVT[4608] // BTA_AV_AVRC_TIMER_EVT


	Line 3666: 05-18 12:22:05.240969  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=API_CONNECT_REQ_EVT event=8 state=CCB_OPENING_ST action=30
	Line 3667: 05-18 12:22:05.240981  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=API_CONNECT_REQ_EVT event=8 state=CCB_OPENING_ST action=0
	Line 3936: 05-18 12:22:05.309359  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=LL_OPEN_EVT event=25 state=CCB_OPEN_ST action=24
	Line 3937: 05-18 12:22:05.309385  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=LL_OPEN_EVT event=25 state=CCB_OPEN_ST action=34
	Line 3940: 05-18 12:22:05.309435  7850  8023 I linls bt_bta_av: bta_av_proc_stream_evt: bta_av_proc_stream_evt: peer_address: 1c:52:16:86:66:3d avdt_handle: 0 event=0x10 scb_index=0 p_scb=0xb4000076e111bf18
	Line 3942: 05-18 12:22:05.309496  7850  8023 I linls bt_bta_av: bta_av_proc_stream_evt: [bta_av_proc_stream_evt][487]event[0x10]BTA_AV_AVDT_CONNECT_EVT[0x1226]
		Line 3952: 05-18 12:22:05.309706  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=API_DISCOVER_REQ_EVT event=0 state=CCB_OPEN_ST action=11
	Line 3954: 05-18 12:22:05.309759  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=SENDMSG_EVT event=19 state=CCB_OPEN_ST action=25
	Line 3967: 05-18 12:22:05.310046  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=SENDMSG_EVT event=19 state=CCB_OPEN_ST action=36
	Line 3968: 05-18 12:22:05.310057  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=API_DISCOVER_REQ_EVT event=0 state=CCB_OPEN_ST action=24
	Line 4072: 05-18 12:22:05.338693  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=MSG_DISCOVER_RSP_EVT event=14 state=CCB_OPEN_ST action=2
	Line 4073: 05-18 12:22:05.338703  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=MSG_DISCOVER_RSP_EVT event=14 state=CCB_OPEN_ST action=4
	Line 4075: 05-18 12:22:05.338741  7850  8023 I linls bt_bta_av: bta_av_proc_stream_evt: bta_av_proc_stream_evt: peer_address: 1c:52:16:86:66:3d avdt_handle: 0 event=0x0 scb_index=0 p_scb=0xb4000076e111bf18
	Line 4077: 05-18 12:22:05.338778  7850  8023 I linls bt_bta_av: bta_av_proc_stream_evt: [bta_av_proc_stream_evt][487]event[0x0]BTA_AV_AVDT_CONNECT_EVT[0x1226]
	Line 4082: 05-18 12:22:05.338890  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=RCVRSP_EVT event=18 state=CCB_OPEN_ST action=21
	Line 4084: 05-18 12:22:05.338914  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=RCVRSP_EVT event=18 state=CCB_OPEN_ST action=24
	Line 4097: 05-18 12:22:05.339150  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=API_GETCAP_REQ_EVT event=1 state=CCB_OPEN_ST action=13
	Line 4099: 05-18 12:22:05.339249  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=SENDMSG_EVT event=19 state=CCB_OPEN_ST action=25
	Line 4117: 05-18 12:22:05.339717  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=SENDMSG_EVT event=19 state=CCB_OPEN_ST action=36
	Line 4118: 05-18 12:22:05.339737  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=API_GETCAP_REQ_EVT event=1 state=CCB_OPEN_ST action=24
	Line 4200: 05-18 12:22:05.352748  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=MSG_GETCAP_RSP_EVT event=15 state=CCB_OPEN_ST action=2
	Line 4201: 05-18 12:22:05.352758  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=MSG_GETCAP_RSP_EVT event=15 state=CCB_OPEN_ST action=6
	Line 4203: 05-18 12:22:05.352804  7850  8023 I linls bt_bta_av: bta_av_proc_stream_evt: bta_av_proc_stream_evt: peer_address: 1c:52:16:86:66:3d avdt_handle: 0 event=0x1 scb_index=0 p_scb=0xb4000076e111bf18
	Line 4205: 05-18 12:22:05.352844  7850  8023 I linls bt_bta_av: bta_av_proc_stream_evt: [bta_av_proc_stream_evt][487]event[0x1]BTA_AV_AVDT_CONNECT_EVT[0x1226]
	Line 4209: 05-18 12:22:05.353003  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=RCVRSP_EVT event=18 state=CCB_OPEN_ST action=21
	Line 4210: 05-18 12:22:05.353017  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=RCVRSP_EVT event=18 state=CCB_OPEN_ST action=24
	Line 4244: 05-18 12:22:05.353520  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=API_GETCAP_REQ_EVT event=1 state=CCB_OPEN_ST action=13
	Line 4246: 05-18 12:22:05.353570  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=SENDMSG_EVT event=19 state=CCB_OPEN_ST action=25
	Line 4259: 05-18 12:22:05.353870  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=SENDMSG_EVT event=19 state=CCB_OPEN_ST action=36
	Line 4260: 05-18 12:22:05.353881  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=API_GETCAP_REQ_EVT event=1 state=CCB_OPEN_ST action=24
	Line 4402: 05-18 12:22:05.391713  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=MSG_GETCAP_RSP_EVT event=15 state=CCB_OPEN_ST action=2
	Line 4403: 05-18 12:22:05.391723  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=MSG_GETCAP_RSP_EVT event=15 state=CCB_OPEN_ST action=6
	Line 4410: 05-18 12:22:05.391878  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=RCVRSP_EVT event=18 state=CCB_OPEN_ST action=21
	Line 4405: 05-18 12:22:05.391767  7850  8023 I linls bt_bta_av: bta_av_proc_stream_evt: bta_av_proc_stream_evt: peer_address: 1c:52:16:86:66:3d avdt_handle: 0 event=0x1 scb_index=0 p_scb=0xb4000076e111bf18
	Line 4407: 05-18 12:22:05.391814  7850  8023 I linls bt_bta_av: bta_av_proc_stream_evt: [bta_av_proc_stream_evt][487]event[0x1]BTA_AV_AVDT_CONNECT_EVT[0x1226]
	Line 4411: 05-18 12:22:05.391890  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=RCVRSP_EVT event=18 state=CCB_OPEN_ST action=24
	Line 4514: 05-18 12:22:05.393731  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=SENDMSG_EVT event=19 state=CCB_OPEN_ST action=25
	Line 4527: 05-18 12:22:05.394006  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=SENDMSG_EVT event=19 state=CCB_OPEN_ST action=36
	Line 4532: 05-18 12:22:05.394140  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=UL_OPEN_EVT event=23 state=CCB_OPEN_ST action=29
	Line 4533: 05-18 12:22:05.394165  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=UL_OPEN_EVT event=23 state=CCB_OPEN_ST action=36
	Line 4665: 05-18 12:22:05.430266  7850  8023 I linls bt_bta_av: bta_av_proc_stream_evt: bta_av_proc_stream_evt: peer_address: 1c:52:16:86:66:3d avdt_handle: 2 event=0x16 scb_index=0 p_scb=0xb4000076e111bf18
	Line 4669: 05-18 12:22:05.430524  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=SENDMSG_EVT event=19 state=CCB_OPEN_ST action=25
	Line 4670: 05-18 12:22:05.430537  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=SENDMSG_EVT event=19 state=CCB_OPEN_ST action=36
	Line 4672: 05-18 12:22:05.430567  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=RCVRSP_EVT event=18 state=CCB_OPEN_ST action=21
	Line 4673: 05-18 12:22:05.430579  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=RCVRSP_EVT event=18 state=CCB_OPEN_ST action=24
	Line 4829: 05-18 12:22:05.440392  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=RCVRSP_EVT event=18 state=CCB_OPEN_ST action=21
	Line 4830: 05-18 12:22:05.440405  7850  8023 I bt_avp  : LogMsg: avdt_ccb_event: event=RCVRSP_EVT event=18 state=CCB_OPEN_ST action=24
	Line 4928: 05-18 12:22:05.452850  7850  8023 I linls bt_bta_av: bta_av_proc_stream_evt: bta_av_proc_stream_evt: peer_address: 1c:52:16:86:66:3d avdt_handle: 2 event=0x2 scb_index=0 p_scb=0xb4000076e111bf18
	Line 4930: 05-18 12:22:05.452887  7850  8023 I linls bt_bta_av: bta_av_proc_stream_evt: [bta_av_proc_stream_evt][487]event[0x2]BTA_AV_AVDT_CONNECT_EVT[0x1226] 
```

蓝牙广播监听：
```
蓝牙开关状态变化
BluetoothAdapter.ACTION_STATE_CHANGED

蓝牙设备配对状态变化
BluetoothDevice.ACTION_BOND_STATE_CHANGED
```

重启不能自动链接上蓝牙设备
```
05-20 03:03:15.068014  1250  1596 I bluetooth: system/bt/stack/rfcomm/rfc_mx_fsm.cc:79 rfc_mx_sm_execute: RFCOMM peer:xx:xx:xx:xx:66:3d event:6 state:idle
05-20 03:03:15.068088  1250  1596 I bt_btm_pm: system/bt/stack/acl/btm_pm.cc:245 BTM_SetPowerMode: Device is already in requested mode 0, interval: 0, max: 0, min: 0
05-20 03:03:15.082605   958  1379 E CountryCodeUtils: country code is null or empty
05-20 03:03:15.092003  1429  1429 D ControlsListingControllerImpl: Initializing
05-20 03:03:15.097784  1429  1568 D ControlsListingControllerImpl: Subscribing callback, service count: 0
05-20 03:03:15.099928  1250  1596 W bt_btm  : system/bt/main/bte_logmsg.cc:198 LogMsg: btm_io_capabilities_req: Incoming bond request, but 1c:52:16:86:66:3d is already bonded (removing)
05-20 03:03:15.099975  1250  1596 W bt_btm  : system/bt/main/bte_logmsg.cc:198 LogMsg: BTM_SecDeleteDevice FAILED: Cannot Delete when connection is active
05-20 03:03:15.099999  1250  1596 W bt_btif : system/bt/main/bte_logmsg.cc:198 LogMsg: Oops, can't find device 1c:52:16:86:66:3d 
05-20 03:03:15.100117  1250  1596 I bt_btif_storage: system/bt/btif/src/btif_storage.cc:881 btif_storage_remove_bonded_device: Removing bonded device addr:xx:xx:xx:xx:66:3d
05-20 03:03:15.100133  1250  1596 I bt_btif_storage: system/bt/btif/src/btif_storage.cc:1208 btif_storage_remove_ble_bonding_keys: Removing bonding keys for bd addr:xx:xx:xx:xx:66:3d
05-20 03:03:15.100792  1250  1596 I bt_btif_config: system/bt/btif/src/btif_config.cc:138 btif_get_device_type: Device [xx:xx:xx:xx:66:3d] device type 3
05-20 03:03:15.100852  1250  1596 I bt_btif_dm: system/bt/btif/src/btif_dm.cc:416 get_cod: get_cod remote_cod = 0x00240418
05-20 03:03:15.100869  1250  1596 I bt_btif_dm: system/bt/btif/src/btif_dm.cc:499 bond_state_changed: Bond state changed to state=0 [0:none, 1:bonding, 2:bonded], prev_state=0, sdp_attempts = 0
05-20 03:03:15.100914  1250  1596 I bt_btif_config: system/bt/btif/src/btif_config.cc:138 btif_get_device_type: Device [xx:xx:xx:xx:66:3d] device type 3
05-20 03:03:15.101165  1250  1596 E bt_stack: [ERROR:bta_gattc_utils.cc(441)] bta_gattc_mark_bg_conn unable to find the bg connection mask for bd_addr=1c:52:16:86:66:3d
05-20 03:03:15.101313  1250  1596 I bt_bta_sys_main: system/bt/bta/sys/bta_sys_main.cc:99 bta_sys_event: Ignoring receipt of unregistered event id:HID Human interface device
```