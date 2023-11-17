# Preloader DA下载

PL DA下载原理

# 参考文档

* [preloader流程—-基于MTK平台](https://blog.csdn.net/cnmlrxtoyt/article/details/79121112)

# MTK安全静态库

```
* vendor/mediatek/proprietary/bootable/bootloader/preloader/platform/mt6765/src/core/main.c
  └── mode = seclib_brom_meta_mode();
      └── vendor/mediatek/proprietary/bootable/bootloader/preloader/platform/mt6765/lib/libsecplat.a
          └── vendor/mediatek/proprietary/bootable/bootloader/preloader/platform/mt6765/src/security/inc/sec.h
```

# PL检测USB流程

* 进入PL后，音量上+USB插入会导致系统进入下载救援模式
* 正常启动，如果连接了USB，开了flashtool下载，会握手进入下载模式
```
* vendor/mediatek/proprietary/bootable/bootloader/preloader/
  └── platform/mt6765/src/core/main.c
      └── void main(u32 *arg)
          ├── bldr_pre_process();
          │   ├── platform_pre_init();                          --> essential hardware initialization. e.g. timer, pll, uart...
          │   ├── platform_init();                              --> hardware initialization
          │   │   └── platform/mt6765/src/drivers/platform.c
          │   │       └── void platform_init(void)
          │   │           └── #if CFG_EMERGENCY_DL_SUPPORT      --> 默认是打开的
          │   │               ├── vendor/mediatek/proprietary/bootable/bootloader/preloader/platform/mt6765/default.mak
          │   │               │   └── CFG_EMERGENCY_DL_SUPPORT :=1
          │   │               └── if (mtk_detect_dl_keys() && g_boot_reason == BR_USB)          --> 插入USB开机，并按下音量+按键进入强制烧录模式
          │   │                   └── platform_emergency_download(CFG_EMERGENCY_DL_TIMEOUT_MS);
          │   │                       └── platform/mt6765/src/drivers/platform.c
          │   │                           └── void platform_emergency_download(u32 timeout)
          │   │                               ├── platform_safe_mode(1, timeout);
          │   │                               │   └── if (en)
          │   │                               │       └── usbdlreg |= USBDL_BIT_EN;             --> 重启后进入BROM下载模式
          │   │                               └── mtk_arch_reset(0);                            --> 重启机器
          │   └── #if CFG_UART_TOOL_HANDSHAKE
          │       ├── boot_mode_t mode = NORMAL_BOOT;
          │       ├── if (!isLocked && mode == NORMAL_BOOT)
          │       ├── pal_log_info("%s Starting tool handshake.\n", MOD);
          │       └── uart_handshake_init();
          │           └── uart_send((u8*)HSHK_COM_READY, strlen(HSHK_COM_READY));               --> 发送READY
          │               └── HSHK_COM_READY
          │                   └── platform/mt6761/src/core/inc/meta.h
          │                       └── #define HSHK_COM_READY          "READY"                   --> 发送READY
          ├── handler.cb   = bldr_cmd_handler;                                                  --> 下载模式不会被处理，meta、factory会调用这里处理
          │   └── static bool bldr_cmd_handler(struct bldr_command_handler *handler, struct bldr_command *cmd, struct bldr_comport *comport)
          ├── bldr_handshake(&handler);                                                         --> USB握手处理
          │   └── switch (mode)
          │       └── case NORMAL_BOOT:
          │           ├── TRUE == usb_handshake(handler)                                        --> USB处理
          │           │   └── usb_handshake
          │           │       └── platform/mt6761/src/core/handshake_usb.c
          │           │           └── bool usb_handshake(struct bldr_command_handler *handler)
          │           │               ├── usb_cable_in()                                        --> 检测USB线接入
          │           │               │   ├── usb_accessory_in()
          │           │               │   │   └── PMIC_CHRDET_EXIST == pmic_IsUsbCableIn()
          │           │               │   │       └── ret = pmic_read_interface((U32)(PMIC_RGS_CHRDET_ADDR), (&val), (U32)(PMIC_RGS_CHRDET_MASK), (U32)(PMIC_RGS_CHRDET_SHIFT));
          │           │               │   └── ret = mt_charger_type_detection();
          │           │               │       └── g_ret = hw_charger_type_detection();
          │           │               │           ├── hw_bc11_DCD()
          │           │               │           │   └── pmic_read_interface(PMIC_RGS_BC11_CMP_OUT_ADDR, &wChargerAvail, PMIC_RGS_BC11_CMP_OUT_MASK, PMIC_RGS_BC11_CMP_OUT_SHIFT);
          │           │               │           │       ├── #define PMIC_RGS_BC11_CMP_OUT_ADDR                         MT6357_CHR_CON8
          │           │               │           │       ├── #define PMIC_RGS_BC11_CMP_OUT_MASK                         0x1
          │           │               │           │       └── #define PMIC_RGS_BC11_CMP_OUT_SHIFT                        14
          │           │               │           └── print("chr type: %d\n", charger_type);
          │           │               ├── pal_log_warn("%s PMIC not dectect usb cable!\n", MOD);
          │           │               ├── usbdl_init();                                         --> USB配置
          │           │               ├── usb_connect(enum_tmo)                                 --> 连接USB
          │           │               └── usb_handshake_handler(handler, handshake_tmo)         --> 处理USB数据事件
          │           │                   ├── usb_listen(&comport, buf, HSHK_TOKEN_SZ, tmo)
          │           │                   │   └── while (1)
          │           │                   │       ├── dsz = mt_usbtty_query_data_size();
          │           │                   │       └── if (dsz) 
          │           │                   │           ├── mt_usbtty_getcn(dsz, (char*)ptr);
          │           │                   │           └── #if CFG_USB_DOWNLOAD                  --> USB下载是直接处理，所以没走handler处理
          │           │                   │               └── if (*ptr == 0xa0)                 
          │           │                   │                   ├── pal_log_info("%s 0xa0 sync time %dms\n", MOD, get_timer(start_time));
          │           │                   │                   └── usbdl_handler(comport, 300);  --> USB下载处理
          │           │                   │                       └── platform/mt6761/src/core/download.c
          │           │                   │                           └── int usbdl_handler(struct bldr_comport *comport, u32 hshk_tmo_ms)
          │           │                   │                               └── while (1)
          │           │                   │                                   ├── usbdl_get_byte(&cmd);
          │           │                   │                                   └── switch (cmd)
          │           │                   │                                       ├── case CMD_SEND_DA:           --> flashtool下载DA
          │           │                   │                                       │   ├── DBGMSG("%s CMD_SEND_DA\n", MOD);
          │           │                   │                                       │   └── usbdl_send_da();
          │           │                   │                                       │       ├── usbdl_get_data((u8*)da_addr, da_len);
          │           │                   │                                       │       └── status = usbdl_verify_da((u8*)da_addr, da_len, sig_len);
          │           │                   │                                       │           └── pal_log_info("usbdl_vfy_da:disabled\n");
          │           │                   │                                       └── case CMD_JUMP_DA:           --> flashtool跳转DA
          │           │                   │                                           ├── DBGMSG("%s CMD_JUMP_DA\n", MOD);
          │           │                   │                                           └── usbdl_jump_da();
          │           │                   │                                               └── bldr_jump(da_addr, (u32)da_arg, (u32)sizeof(DownloadArg));
          │           │                   └── return handler->cb(handler, &cmd, &comport);
          │           └── TRUE == uart_handshake(handler)
          └── bldr_post_process();
```
