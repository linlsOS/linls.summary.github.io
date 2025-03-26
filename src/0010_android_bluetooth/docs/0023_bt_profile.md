# bt profile

蓝牙协议介绍

# 参考文档


* [吐血推荐历史最全的蓝牙协议栈介绍](https://blog.csdn.net/XiaoXiaoPengBo/article/details/107466841)
* [蓝牙（三）蓝牙协议的初始化](https://blog.csdn.net/pashanhu6402/article/details/79956997)
* [对于蓝牙Profile的理解](https://www.jianshu.com/p/f96a871c9ebc)


第四篇:传统蓝牙host介绍，主要介绍传统蓝牙的协议栈，比如HCI,L2CAP,SDP,RFCOMM,HFP,SPP,HID,AVDTP,AVCTP,A2DP,AVRCP,OBEX,PBAP,MAP等等一系列的协议吧。  
第六篇：低功耗蓝牙host介绍，低功耗蓝牙协议栈的介绍，包括HCI,L2CAP,ATT,GATT,SM等  


All Bluetooth Products shall be one of the following:
• Bluetooth End Product                       //蓝牙终端产品  
• Bluetooth Host Subsystem Product            //蓝牙主机子系统产品  
• Bluetooth Controller Subsystem Product      //蓝牙控制器子系统产品  
• Bluetooth Profile Subsystem Product         //蓝牙配置文件子系统产品  
• Bluetooth Component Product                 //蓝牙组件产品  
• Bluetooth Development Tool                  //蓝牙开发工具  
• Bluetooth Test Equipment.                   //蓝牙测试设备  

BR CC:                           Bluetooth Basic Rate Core Configuration  
EDR CC:                          Bluetooth Enhanced Data Rate Core Configuration   
HS CC:                           High Speed Bluetooth Core Configuration  
LE CC:                           Bluetooth Low Energy Core Configuration   
BR and LE Combined CC:           Bluetooth Basic Rate and Low Energy Combined Core Configuration  
HCI CC:                          Host Controller Interface Core Configuration   

Generic Attribute Profile  GATT is used on LE devices for LE profile service discovery  
Generic Access Profile  GAP services include device discovery, connection modes, security, authentication, association models and service discovery.

SCO    Synchronous Connection-Oriented 

HCI Command Packet: host 发给controller,主要发送HCI命令，注意是命令，不是数据；  
HCI Event Packet: controller 发送给host,对应于command packet  
HCI ACL Data Packet: host 发给controller或者相反，主要是L2CAP发送或者接受的数据，我们上层的所有数据，注意是数据而不是命令都是通过这个type来传递的  
HCI Synchronous Data Packet: 用来传输语音（SCO）的数据，注意一般都会通过PCM接口来传递SCO数据。  

* HFP  
  HFP(Hands-freeProfile)，让蓝牙设备可以控制电话，如接听、挂断、拒接、语音拨号等，拒接、语音拨号要视蓝牙耳机及电话是否支持。  

* HSP  
  HSP 描述了Bluetooth 耳机如何与计算机或其它Bluetooth 设备（如手机）通信。连接和配置好后，耳机可以作为远程设备的音频输入和输出接口。这是最常用的配置，为当前流行支持蓝牙耳机与移动电话使用。它依赖于在64千比特编码的音频/s的CVSD的或PCM以及AT命令从GSM07.07的一个子集，包括环的能力最小的控制，接听来电，挂断以及音量调整。典型的使用情景是使用无线耳机与手机进行连接。可能会使用HSP的若干设备类型：耳机、手机、PDA、个人电脑、手提电脑。  

* A2DP  
  A2DP全名是AdvancedAudio Distribution Profile蓝牙音频传输模型协定！A2DP是能够采用耳机内的芯片来堆栈数据，达到声音的高清晰度。有A2DP的耳机就是蓝牙立体声耳机。声音能达到44.1kHz，一般的耳机只能达到8kHz。如果手机支持蓝牙，只要装载A2DP协议，就能使用A2DP耳机了。还有消费者看到技术参数提到蓝牙V1.0V1.1 V1.2 V2.0——这些是指蓝牙的技术版本，是指通过蓝牙传输的速度，他们是否支持A2DP具体要看蓝牙产品制造商是否使用这个技术  

* AVRCP  
  AVRCP（Audio/VideoRemote Control Profile），也就是音频/视频远程控制规范。AVRCP设计用于提供控制TV、Hi-Fi设备等的标准接口。此配置文件用于许可单个远程控制设备（或其它设备）控制所有用户可以接入的A/V设备。它可以与A2DP或VDP配合使用。AVRCP定义了如何控制流媒体的特征。包括暂停、停止、启动重放、音量控制及其它类型的远程控制操作。AVRCP定义了两个角色，即控制器和目标设备。控制器通常为远程控制设备，而目标设备为特征可以更改的设备。在AVRCP中，控制器将检测到的用户操作翻译为A/V控制信号，然后再将其传输至远程Bluetooth设备。对于“随身听”类型的媒体播放器，控制设备可以是允许跳过音轨的耳机，而目标设备则是实际的播放器。常规红外遥控器的可用功能可以在此协议中实现。AVRCP协议规定了AV/C数字接口命令集（AV/C命令集，由1394行业协会定义）的应用范围，实现了简化实施和易操作性。此协议为控制消息采用了AV/C设备模式和命令格式，这些消息可以通过音频/视频控制传输协议(AVCTP)传输。  

* OPP  
  蓝牙通信程序部分需采用用于设备之间传输数据对象OPP Profile: Object Push Profile由于OPP profile又细分为OPPC (client)端和OPPS(server)端profile，这两个profile区别在于只有client端可以发起数据传输的过程，但是附件设备与手机通信的情景中，既有手机发起数据传输请求也有设备侧发起传输请求的需要，所以要在设备中实现OPPC和OPPS两个profile。 

* PBAP  
  电话号码簿访问协议（PhonebookAccess Profile）  
