# bt l2cap

l2cap 介绍

# 参考文档

* [蓝牙（七）L2CAP层协议解析](https://blog.csdn.net/xsophiax/article/details/104052416)
* [蓝牙协议栈之L2CAP使用](https://blog.csdn.net/qq_40993639/article/details/130540160)
* [蓝牙核心技术概述（四）：蓝牙协议规范（HCI、L2CAP、SDP、RFOCMM）](https://blog.csdn.net/xubin341719/article/details/38305331?spm=1001.2101.3001.6650.2&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7ERate-2-38305331-blog-130540160.235%5Ev36%5Epc_relevant_yljh&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7ERate-2-38305331-blog-130540160.235%5Ev36%5Epc_relevant_yljh&utm_relevant_index=3)
# 概述

L2CAP-全称是逻辑链路控制与适配层，为两个通信的蓝牙设备提供一个端到端的通道。

* L2CAP主要功能：  
  1.协议信道复用（protocol/channel multiplexing）  
  2.分段与重组（segmentation and reassembly SAR）  
  3.每个信道流控（per-channel flow control）  
  4.差错控制（error control）  
  
* 逻辑信道：  
  L2CAP逻辑信道分配情况如下（CID即channelID）：  
  1.0x0001信道作为发送信令的信道，如发送Conn_req；  
  2.0x0002被作为无连接（两个设备间未建立ACL通路）的信道使用；  
  3.0x0040-0xFFFF是动态分配的，如在SDP服务发现时候就需要动态分配一个CID，和对端的CID连接后进行通信；  
  4.在BLE设备中0x0005作为BLE设备的信令通道；如果设备支持BLE功能，0x0005作为其信令通道，则0x0004和0x0006也会被强制分配给BLE，分别作为ATT和SecurityManager信道  

* 信道模式:  
  逻辑信道分为5中模式，最后一种是LE设备特有的：  
  1.Basic L2CAP Mode(equivalent to L2CAP specification in Bluetooth v1.1)，是默认模式，在未选择其他模式的情况下，就是用此模式  
  2.Flow Control Mode 模式下，不会进行重传，但是丢失的数据能够被检测到，并报告丢失。  
  3.Retransmission Mode 确保数据包都成功的传输给对端设备  
  4.Enhanced Retransmission Mode 是为了真实的等时传输，数据包被编号但是不需要ACK确认。设定一个超时定时器，一旦定时器超时就将超时的数据干掉  
  5.Streaming Mode 是为了真实的等时传输，数据包被编号但是不需要ACK确认。设定一个超时定时器，一旦定时器超时就将超时的数据干掉。  
  6.LE Credit Based Flow Control Mode 模式是唯一被用于LE设备的信道模式  

# 常用的L2CAP术语

L2CAP Channel: L2CAP通道，对端设备两节点之间的逻辑连接，用它们的信道标识符（CIDs）做通道的区分。  
SDU: 服务数据单元，L2CAP与上层交换的数据包，它不包含L2CAP的帧头。  
PDU: 协议数据单元，包括了L2CAP协议信息域、控制信息、上层信息数据，这个数据包就包含了L2CAP的帧头。一个SDU可能被分割成多个PDU进行传输。  
MTU: 最大数据传输单元，上层应用可以接收的payload最大字节数，注意这个跟ATT的MTU是不一样的。  
MPS: L2CAP可以接收的payload最大字节数。  
Credit: 本蓝牙设备可以接收的LE帧数量。Credits取值范围是1~65535，在两个设备之间使用流控制。  
L2CAP Basic Header: 为每个PDU预先准备的L2CAP协议信息。它包括了CID和长度。  
PSM: 协议服务复用器，占用两个字节，用于定义L2CAP信道数据的解析。有动态的PSM和固定的PSMs。固定的PSMs是由SIG定义的，而动态的PSMs可以由GATT发现。  
Fragmentation/Reconbination: 分包重组，分包（分段）是将单个L2CAP PDU分解成更小的数据段以供发送器发送的过程。重组是控制器将片段重新组装成完整的L2CAP PDU的过程。分包重组是由控制器实现的，并且基于LE数据长度扩展特征（可以看下Local supported feature）。  
Segmentation/Reassembly: 分段是将单个L2CAP SDU分解成多个成为SDU段的L2CAP数据包的过程。在接收侧按照与此操作相反的方式进行重新组装。每个段都封装在一个适当的L2CAP报头中。分段和重组都是由L2CAP处理，并且对上下层都是透明的。  


# L2CAP MTU

 L2CAP MTU是L2CAP层能够处理的最大数据字节大小。然而，L2CAP使用的MTU是不同的，取决于模式和通道类型。  
（1）信号通道会使用L2CAP_SIG_MTU_SIZE  
（2）固定通道数据包的MTU限定最大是MAX_PDU_SIZE - L2CAP_HDR_SIZE  
（3）COC数据包的最大字节数取决于PSM的MTU，同时被L2CAP_SDU_SIZE限制  
    当对于固定通道，MTU由更高级别的协议（如ATT）定义。在COC上，MTU受L2CAP_SDU_SIZE和对端设备支持的MTU的最小值约束。  


