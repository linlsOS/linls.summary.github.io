# avrcp

AVRCP介绍及代码流程

# 参考文档

* [android蓝牙音乐之AVRCP介绍和使用](https://blog.csdn.net/Jason_Lee155/article/details/116426065)
* [【经典蓝牙】蓝牙AVRCP协议分析](https://blog.csdn.net/hesuping/article/details/129224939)
* [蓝牙AVRCP协议解析](https://blog.csdn.net/william198757/article/details/52037159)
* [蓝牙协议 HFP,HSP,A2DP,AVRCP,OPP,PBAP](https://blog.csdn.net/peng825223208/article/details/52072680?spm=1001.2101.3001.6650.1&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7ERate-1-52072680-blog-52037159.235%5Ev36%5Epc_relevant_yljh&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7ERate-1-52072680-blog-52037159.235%5Ev36%5Epc_relevant_yljh&utm_relevant_index=2)

# AVRCP简介 

* AVRCP：Audio/Video Remote Control Profile。音视频远端控制协议，所以该协议不但能控制蓝牙音乐，也可以控制视频流相应的功能  
常见的使用到AVRCP控制功能的场景有如下几种常：
  * 耳机或车载等蓝牙设备控制其他设备上的音乐音源播放（手机）
  * 远端设备控制其他设备上的视频播放
  * 远端设备控制手机拍照（手机前台应用为照相机）

# AVRCP连接
  
* AVCTP 的连接分为两个通道： Control 通道跟 Brwoing 通道。 ,对应的 L2CAP PSM 不同， control通道的 PSM 为 0x0017,AVCTP browing 通道的 PSM 为 0x001B。 两个通道 controller 跟 target 角色都可以发起连接。

# AVRCP指令

蓝牙AVRCP的命令分为以下几种：
* AV/C 指令码
AV/C 指令码是 AV/C通用规范定义的指令，AV/C指令码又细分为以下几种：
  * UNIT INFO 指令： 获取COMPAND_ID指令，对端返回值为固定值，表示蓝牙的COMPAND_ID
  * SUBUNIT INFO 指令：获取SUBUNIT INFO， 返回值通常为固定值，表示pass through指令的类型。
  * VENDOR DEPENDENT 指令（重要）：蓝牙技术联盟定义的指令，包括控制指令、状态指令、通知指令等，使用的是AV/C设备模型，这些消息是按照AV/C数字接口命令集的规定进行发送的，消息是在AVCTP上进行传输。 比如蓝牙耳机获取手机音乐的播放状态，播放事件通知，调节手机音乐的均衡器模式， 播放循环模式等，这些都是VENDOR DEPENDENT 指令。
  * PASS THROUGH 指令（重要）： 用来控制对端蓝牙的音乐播放状态，例如常用的蓝牙耳机控制手机音乐播放、暂停、上一曲、下一曲等指令都是PASS THROUGH 指令。

* 浏览指令：浏览功能是用来导航对端蓝牙的音乐列表，并控制特定的媒体播放器，使用的是AVCTP的第二通道进行传输的。
* 封面指令：蓝牙AVRCP 1.6协议支持了传输音乐封面图片，使用的BIP协议进行传输，下层通道使用的是OBEX通道
* PASS THROUGH 指令PASS THROUGH指令指令是AV/C标准指令， 用来控制对端蓝牙设备的音乐播放状态。 常用的控制音乐的播放、暂停、上一曲、下一曲等命令都是通过PASS THROUGH指令下发的

# AVCTP

AVCTP（Audio/Video Control Transport Protocol Specification）音频/视频控制传输协议是一个框架协议，描述蓝牙设备间Audio/Video的控制信号交换的传输机制，具体的控制信息（编码和格式）由其指定的协议(如AVRCP)实现，AVCTP本身只指定控制command和response的总体的格式,AVCTP是建立在L2CAP上的；

AVCTP分为两个角色，CT（The Controller）是命令的发起方，命令接收方是TG（The target）

# 代码堆栈

```
* mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/src/com/android/bluetooth/avrcp/AvrcpTargetService.java
  * protected boolean start()
    * mAudioManager = getSystemService(AudioManager.class);
    * mMediaPlayerList = new MediaPlayerList(Looper.myLooper(), this);
    * mNativeInterface = AvrcpNativeInterface.getInterface();
    * mNativeInterface.init(AvrcpTargetService.this);

* mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/jni/com_android_bluetooth_btservice_AdapterService.cpp 
  * static void classInitNative(JNIEnv* env, jclass clazz) 
    * 
```