# bt gatt

android bt gatt 

# 缩写

GATT Generic Attribute Profile（通用属性配置文件）
GATT 是一个在蓝牙连接之上的发送和接收很短的数据段的通用规范，这些很短的数据段被称为属性（Attribute）

# 参考文档

* [GATT](https://blog.csdn.net/dfsae/article/details/78202596)
* [蓝牙【GATT】协议介绍](https://blog.csdn.net/qq_42166454/article/details/108229186)

# 为什么需要支持GATT

现在低功耗蓝牙（BLE）连接都是建立在 GATT (Generic Attribute Profile) 协议之上。GATT 是一个在蓝牙连接之上的发送和接收很短的数据段的通用规范，这些很短的数据段被称为属性（Attribute）

# GAP 

GAP （Generic Access Profile）

* 它在用来控制设备连接和广播。GAP 使你的设备被其他设备可见，并决定了你的设备是否可以或者怎样与合同设备进行交互。例如 Beacon 设备就只是向外广播，不支持连接，小米手环就等设备就可以与中心设备连接。

* GAP 给设备定义了若干角色，其中主要的两个是：外围设备（Peripheral）和中心设备（Central）。
  * 外围设备：这一般就是非常小或者简单的低功耗设备，用来提供数据，并连接到一个更加相对强大的中心设备。例如小米手环。
  * 中心设备：中心设备相对比较强大，用来连接其他外围设备。例如手机等。

* 广播数据
  * 在 GAP 中外围设备通过两种方式向外广播数据： Advertising Data Payload（广播数据）和 Scan Response Data Payload（扫描回复），每种数据最长可以包含 31 byte。这里广播数据是必需的，因为外设必需不停的向外广播，让中心设备知道它的存在。扫描回复是可选的，中心设备可以向外设请求扫描回复，这里包含一些设备额外的信息，例如设备的名字。

* 广播流程
  * 外围设备会设定一个广播间隔，每个广播间隔中，它会重新发送自己的广播数据。广播间隔越长，越省电，同时也不太容易扫描到。

* 广播的网络拓扑结构
  * 大部分情况下，外设通过广播自己来让中心设备发现自己，并建立 GATT 连接，从而进行更多的数据交换。也有些情况是不需要连接的，只要外设广播自己的数据即可。用这种方式主要目的是让外围设备，把自己的信息发送给多个中心设备。因为基于 GATT 连接的方式的，只能是一个外设连接一个中心设备。 使用广播这种方式最典型的应用就是苹果的 iBeacon。

# GATT
GATT 就是使用了 ATT（Attribute Protocol）协议，ATT 协议把 Service, Characteristic遗迹对应的数据保存在一个查找表中，次查找表使用 16 bit ID 作为每一项的索引。

GATT 连接需要特别注意的是：GATT 连接是独占的。也就是一个 BLE 外设同时只能被一个中心设备连接。一旦外设被连接，它就会马上停止广播，这样它就对其他设备不可见了。当设备断开，它又开始广播。

* GATT 连接的网络拓扑

GTT 连接网络拓扑结构一个外设只能连接一个中心设备，而一个中心设备可以连接多个外设。一旦建立起了连接，通信就是双向的了，对比前面的 GAP 广播的网络拓扑，GAP 通信是单向的。如果你要让两个设备外设能通信，就只能通过中心设备中转。

* GATT 通信事务

GATT 通信的双方是 C/S 关系。外设作为 GATT 服务端（Server），它维持了 ATT 的查找表以及 service 和 characteristic 的定义。中心设备是 GATT 客户端（Client），它向 Server 发起请求。需要注意的是，所有的通信事件，都是由客户端（也叫主设备，Master）发起，并且接收服务端（也叫从设备，Slave）的响应。

一旦连接建立，外设将会给中心设备建议一个连接间隔（Connection Interval）,这样，中心设备就会在每个连接间隔尝试去重新连接，检查是否有新的数据。但是，这个连接间隔只是一个建议，你的中心设备可能并不会严格按照这个间隔来执行，例如你的中心设备正在忙于连接其他的外设，或者中心设备资源太忙

* GATT 结构

GATT 事务是建立在嵌套的Profiles, Services 和 Characteristics之上的的。
Profile Profile 并不是实际存在于 BLE 外设上的，它只是一个被 Bluetooth SIG 或者外设设计者预先定义的 Service 的集合。例如心率Profile（Heart Rate Profile）就是结合了 Heart Rate Service 和 Device Information Service。所有官方通过 GATT Profile 的列表可以从这里找到
Service Service 是把数据分成一个个的独立逻辑项，它包含一个或者多个 Characteristic。每个 Service 有一个 UUID 唯一标识。 UUID 有 16 bit 的，或者 128 bit 的。16 bit 的 UUID 是官方通过认证的，需要花钱购买，128 bit 是自定义的，这个就可以自己随便设置。

* 初始化代码堆栈
因为都是蓝牙对应的服务，都继承ProfileService，所以代码整体结构都很相似
```
* mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/jni/com_android_bluetooth_gatt.cpp
  * static void initializeNative(JNIEnv* env, jobject object)
    * sGattIf = (btgatt_interface_t*)btIf->get_profile_interface(BT_PROFILE_GATT_ID);
      * mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/btif/src/bluetooth.cc
        * static const void* get_profile_interface(const char* profile_id)
          * if (is_profile(profile_id, BT_PROFILE_GATT_ID))
            * return btif_gatt_get_interface();
              * btgattInterface.scanner = get_ble_scanner_instance();
                * mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/btif/src/btif_ble_scanner.cc
                  * BleScannerInterface* get_ble_scanner_instance()
                    * btLeScannerInstance = new BleScannerInterfaceImpl();
                    * return btLeScannerInstance; 
              * btgattInterface.advertiser = get_ble_advertiser_instance();
                * mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/btif/src/btif_ble_advertiser.cc 
                  * btLeAdvertiserInstance = new BleAdvertiserInterfaceImpl();
              * return &btgattInterface;
    * bt_status_t status = sGattIf->init(&sGattCallbacks); //传入回调函数
      * mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/btif/src/btif_gatt.cc
        * .init = btif_gatt_init,
          * static bt_status_t btif_gatt_init(const btgatt_callbacks_t* callbacks)
            * bt_gatt_callbacks = callbacks;
        * .server = &btgattServerInterface,
          * const btgatt_server_interface_t btgattServerInterface = {
            btif_gatts_register_app,   btif_gatts_unregister_app,
            btif_gatts_open,           btif_gatts_close,
            btif_gatts_add_service,    btif_gatts_stop_service,
            btif_gatts_delete_service, btif_gatts_send_indication,
            btif_gatts_send_response,  btif_gatts_set_preferred_phy,
            btif_gatts_read_phy};
```

相关数据结构及回调函数
```
mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/btif/src/btif_gatt.cc 
  static btgatt_interface_t btgattInterface = {
      .size = sizeof(btgattInterface),
  
      .init = btif_gatt_init,
      .cleanup = btif_gatt_cleanup,
  
      .client = &btgattClientInterface,
      .server = &btgattServerInterface,
      .scanner = nullptr,    // filled in btif_gatt_get_interface
      .advertiser = nullptr  // filled in btif_gatt_get_interface
  };
  
mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/android/app/jni/com_android_bluetooth_gatt.cpp
  static const btgatt_scanner_callbacks_t sGattScannerCallbacks = {
      btgattc_scan_result_cb,
      btgattc_scan_result_for_channel_cb,
      btgattc_batchscan_reports_cb,
      btgattc_batchscan_threshold_cb,
      btgattc_track_adv_event_cb,
  };

  static const btgatt_callbacks_t sGattCallbacks = {
      sizeof(btgatt_callbacks_t), &sGattClientCallbacks, &sGattServerCallbacks,
      &sGattScannerCallbacks,
  };

mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/include/hardware/bt_gatt.h 
  typedef struct {
    /** Set to sizeof(btgatt_interface_t) */
    size_t size;
  
    /**
     * Initializes the interface and provides callback routines
     */
    bt_status_t (*init)(const btgatt_callbacks_t* callbacks);
  
    /** Closes the interface */
    void (*cleanup)(void);
  
    /** Pointer to the GATT client interface methods.*/
    const btgatt_client_interface_t* client;
  
    /** Pointer to the GATT server interface methods.*/
    const btgatt_server_interface_t* server;
  
    /** Pointer to the LE scanner interface methods.*/
    BleScannerInterface* scanner;
  
    /** Pointer to the advertiser interface methods.*/
    BleAdvertiserInterface* advertiser;
  } btgatt_interface_t;

mssi/vendor/mediatek/proprietary/packages/modules/Bluetooth/system/include/hardware/bt_gatt_server.h 
  typedef struct {
    /** Registers a GATT server application with the stack */
    bt_status_t (*register_server)(const bluetooth::Uuid& uuid,
                                   bool eatt_support);
  
    /** Unregister a server application from the stack */
    bt_status_t (*unregister_server)(int server_if);
  
    /** Create a connection to a remote peripheral */
    bt_status_t (*connect)(int server_if, const RawAddress& bd_addr,
                           bool is_direct, int transport);
  
    /** Disconnect an established connection or cancel a pending one */
    bt_status_t (*disconnect)(int server_if, const RawAddress& bd_addr,
                              int conn_id);
  
    /** Create a new service */
    bt_status_t (*add_service)(int server_if, const btgatt_db_element_t* service,
                               size_t service_count);
  
    /** Stops a local service */
    bt_status_t (*stop_service)(int server_if, int service_handle);
  
    /** Delete a local service */
    bt_status_t (*delete_service)(int server_if, int service_handle);
  
    /** Send value indication to a remote device */
    bt_status_t (*send_indication)(int server_if, int attribute_handle,
                                   int conn_id, int confirm, const uint8_t* value,
                                   size_t length);
  
    /** Send a response to a read/write operation */
    bt_status_t (*send_response)(int conn_id, int trans_id, int status,
                                 const btgatt_response_t& response);
  
    bt_status_t (*set_preferred_phy)(const RawAddress& bd_addr, uint8_t tx_phy,
                                     uint8_t rx_phy, uint16_t phy_options);
  
    bt_status_t (*read_phy)(
        const RawAddress& bd_addr,
        base::Callback<void(uint8_t tx_phy, uint8_t rx_phy, uint8_t status)> cb);
  
  } btgatt_server_interface_t;
```