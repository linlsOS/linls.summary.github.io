# android bluth pause have noise

听音乐时单击Jabra Talk25蓝牙耳机有卡顿

# 参考文档

* [android蓝牙音乐之AVRCP介绍和使用](https://blog.csdn.net/Jason_Lee155/article/details/116426065)


# 日志分析

1、 复现问题，过滤audio、bluetooth均为发现异常
2、 过滤bt每次暂停均提示如下日志
```
在按暂停按键后，出现卡顿时有如下日志

03-24 14:03:29.424 14010 14067 W bt_btif_a2dp_source: btif_a2dp_source_enqueue_callback: btif_a2dp_source_enqueue_callback: TX queue buffer size now=22 adding=7 max=28
03-24 14:03:29.425 14010 14067 I bt_btm_ble: BTM_ReadDevInfo: Determining device_type:BR_EDR addr_type:public
03-24 14:03:29.425 14010 14067 I bt_btm_ble: BTM_ReadDevInfo: Determining device_type:BR_EDR addr_type:public
03-24 14:03:29.434 14010 14058 W bt_btif_a2dp_source: btm_read_rssi_cb: btm_read_rssi_cb: device: xx:xx:xx:xx:3a:6d, rssi: 0
03-24 14:03:29.435 14010 14058 W bt_btif_a2dp_source: btm_read_failed_contact_counter_cb: btm_read_failed_contact_counter_cb: device: xx:xx:xx:xx:3a:6d, Failed Contact Counter: 0
03-24 14:03:29.437 14010 14058 E bt_btif_a2dp_source: btm_read_tx_power_cb: btm_read_tx_power_cb: unable to read Tx Power (status 10)

```

# mtk 分析

```
基于目前的log分析root cause应该是jabra talk25耳机不支持avrcp协议所导致
荣耀30对比log：
04-03 09:12:25.323 32471 32528 I bt_stack: [INFO:connection_handler.cc(350)] void bluetooth::avrcp::ConnectionHandler::InitiatorControlCb(uint8_t, uint8_t, uint16_t, const RawAddress *): Connection Closed Event//无法与对端建立avrcp连线
 
耳机在按下暂停键后mtk测试机有两秒左右的卡顿后继续播放，应该是无法解析耳机端的命令所导致：
946 ACL Data 2023/3/30 22:50:56.623398 0x0032 Available Host's ACL credits: 2 First 612 617
947 ACL Data 2023/3/30 22:50:56.642674 0x0032 Available Host's ACL credits: 1 First 612 617 00:00:00.019276
984 Event HCI_Number_Of_Completed_Packets 2023/3/30 22:50:59.151573 0x0032 Available Host's ACL credits: 2 5 8 00:00:02.508899
985 ACL Data 2023/3/30 22:50:59.152314 0x0032 Available Host's ACL credits: 1 First 612 617 00:00:00.000741
 
所以本题应该是对端问题。
```