# bt keyevent

蓝牙按键事件处理

# menu

* [1 参考文档](#1-参考文档)
* [2 代码堆栈](#2-代码堆栈)
  * [2.1 a2dp](#21-a2dp)
  * [2.2 hid](#22-hid)

# 1 参考文档

* [Android-普通按键和蓝牙耳机按键处理流程详解](https://blog.csdn.net/cheriyou_/article/details/114898482)
* [android蓝牙音乐之AVRCP介绍和使用](https://blog.csdn.net/Jason_Lee155/article/details/116426065?spm=1001.2101.3001.6661.1&utm_medium=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7ERate-1-116426065-blog-114898482.235%5Ev38%5Epc_relevant_yljh&depth_1-utm_source=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7ERate-1-116426065-blog-114898482.235%5Ev38%5Epc_relevant_yljh&utm_relevant_index=1)
* [蓝牙AVRCP协议分析](https://blog.csdn.net/bin_linux96/article/details/88955830?spm=1001.2101.3001.6650.1&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7ERate-1-88955830-blog-114898482.235%5Ev38%5Epc_relevant_yljh&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7ERate-1-88955830-blog-114898482.235%5Ev38%5Epc_relevant_yljh&utm_relevant_index=2)
* [蓝牙耳机按键在Android侧的处理流程](https://blog.csdn.net/feelinghappy/article/details/98652657)

# 2 代码堆栈

## 2.1 a2dp

```
* mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/stack/avct/avct_lcb.cc
  └── (*avct_lcb_action[action])(p_lcb, p_data);
      └── avct_lcb_msg_ind
          └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/stack/avct/avct_lcb_act.cc
              └── void avct_lcb_msg_ind(tAVCT_LCB* p_lcb, tAVCT_LCB_EVT* p_data) 
                  └── (*p_ccb->cc.p_msg_cback)(avct_ccb_to_idx(p_ccb), label, cr_ipid,p_data->p_buf);
                      └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/stack/avrc/avrc_api.cc 
                          └── uint16_t AVRC_Open(uint8_t* p_handle, tAVRC_CONN_CB* p_ccb,
                              └── cc.p_msg_cback = avrc_msg_cback; 
                                  └── static void avrc_msg_cback(uint8_t handle, uint8_t label, uint8_t cr,
                                      └── avrc_cb.ccb[handle].msg_cback.Run(handle, label, opcode, &msg);
                                          └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/profile/avrcp/connection_handler.cc 
                                              └── void ConnectionHandler::MessageCb(uint8_t handle, uint8_t label, uint8_t opcode,
                                                  └── device_map_[handle]->MessageReceived(label, Packet::Parse(pkt));
                                                      └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/profile/avrcp/device.cc 
                                                          └── void Device::MessageReceived(uint8_t label, std::shared_ptr<Packet> pkt)
                                                              └── case Opcode::VENDOR
                                                                  └── VendorPacketHandler(label, vendor_pkt);
                                                                      └── case CommandPdu::REGISTER_NOTIFICATION: 
                                                                          └── HandleVolumeChanged(label, register_notification);
                                                                              └── volume_interface_->SetVolume(volume_);
                                                                                  └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/btif/avrcp/avrcp_service.cc
                                                                                      └── void SetVolume(int8_t volume) override 
                                                                                          └── do_in_avrcp_jni(base::Bind(&VolumeInterface::SetVolume, base::Unretained(wrapped_), volume));
                                                                                              └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/jni/com_android_bluetooth_avrcp_target.cpp
                                                                                                  └── static void setVolume(int8_t volume) 
                                                                                                      └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/avrcp/AvrcpNativeInterface.java 
                                                                                                          └── void setVolume(int volume)
                                                                                                              └── mAvrcpService.setVolume(volume);
                                                                                                                  └── mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/avrcp/AvrcpTargetService.java 
                                                                                                                      └── mVolumeManager.setVolume(activeDevice, avrcpVolume);
```

## 2.2 hid

```
* mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/stack/l2cap/l2c_csm.cc
  * void l2c_csm_execute(tL2C_CCB* p_ccb, tL2CEVT event, void* p_data)
    * l2c_csm_open(p_ccb, event, p_data);
      * (*p_ccb->p_rcb->api.pL2CA_DataInd_Cb)(p_ccb->local_cid,(BT_HDR*)p_data); 
        * mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/stack/hid/hidh_conn.cc 
          * .pL2CA_DataInd_Cb = hidh_l2cif_data_ind,
            * static void hidh_l2cif_data_ind(uint16_t l2cap_cid, BT_HDR* p_msg) 
              *  evt = (hh_cb.devices[dhandle].conn.intr_cid == l2cap_cid)? HID_HDEV_EVT_INTR_DATA: HID_HDEV_EVT_CTRL_DATA;
              * hh_cb.callback(dhandle, hh_cb.devices[dhandle].addr, evt, rep_type, p_msg);
                * 
```

# 日志

```
07-04 18:45:22.129410 13257 13310 I bt_l2cap: LogMsg: Entry chnl_state=CST_OPEN [6], event=PEER_DATA [20]
07-04 18:45:22.129588 13257 13310 I bt_l2cap: LogMsg: LCID: 0x0041  st: OPEN  evt: PEER_DATA
07-04 18:45:22.129666 13257 13310 I bt_hid  : LogMsg: HID-Host hidh_l2cif_data_ind [l2cap_cid=0x0041]
07-04 18:45:22.129737 13257 13310 I bt_btif : LogMsg: bta_hh_cback::HID_event [HID_HDEV_EVT_INTR_DATA]
07-04 18:45:22.129857 13257 13310 I bt_l2cap: LogMsg: Exit chnl_state=CST_OPEN [6], event=PEER_DATA [20]
07-04 18:45:22.129941 13257 13310 I bt_btif : LogMsg: bta_sys_event: Event 0x1704
07-04 18:45:22.129996 13257 13310 I bt_btif : LogMsg: bta_hh_hdl_event:: handle = 0 dev_cb[0] 
07-04 18:45:22.130045 13257 13310 I bt_btif : LogMsg: bta_hh_sm_execute: State 0x03 [BTA_HH_CONN_ST], Event [BTA_HH_INT_DATA_EVT]
07-04 18:45:22.130099 13257 13310 I bt_btif : LogMsg: bta_hh_co_data: dev_handle = 0, subclass = 0x40, mode = 0, ctry_code = 0, app_id = 2
07-04 18:45:22.130147 13257 13310 I bt_btif : LogMsg: bta_hh_co_write: UHID write 10
07-04 18:45:22.130858  1285  2945 D WindowManager: KeyEvent:46
```