# bt hid iperformance

蓝牙hid 性能分析

# 问题描述

测试反馈连接蓝牙键盘同测试机比对差18.89%，测试方法从点击蓝牙提示框开始，到右上角显示蓝牙连接

# 日志分析
```
测试机器
Line 6854: 06-12 10:02:29.817487 12820 12820 D BTPairingController: Pairing dialog accepted
Line 6993: 06-12 10:02:30.472651  3102  3167 D MtkCachedBluetoothDevice: No profiles. Maybe we will connect later for device B4:EE:25:E6:BD:42
Line 7001: 06-12 10:02:30.479491 12820 12820 D MtkCachedBluetoothDevice: onBondingStateChanged bondState = 12
Line 7535: 06-12 10:02:31.926841 12820 12820 D MtkCachedBluetoothDevice: updating profiles for RAPOO BT4.0 KB       //2109ms
Line 7575: 06-12 10:02:31.953298  3102  3167 D MtkCachedBluetoothDevice: onProfileStateChanged: profile HID, device RAPOO BT4.0 KB, newProfileState 1//2136ms
Line 7587: 06-12 10:02:31.964510 12820 12820 D MtkCachedBluetoothDevice: onProfileStateChanged: profile HID, device RAPOO BT4.0 KB, newProfileState 1
Line 7727: 06-12 10:02:33.096943 12820 12820 D MtkCachedBluetoothDevice: onProfileStateChanged: profile HID, device RAPOO BT4.0 KB, newProfileState 2
Line 7729: 06-12 10:02:33.101634  3102  3167 D MtkCachedBluetoothDevice: onProfileStateChanged: profile HID, device RAPOO BT4.0 KB, newProfileState 2//3284ms


对比机器
06-01 06:21:08.227992 14403 14403 D BTPairingController: Pairing dialog accepted
06-01 06:21:08.689686  1092  1326 D CachedBluetoothDevice: No profiles. Maybe we will connect later for device B4:EE:25:E6:9D:42 //462ms
06-01 06:21:09.623705 14403 14403 D CachedBluetoothDevice: updating profiles for RAPOO BT4.0 KB//1396ms
06-01 06:21:09.639760  1092  1326 D CachedBluetoothDevice: onProfileStateChanged: profile HID, device RAPOO BT4.0 KB, newProfileState 1//1412ms
06-01 06:21:09.670930 14403 14403 D CachedBluetoothDevice: onProfileStateChanged: profile HID, device RAPOO BT4.0 KB, newProfileState 1
06-01 06:21:10.986399 14403 14403 D CachedBluetoothDevice: onProfileStateChanged: profile HID, device RAPOO BT4.0 KB, newProfileState 2
06-01 06:21:10.995350  1092  1326 D CachedBluetoothDevice: onProfileStateChanged: profile HID, device RAPOO BT4.0 KB, newProfileState 2//2768ms

```

日志过滤
```
logcat -v threadtime |grep -E "BTPairingController|onProfileStateChanged" 
```

# 代码堆栈

```
* mssi/vendor/mediatek/proprietary/packages/apps/MtkSettings/src/com/android/settings/bluetooth/BluetoothPairingController.java
  * private void onPair(String passkey)
    * mDevice.setPin(passkey);
      * mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/framework/java/android/bluetooth/BluetoothDevice.java 
        * public boolean setPin(byte[] pin)
          * service.setPin(this, true, pin.length, pin, mAttributionSource, recv);
            * mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/btservice/AdapterService.java
              * public void setPin(BluetoothDevice device, boolean accept, int len, byte[] pinCode, AttributionSource source, SynchronousResultReceiver receiver)
                * receiver.send(setPin(device, accept, len, pinCode, source));
                  * private boolean setPin(BluetoothDevice device, boolean accept, int len, byte[] pinCode, AttributionSource attributionSource)
                    * service.pinReplyNative(getBytesFromAddress(device.getAddress()), accept, len, pinCode);
                      * mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/jni/com_android_bluetooth_btservice_AdapterService.cpp
                        * static jboolean pinReplyNative(JNIEnv* env, jobject obj, jbyteArray address, jboolean accept, jint len, jbyteArray pinArray)
                          * int ret = sBluetoothInterface->pin_reply((RawAddress*)addr, accept, len, (bt_pin_code_t*)pinPtr);
                            * mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/btif/src/bluetooth.cc
                              * static int pin_reply(const RawAddress* bd_addr, uint8_t accept, uint8_t pin_len,bt_pin_code_t* pin_code)
                                * mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/btif/src/btif_dm.cc
                                  * void btif_dm_pin_reply(const RawAddress bd_addr, uint8_t accept, uint8_t pin_len, bt_pin_code_t pin_code)
                                    * BTA_DmBlePasskeyReply(bd_addr, accept, passkey);
                                    * BTA_DmPinReply(bd_addr, accept, pin_len, pin_code.pin);
                                      * do_in_main_thread(FROM_HERE, base::Bind(bta_dm_pin_reply, base::Passed(&msg)));
                                        * mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/bta/dm/bta_dm_act.cc
                                          * void bta_dm_pin_reply(std::unique_ptr<tBTA_DM_API_PIN_REPLY> msg) 
                                            * BTM_PINCodeReply(msg->bd_addr, BTM_SUCCESS, msg->pin_len, msg->p_pin);
                                            * BTM_PINCodeReply(msg->bd_addr, BTM_NOT_AUTHORIZED, 0, NULL);
```

# bt 连接监听广播

```java
public class BluetoothReciever extends BoradcastReceiver{
  @Override
  public void onReceive(Context context, Intent intent){
    String action = intent.getAction();
    BluetoothDevice device;
    if(BluetoothDevice.ACTION_FOUND.equals(action)){
      device = intent.getParcelabeExtra(BluetoothDevice.EXTRA_DEVICE);
    }else if(BluetoothDevice.ACTION_BOND_STATE_CHANGED.equals(action)){
      device = intent.getParcelabeExtra(BluetoothDevice.EXTRA_DEVICE);
      switch(device.getBondState()){
        case BluetoothDevice.BOND_BONDING://正在配对
          break;
        case BluetoothDevice.BOND_BONDED: //配对结束
          break;
        case BluetoothDevice.BOND_NONE: //取消配对
          break;

        default:
          break;
      }
    }
  }
}

//初始化广播接收者
mBroadCastReceiver = new BluetoothReciever();
IntentFilter intentFilter = new IntentFilter();
in
```

```
06-29 11:22:50.747988  6478  6478 W System.err: java.lang.Exception: onProfileStateChanged
06-29 11:22:50.748213  6478  6478 W System.err: 	at com.android.mtksettingslib.bluetooth.CachedBluetoothDevice.onProfileStateChanged(CachedBluetoothDevice.java:190)
06-29 11:22:50.748248  6478  6478 W System.err: 	at com.android.mtksettingslib.bluetooth.LocalBluetoothProfileManager$StateChangedHandler.onReceiveInternal(LocalBluetoothProfileManager.java:317)
06-29 11:22:50.748270  6478  6478 W System.err: 	at com.android.mtksettingslib.bluetooth.LocalBluetoothProfileManager$StateChangedHandler.onReceive(LocalBluetoothProfileManager.java:296)
06-29 11:22:50.748297  6478  6478 W System.err: 	at com.android.mtksettingslib.bluetooth.BluetoothEventManager$BluetoothBroadcastReceiver.onReceive(BluetoothEventManager.java:287)
06-29 11:22:50.748317  6478  6478 W System.err: 	at android.app.LoadedApk$ReceiverDispatcher$Args.lambda$getRunnable$0$LoadedApk$ReceiverDispatcher$Args(LoadedApk.java:1697)
06-29 11:22:50.748338  6478  6478 W System.err: 	at android.app.LoadedApk$ReceiverDispatcher$Args$$ExternalSyntheticLambda0.run(Unknown Source:2)
06-29 11:22:50.748370  6478  6478 W System.err: 	at android.os.Handler.handleCallback(Handler.java:938)
06-29 11:22:50.748388  6478  6478 W System.err: 	at android.os.Handler.dispatchMessage(Handler.java:99)
06-29 11:22:50.748404  6478  6478 W System.err: 	at android.os.Looper.loopOnce(Looper.java:201)
06-29 11:22:50.748423  6478  6478 W System.err: 	at android.os.Looper.loop(Looper.java:288)
06-29 11:22:50.748441  6478  6478 W System.err: 	at android.app.ActivityThread.main(ActivityThread.java:7889)
06-29 11:22:50.748458  6478  6478 W System.err: 	at java.lang.reflect.Method.invoke(Native Method)
06-29 11:22:50.748484  6478  6478 W System.err: 	at com.android.internal.os.RuntimeInit$MethodAndArgsCaller.run(RuntimeInit.java:568)
06-29 11:22:50.748503  6478  6478 W System.err: 	at com.android.internal.os.ZygoteInit.main(ZygoteInit.java:1054)
```

# 结论
这里存在测试取点的差异，对比机器比测试机器快，是测试机器先改变了蓝牙设备连接成功的图标（2.8s），然后600ms后显示HID设备连接成功的图标，整体耗时约为3400ms；

422s测试机器是蓝牙连接成功图片与HID设备连接成功的图标两个显示时间前后差约为50ms，所以HID设备整体连接成功的时间差异不大。从如上分析，从点击蓝牙确认连接按钮到上报蓝牙设备连接成功耗时相近。
