# bt headset have internet access

423海外产测客户反馈在连接手机后进入网络打开蓝牙热点，然后进行配对蓝牙耳机，配对后蓝牙耳机会显示互联网连接  
本地对问题进行复现，不复现

# menu

* [1 MTK 回复](#1-MTK-回复)
  * [1.1 代码逻辑](#11-代码逻辑)
  * [1.2 日志分析](#12-日志分析)
  * [1.3 如果设备支持pan，则在连接过程有如下日志](#13-如果设备支持pan则在连接过程有如下日志)
* [2 代码堆栈](#2-代码堆栈)
  * [2.1 stack](#21-stack)

# 1 MTK 回复

## 1.1 代码逻辑

```c
          if (mPanProfile == null && supportedList.contains(BluetoothProfile.PAN)) {
              if (DEBUG) Log.d(TAG, "Adding local PAN profile");
              mPanProfile = new PanProfile(mContext);
              addPanProfile(mPanProfile, PanProfile.NAME,
                      BluetoothPan.ACTION_CONNECTION_STATE_CHANGED);
          }
```
从贵司提供的Log看起来是有连接的

## 1.2 日志分析

```
06-29 13:03:52.261857  1467  1467 D BluetoothPan: Proxy object connected
06-29 13:03:52.274000  7961  7961 D BluetoothPanServiceJni: initializeNative(L113): pan
06-29 13:03:52.274098  7961  7961 I bt_btif : get_profile_interface: get_profile_interface: id = pan

Line 72735: 06-28 13:05:29.469017 12340 12340 V BluetoothDatabase: getProfileConnectionPolicy: device XX:XX:XX:47:F0:AC profile=PAN, connectionPolicy=-1
Line 72782: 06-28 13:05:29.516035  1441  1441 D LocalBluetoothProfileManager: New Profiles[HEADSET, A2DP, OPP, PAN]
Line 72805: 06-28 13:05:29.527981  1709  1846 D LocalBluetoothProfileManager: New Profiles[HEADSET, A2DP, OPP, PAN]
Line 72874: 06-28 13:05:29.593541 12340 12367 V BluetoothDatabase: getProfileConnectionPolicy: device XX:XX:XX:47:F0:AC profile=PAN, connectionPolicy=-1
Line 72978: 06-28 13:05:29.636880 12340 28576 V BluetoothDatabase: getProfileConnectionPolicy: device XX:XX:XX:47:F0:AC profile=PAN, connectionPolicy=-1
Line 76192: 06-28 13:05:33.649883  1441  1441 D BtDetailsProfilesCtrl: getProfiles: PAN add device first time XX:XX:XX:47:F0:AC
Line 76199: 06-28 13:05:33.663930  1441  1441 D BtDetailsProfilesCtrl: getProfiles:result:[HEADSET, A2DP, PAN, PBAP Server]
```

## 1.3 如果设备支持pan，则在连接过程有如下日志

```
06-08 16:10:47.740  8505  8505 D BluetoothPanServiceJni: connectPanNative(L175): in
06-08 16:10:47.741  8505  8567 D BluetoothPanServiceJni: connection_state_callback(L79): state:1, local_role:2, remote_role:1
06-08 16:10:47.868  8505  8567 D BluetoothPanServiceJni: connection_state_callback(L79): state:0, local_role:2, remote_role:1
06-08 16:10:47.980  8505  8567 D BluetoothPanServiceJni: connection_state_callback(L79): state:2, local_role:2, remote_role:1
06-08 16:10:48.035  8505  8505 D BluetoothPanServiceJni: connectPanNative(L175): in
06-08 16:10:48.036  8505  8567 D BluetoothPanServiceJni: connection_state_callback(L79): state:1, local_role:2, remote_role:1
06-08 16:10:48.076  8505  8567 D BluetoothPanServiceJni: connection_state_callback(L79): state:0, local_role:2, remote_role:1
06-08 16:10:48.144  8505  8567 D BluetoothPanServiceJni: connection_state_callback(L79): state:2, local_role:2, remote_role:1
06-08 16:10:48.400  8505  8505 D BluetoothPanServiceJni: connectPanNative(L175): in
06-08 16:10:48.401  8505  8567 D BluetoothPanServiceJni: connection_state_callback(L79): state:1, local_role:2, remote_role:1
06-08 16:10:48.428  8505  8567 D BluetoothPanServiceJni: connection_state_callback(L79): state:0, local_role:2, remote_role:1
06-08 16:10:48.488  8505  8567 D BluetoothPanServiceJni: connection_state_callback(L79): state:2, local_role:2, remote_role:1
06-08 16:10:48.576  8505  8505 D BluetoothPanServiceJni: connectPanNative(L175): in
06-08 16:10:48.577  8505  8567 D BluetoothPanServiceJni: connection_state_callback(L79): state:1, local_role:2, remote_role:1
06-08 16:10:48.631  8505  8567 D BluetoothPanServiceJni: connection_state_callback(L79): state:0, local_role:2, remote_role:1
06-08 16:10:48.708  8505  8567 D BluetoothPanServiceJni: connection_state_callback(L79): state:2, local_role:2, remote_role:1
```

# 2 代码堆栈

## 2.1 stack

```
* mssi/packages/modules/Connectivity/Tethering/src/com/android/networkstack/tethering/Tethering.java
  * private void enableTetheringInternal(int type, boolean enable, final IIntResultListener listener)
    * setBluetoothTethering(enable, listener);
      * setBluetoothTetheringSettings(mBluetoothPan, enable, listener);
        * if (SdkLevel.isAtLeastT())
          * changeBluetoothTetheringSettings(bluetoothPan, enable);
        * else 
          * changeBluetoothTetheringSettingsPreT(bluetoothPan, enable);
      * mPendingPanRequests.add(new Pair(enable, listener));

```

## 是否显示网络连接

```
* mssi/vendor/mediatek/proprietary/packages/apps/MtkSettings/src/com/android/settings/bluetooth/BluetoothDetailsProfilesController.java 
  * protected void refresh()
    * for (LocalBluetoothProfile profile : getProfiles())
      *  
    * SwitchPreference pref = mProfilesContainer.findPreference(profile.toString());
      * if (pref == null) 
        * pref = createProfilePreference(mProfilesContainer.getContext(), profile);
        *  mProfilesContainer.addPreference(pref);
        * maybeAddHighQualityAudioPref(profile);
      * else 
        * Log.d(TAG," setvisible "+ profile.toString());
        *  pref.setVisible(true);
```

## 如何判断是否支持pan 

```
* mssi/frameworks/base/packages/SettingsLib/src/com/android/settingslib/bluetooth/LocalBluetoothProfileManager.java
  * synchronized void updateProfiles(ParcelUuid[] uuids, ParcelUuid[] localUuids,
    * if ((ArrayUtils.contains(uuids, BluetoothUuid.NAP) && mPanProfile != null)
```

## 获取bt支持的profile

```
mssi/frameworks/base/packages/SettingsLib/src/com/android/settingslib/bluetooth/BluetoothEventManager.java
  * private class BluetoothBroadcastReceiver extends BroadcastReceiver
    * handler.onReceive(context, intent, device);
      * mLocalAdapter.setBluetoothStateInt(state);
        * mssi/frameworks/base/packages/SettingsLib/src/com/android/settingslib/bluetooth/LocalBluetoothAdapter.java
          * void setBluetoothStateInt(int state)
            * mProfileManager.setBluetoothStateOn();
              * mssi/frameworks/base/packages/SettingsLib/src/com/android/settingslib/bluetooth/LocalBluetoothProfileManager.java
                * void setBluetoothStateOn() 
                  * updateLocalProfiles();                  
                  * mEventManager.readPairedDevices();
```

## 代码堆栈 由c层回调，java层进行广播分发

```
* vendor/mediatek/proprietary/packages/apps/Bluetooth/src/com/android/bluetooth/btservice/JniCallbacks.java
  * void devicePropertyChangedCallback(byte[] address, int[] types, byte[][] val) 
    * vendor/mediatek/proprietary/packages/apps/Bluetooth/src/com/android/bluetooth/btservice/RemoteDevices.java
      * void devicePropertyChangedCallback(byte[] address, int[] types, byte[][] values) 
        * case AbstractionLayer.BT_PROPERTY_UUIDS:
          * sAdapterService.deviceUuidUpdated(bdDevice);
            * Message msg = mBondStateMachine.obtainMessage(BondStateMachine.UUID_UPDATE);
            * mBondStateMachine.sendMessage(msg);
              * vendor/mediatek/proprietary/packages/apps/Bluetooth/src/com/android/bluetooth/btservice/BondStateMachine.java 
                * private class StableState extends State
                  * public synchronized boolean processMessage(Message msg) 
                    * case UUID_UPDATE:
                      * if (mPendingBondedDevices.contains(dev))
                        * sendIntent(dev, BluetoothDevice.BOND_BONDED, 0);
          * sendUuidIntent(bdDevice, device);
            * Intent intent = new Intent(BluetoothDevice.ACTION_UUID);//发送BluetoothDevice.ACTION_UUID
            * sAdapterService.sendBroadcast(intent, BLUETOOTH_CONNECT, Utils.getTempAllowlistBroadcastOptions());
              * mssi/frameworks/base/packages/SettingsLib/src/com/android/settingslib/bluetooth/BluetoothEventManager.java 
                * private class BluetoothBroadcastReceiver extends BroadcastReceiver
                  * handler.onReceive(context, intent, device);
                    * private class UuidChangedHandler implements Handler
                      * cachedDevice.onUuidChanged();
                        * mssi/frameworks/base/packages/SettingsLib/src/com/android/settingslib/bluetooth/CachedBluetoothDevice.java 
                          * void onUuidChanged() 
                            * updateProfiles();
                              * mProfileManager.updateProfiles(uuids, localUuids, mProfiles, mRemovedProfiles,mLocalNapRoleConnected, mDevice);
                                * mssi/frameworks/base/packages/SettingsLib/src/com/android/settingslib/bluetooth/LocalBluetoothProfileManager.java
                                  * synchronized void updateProfiles
```
