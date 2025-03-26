# bluetooth networkshare

工厂反馈蓝牙连接的网络共享不能访问网络

# log

* [0020_server_main_log_1__2023_0408_153638](refers/0020_server_main_log_1__2023_0408_153638)
* [0020_sink_main_log_1__2023_0408_153624](refers/0020_sink_main_log_1__2023_0408_153624)

# 日志分析

从设备日志在网络连接时，提示验证失败
```
04-08 15:34:36.318264  1693 20700 D NetworkMonitor/105: PROBE_HTTPS https://www.google.cn/generate_204 Probe failed with exception java.net.UnknownHostException: Unable to resolve host "www.google.cn": No address associated with hostname
04-08 15:34:36.318704 19229 19263 D NetworkWatcher: notifyListener connected:true
04-08 15:34:36.318765 19229 19263 D NetworkWatcher: notifyListener listener size:0
04-08 15:34:36.319567  6532  6532 I SetupWizard: [bob] User unlocked, initiate a checkin task java.util.concurrent.CompletableFuture@8cb6724[Not completed, 1 dependents]
04-08 15:34:36.319696  6532  6532 I SetupWizard: [SetupNotificationService] createNotification startedForegroundService=true
04-08 15:34:36.321723  1584  2055 I bt_btm_pm: system/bt/stack/acl/btm_pm.cc:245 BTM_SetPowerMode: Device is already in requested mode 0, interval: 0, max: 0, min: 0
04-08 15:34:36.321768  1584  2055 I bluetooth: system/bt/bta/dm/bta_dm_pm.cc:905 bta_dm_pm_active: Active power mode already set for device:xx:xx:xx:xx:0a:00
04-08 15:34:36.324269  1693 20697 D NetworkMonitor/105: isCaptivePortal: isSuccessful()=false isPortal()=false RedirectUrl=null isPartialConnectivity()=false Time=109ms
04-08 15:34:36.325484  1312  1440 D ConnectivityService: [105 BLUETOOTH] validation failed
```

主设备中网络连接直接由connectiong 转为disconnected，确认此部分APN配置是否正

```
04-08 15:36:24.278282  2234  2234 V UserExperience: o$d.onPreciseDataConnectionStateChanged() state:  state: CONNECTING, transport: WWAN, id: -1, network type: LTE, APN Setting: [ApnSetting] mineo(ドコモプラン), 2849, 44010, mineo-d.jp, , null, , null, null, 2, supl | dun | hipri | default, IP, IP, true, 1, true, 0, 0, 0, 0, 0, null, , false, UNKNOWN, UNKNOWN, 0, -1, -1, false, link properties: {LinkAddresses: [ ] DnsAddresses: [ ] Domains: null MTU: 0 Routes: [ ]}, fail cause: NONE(0x0)
04-08 15:36:24.282647   767  4996 D CCodecBuffers: [c2.mtk.avc.decoder#191:2D-BB-Output] updating image-data
04-08 15:36:24.282745   767  4996 D CCodecBuffers: [c2.mtk.avc.decoder#191:2D-BB-Output] updating stride = 800, width: 800, height: 1280
04-08 15:36:24.282768   767  4996 D CCodecBuffers: [c2.mtk.avc.decoder#191:2D-BB-Output] updating vstride = 1280
04-08 15:36:24.287599  1187  1316 D ConnectivityService: [116 CELLULAR] EVENT_NETWORK_INFO_CHANGED, going from CONNECTING to CONNECTING
04-08 15:36:24.291844  1501  5006 D NetworkMonitor/116: checkAndRenewResourceConfig: update captive portal https urls to [https://www.google.com/generate_204]
04-08 15:36:24.294167  1501  5006 D NetworkMonitor/116: checkAndRenewResourceConfig: update captive portal http urls to [http://connectivitycheck.gstatic.com/generate_204]
04-08 15:36:24.294593  1501  5006 D NetworkMonitor/116: checkAndRenewResourceConfig: update captive portal fallback urls to[http://www.google.com/gen_204, http://play.googleapis.com/generate_204]
04-08 15:36:24.294837  1501  5006 D NetworkMonitor: Starting on network 116 with capport HTTPS URL [https://www.google.com/generate_204] and HTTP URL [http://connectivitycheck.gstatic.com/generate_204]
......
......
......
04-08 15:36:24.369935   610   610 I SurfaceFlinger: operator()(), mtkRenderCntDebug 1198, screenshot (NotificationShade#954)
04-08 15:36:24.384179  2234  2234 V UserExperience: o$d.onPreciseDataConnectionStateChanged() state:  state: DISCONNECTED, transport: WWAN, id: -1, network type: LTE, APN Setting: [ApnSetting] mineo(ドコモプラン), 2849, 44010, mineo-d.jp, , null, , null, null, 2, supl | dun | hipri | default, IP, IP, true, 1, true, 0, 0, 0, 0, 0, null, , false, UNKNOWN, UNKNOWN, 0, -1, -1, false, link properties: {LinkAddresses: [ ] DnsAddresses: [ ] Domains: null MTU: 0 Routes: [ ]}, fail cause: USER_AUTHENTICATION(0x1d)
04-08 15:36:24.384508  2234  2234 V UserExperience: o$d.e() isPrcLTESku false , getCellLocation true
04-08 15:36:24.385852   767   909 D StagefrightMetadataRetriever: ~StagefrightMetadataRetriever()
04-08 15:36:24.392464  1187  1316 D ConnectivityService: [116 CELLULAR] EVENT_NETWORK_INFO_CHANGED, going from CONNECTING to DISCONNECTED
```

