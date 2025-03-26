# *请确认索引缺失文件*

参考文档：[0002_bluetooth.md](0002_bluetooth.md)

# bluetooth performance analyse

测试反馈通知栏打开蓝牙比三星慢百分之五十

# mtk analyse

```
因为连线，往往与环境干扰，操作手法（比如开机后做BT连接，放置一段时间等module 都启动ready后发起BT 连接），是否开关wifi，
ble 设备还包括scan 参数，对端adv interval等. 甚至每次测量结果，都会有一定差异性, 
MTK FW 这边check，一般是以HCI 发送hci_create_connection  -> acl connection complete -> encryption complete.
而hci_create_connection  -> acl connection complete 就表示BT link以及建立，重点会check这个时间是否合理，一般在3s左右，FW认为都是正常范围，
```

# 蓝牙开启流程分析

参考文档：[0002_bluetooth.md](0002_bluetooth.md)
* [2019-03-11](https://www.jianshu.com/p/d892aa630752)

# 竞品比对

拿相同的安卓版本的设备进行比对，多耗时近700ms
```
本地测试日志：

03-30 04:12:15.298765  1292  4209 D BluetoothManagerService: enable(com.android.systemui):  mBluetooth =null mBinding = false mState = OFF
03-30 04:12:15.629264  1292  1292 D BluetoothManagerService: BluetoothServiceConnection: com.android.bluetooth.btservice.AdapterService
03-30 04:12:15.870805  1292  2987 D BluetoothManagerService: Sending State Change: OFF > TURNING_ON
03-30 04:12:16.266955  1292  2987 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: TURNING_ON > ON

竞品日志：
03-30 15:29:58.478  1862  7491 D BluetoothManagerService: enable(com.android.systemui):
03-30 15:29:58.484  1862  2340 D BluetoothManagerService: Sending State Change: OFF > TURNING_ON
03-30 15:29:58.709  1862  2340 D BluetoothManagerService: MESSAGE_BLUETOOTH_STATE_CHANGE: TURNING_ON > ON
```

