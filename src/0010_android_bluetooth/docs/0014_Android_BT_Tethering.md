# Android BT Tethering

蓝牙共享网络网卡生成原理

# master
  * ifconfig
    ```
    ...省略
    bt-pan    Link encap:Ethernet  HWaddr be:f5:46:03:bf:cb
              inet addr:192.168.44.1  Bcast:192.168.44.255  Mask:255.255.255.0
              inet6 addr: fe80::bcf5:46ff:fe03:bfcb/64 Scope: Link
              UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
              RX packets:450 errors:0 dropped:0 overruns:0 frame:0
              TX packets:506 errors:0 dropped:0 overruns:0 carrier:0
              collisions:0 txqueuelen:1000
              RX bytes:34548 TX bytes:383411    
    ```

# slave

  * getprop | grep dns
    ```
    [net.dns1]: [192.168.44.1]
    ```
  * ifconfig
    ```
    ...省略
    bt-pan    Link encap:Ethernet  HWaddr 36:6f:46:34:60:45
              inet addr:192.168.44.196  Bcast:192.168.44.255  Mask:255.255.255.0
              inet6 addr: fe80::eb22:9004:d650:4374/64 Scope: Link
              UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
              RX packets:358 errors:0 dropped:0 overruns:0 frame:0
              TX packets:328 errors:0 dropped:0 overruns:0 carrier:0
              collisions:0 txqueuelen:1000
              RX bytes:290844 TX bytes:25353
    ...省略

    ```
# logcat

  * logcat -s netd -s TetherController -s dnsmasq -s resolv

# bt pan
  
logcat
```
12-31 17:07:34.835131  1357  3931 D PanService: setBluetoothTethering: true, pkgName: com.google.android.networkstack.tethering, mTetherOn: false
12-31 17:07:53.409331  1181  1202 I EthernetTracker: interfaceLinkStateChanged, iface: bt-pan, up: false
12-31 17:07:53.409631  1181  1202 I EthernetTracker: interfaceLinkStateChanged, iface: bt-pan, up: false
12-31 17:07:53.410112  1181  1202 I EthernetTracker: interfaceLinkStateChanged, iface: bt-pan, up: true
12-31 17:07:53.419654  1357  1742 D BluetoothPanServiceJni: connection_state_callback(L81): state:0, local_role:1, remote_role:2
12-31 17:07:53.421976  1357  1357 I PanService: state 2 Num of connected pan devices: 0
12-31 17:07:53.422685  1357  1357 D PanService: handlePanDeviceStateChange preState: 0 state: 2
12-31 17:07:53.422960  1357  1357 D PanService: handlePanDeviceStateChange LOCAL_NAP_ROLE:REMOTE_PANU_ROLE
12-31 17:07:53.423070  1357  1357 I PanService: configureBtIface: bt-pan enable: true
12-31 17:07:53.423070  1357  1357 I PanService: configureBtIface: bt-pan enable: true
12-31 17:07:53.424746   538   755 I netd    : interfaceGetList() -> {["ccmni4","dummy0","ccmni16","tap0","ccmni2","ccmni14","ccmni0","ccmni12","bt-pan","ccmni9","ccmni20","p2p0","ccmni10","ip6_vti0","ccmni7","ccmni19","ccmni5","ccmni17","ccmni3","ccmni15","lo","ccmni1","ccmni13","ip6tnl0","ip_vti0","ccmni11","ccmni8","sit0","tunl0","wlan0","ap0","ccmni-lan","ccmni6","ccmni18"]} <0.27ms>
12-31 17:07:53.427382   538   755 I netd    : interfaceGetCfg("bt-pan") <0.39ms>
12-31 17:07:53.556642   538   755 D TetherController: tetherInterface(bt-pan)
12-31 17:07:53.558168   538   755 I netd    : tetherInterfaceAdd("bt-pan") <1.43ms>
12-31 17:07:53.563973   538   755 I netd    : networkAddInterface(99, "bt-pan") <4.12ms>
12-31 17:07:53.566335   538   755 I netd    : networkAddRoute(99, "bt-pan", "192.168.44.0/24", "") <1.14ms>
12-31 17:07:53.568434   538   755 I netd    : networkAddRoute(99, "bt-pan", "fe80::/64", "") <0.90ms>
12-31 17:07:53.647908  7864  7864 D dnsmasq : buffer: update_ifaces|bt-pan
12-31 17:07:53.647946  7864  7864 D dnsmasq : set_interfaces(bt-pan)
12-31 17:07:53.650801  7864  7864 D dnsmasq : adding listener for fe80::bcf5:46ff:fe03:bfcb%bt-pan
12-31 17:07:53.651729  7864  7864 D dnsmasq : dump_listeners listener fe80::bcf5:46ff:fe03:bfcb%bt-pan
```

bt-pan网卡是如何创建的

```cpp
// packages/modules/Bluetooth/system/btif/src/btif_pan.cc

int btpan_tap_open() {
  struct ifreq ifr;
  int fd, err;
  const char* clonedev = "/dev/tun";

  /* open the clone device */

  fd = open(clonedev, O_RDWR);
  if (fd < 0) {
    BTIF_TRACE_DEBUG("could not open %s, err:%d", clonedev, errno);
    return fd;
  }

  memset(&ifr, 0, sizeof(ifr));
  ifr.ifr_flags = IFF_TAP | IFF_NO_PI;

  strlcpy(ifr.ifr_name, TAP_IF_NAME, IFNAMSIZ);

  /* try to create the device */
  err = ioctl(fd, TUNSETIFF, (void*)&ifr);
  if (err < 0) {
    BTIF_TRACE_DEBUG("ioctl error:%d, errno:%s", err, strerror(errno));
    close(fd);
    return err;
  }
  if (tap_if_up(TAP_IF_NAME, controller_get_interface()->get_address()) == 0) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    return fd;
  }
  BTIF_TRACE_ERROR("can not bring up tap interface:%s", TAP_IF_NAME);
  close(fd);
  return INVALID_FD;
}
```

TAP_IF_NAME

```
// packages/modules/Bluetooth/system/btif/include/btif_pan_internal.h

#define TAP_IF_NAME "bt-pan"
```





