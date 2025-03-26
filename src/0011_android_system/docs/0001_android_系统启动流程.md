# android 系统启动流程

android 系统启动流程


# menu


# 简介

  这里主要介绍android 系统启动流程


# 参考文档

* [Android操作系统启动过程概览](https://zhuanlan.zhihu.com/p/127074795)
* [Android系统启动过程分析](https://www.cnblogs.com/Robin132929/p/12158006.html)
* [安卓开机流程](https://blog.csdn.net/mengxin00100/article/details/120684441)
* [基于Android13的系统启动流程分析（一）之SeLinux权限介绍](https://blog.csdn.net/q1210249579/article/details/128637218)
* [基于Android13的系统启动流程分析（三）之FirstStageMain阶段](https://blog.csdn.net/q1210249579/article/details/128652348)
* [Android安全启动学习（三）：AVB校验的内容、怎么校验、AVB的作用](https://blog.csdn.net/weixin_45264425/article/details/127255603)
* [基于Android13的系统启动流程分析（四）之SecondStageMain阶段](https://blog.csdn.net/q1210249579/article/details/128759108)
* [Android MultiMedia框架完全解析 - 从开机到MediaServer的注册过程](https://blog.csdn.net/yanbixing123/article/details/88926554/)
* [Android servicemanager进程启动过程](https://blog.csdn.net/qq_36063677/article/details/129268692)

# 开机流程

  1 Boot ROM
  
    长按电源按键，引导 ROM 代码从 ROM 中预定义位置开始执行。它将引导加载程序加载到 RAM 中并开始执行。存储在Boot ROM中的 Primary Bootloader (PBL)是引导过程第一阶段，此代码由芯片制造商编写；PBL验证下一阶段的真实性，如果辅助引导加载程序验证失败，则进入EDL(Emergency Download)。由于Boot ROM 空间有限，因此将辅助引导加载程序存储在eMMC中
  
  2 Bootloader
  
    Bootloader主要是在系统加载前，初始化硬件设备，建立内存空间的映像图，为最终调用系统内核准备好环境。在 Android运行环境中里没有硬盘，而是 ROM，它类似于硬盘存放操作系统，用户程序等。ROM 跟硬盘一样也会划分为不同的区域，用于放置不同的程序Bootloader程序分两个阶段执行。第一个阶段，检测外部的RAM以及加载对第二阶段有用的程序；第二阶段，设置网络、内存等等。这些对于运行内核是必要的，为了达到特殊的目标，bootloader可以根据配置参数或者输入数据设置内核。
  
   3 初始化Kernel
  
     kernel开始启动时，设置缓存、被保护存储器、计划列表，加载驱动。当初始化内核之后，就会启动一个相当重要的祖先进程，也就是init进程。关于init进程有兴趣的可以去研究下system/core/init/main.cpp  。
  
   4 servicemanager、zygote、mediaserver、surfaceflinger等进程创建
  
     当init进程创建之后，会创建进程服务，其中包括servicemanager、mediaserver、surfaceflinger、Zygote等一系列服务。init进程如何创建各种服务的，有兴趣的可以去跟下代码：system/core/init/main.cpp 。
  
   5 系统服务启动

     zygote启动后拉起安卓多应的系统服务

   6 开机完成

     发送广播“android.intent.action.BOOT_COMPLETED”，通知完成开机过程

# init进程启动分析

  源码分析  

  system/core/init/main.cpp 

  ```cpp
  int main(int argc, char** argv) {
  #if __has_feature(address_sanitizer)
      __asan_set_error_report_callback(AsanReportCallback);
  #endif
  
      if (!strcmp(basename(argv[0]), "ueventd")) {
          return ueventd_main(argc, argv);
      }
  
      if (argc > 1) {
          if (!strcmp(argv[1], "subcontext")) {
              android::base::InitLogging(argv, &android::base::KernelLogger);
              const BuiltinFunctionMap& function_map = GetBuiltinFunctionMap();
  
              return SubcontextMain(argc, argv, &function_map);
          }
  
          if (!strcmp(argv[1], "selinux_setup")) {
              return SetupSelinux(argv);
          }
  
          if (!strcmp(argv[1], "second_stage")) {
              return SecondStageMain(argc, argv);
          }
      }
  
      return FirstStageMain(argc, argv);
  }
  ```

  FirstStageMain阶段总结

    根据main.cpp来启用第一阶段FirstStageMain，挂载最基本的文件系统，该文件系统是运行于RAM上的，优点是相比disk磁盘来说运行速度快，不占存储空间，特点是易失性，断电即丢失，挂载上最基本的文件系统后会根据根目录"/"来挂  
    载/mnt/{vendor,product}等重要的分区，其他不重要的文件挂载在第二阶段rc中处理，生成 /second_stage_resources/system/etc/ramdisk/build.prop,该文件会在第二阶段使用，在第一阶段并开启kernel log，挂载  
    /first_stage_ramdisk新的根目录，根据设备树（fstab）来创建逻辑分区system，system_ext,vendor,product并挂载到/first_stage_ramdisk根目录上，然后将old 根目录切换到/first_stage_ramdisk 根目录，释放old
    根目录，/first_stage_ramdisk根目录将赋予较为安全的权限，创建AVB数据校验，启用overlayfs机制来保护分区原子性，初始化恢复模式下的AVB校验方案，然后调用"/system/bin/init"进入下一个阶段：selinux_setup

  SecondStageMain

    至此第二阶段就分析完毕了，一句话来总结：优先保证init进程的存活率（拉高优先级），处理设备是否unlock,决定是否卸载一些分区以及调整selinux规则和权限，创建并启动属性服务（实质上就是把属性值映射到全局内存中供所有进程访问，  
    然后在创建socket等待进程来连接 实现更新和新增属性值），继续决定是否把vendor_overlay覆盖到/vendor分区中，然后持续监控/proc/mounts,如果有分区信息加入到该文件中则挂载此分区，接着解析rc文件（创建目录，修改权限，  
    挂载分区，启动服务进程等），根据调用顺序启动核心服务（adbd,ueventd等）以及主服务(zygote)和其他服务

# 代码堆栈
  
  ```
  * int main(int argc, char** argv)  //system/core/init/main.cpp 
    ├── return SetupSelinux(argv);  //挂载并启用selinux权限系统
    ├── return SecondStageMain(argc, argv);  //主要解析ini.rc文件，创建zygote孵化器，fork 进程等
    │   └── int SecondStageMain(int argc, char** argv)
    │       ├── PropertyInit();  //初始化属性服务
    │       │   └── void PropertyInit() 
    │       │       ├── CreateSerializedPropertyInfo();  //建序列化过后的propertyInfo实体，主要就是读取property_contexts文件
    │       │       ├── if (__system_property_area_init())  //通过mmap映射，将文件（/dev/__properties__/{..}）映射进内存（初始化属性内存映射文件）
    │       │       ├── if (!property_info_area.LoadDefaultPath()) //加载/dev/__properties__/property_info
    │       │       ├── ProcessKernelDt();
    │       │       ├── ProcessKernelCmdline(); //这两个函数主要就是生成ro.boot.xx属性值
    │       │       ├── ExportKernelBootProps();  //初始化ro.xx,将ro.boot.xx的属性值复制给ro.xxx
    │       │       └── PropertyLoadBootDefaults();  //读取{system/vendor/odm/product}/build.prop等，将build.prop通过MMAP映射到全局内存中
    │       ├── MountExtraFilesystems();  //主要就挂载了/apex和/linkerconfig并归属于tmpfs文件系统
    │       ├── StartPropertyService(&property_fd);
    │       │   └── void StartPropertyService(int* epoll_socket)
    │       │       ├── int sockets[2];  //创建sockets,套接字,可以用于网络通信，也可以用于本机内的进程通信
    │       │       ├── if (socketpair(AF_UNIX, SOCK_SEQPACKET | SOCK_CLOEXEC, 0, sockets) != 0)  //创建一对无名的,相互连接的套接字
    │       │       ├── listen(property_set_fd, 8);  //监听socket:/dev/socket/property_service,最大连接：8
    │       │       ├── auto new_thread = std::thread{PropertyServiceThread};
    │       │       └── property_service_thread.swap(new_thread); // 开启线程处理socket
    │       ├── fs_mgr_vendor_overlay_mount_all();  // 将vendor_overlay挂载到vendor上
    │       ├── ActionManager& am = ActionManager::GetInstance();  //创建ActionManager对象
    │       ├── ServiceList& sm = ServiceList::GetInstance();  //创建ServiceList对象
    │       └── LoadBootScripts(am, sm);  //加载rc文件，保存到action manager和service list中
    │           └── static void LoadBootScripts(ActionManager& action_manager, ServiceList& service_list)
    │               ├── Parser parser = CreateParser(action_manager, service_list);  //创建解析器，用于解析rc文件
    │               └── parser.ParseConfig("/system/etc/init/hw/init.rc");  //解析init.rc, 其中import zygote.rc
    │                   ├── import /system/etc/init/hw/init.${ro.zygote}.rc
    │                   │   ├── service zygote /system/bin/app_process64 -Xzygote /system/bin --zygote --start-system-server  //启动zygote
    │                   │   │   └── frameworks/base/cmds/app_process/app_main.cpp
    │                   │   │       └── int main(int argc, char* const argv[])
    │                   │   │           └── runtime.start("com.android.internal.os.ZygoteInit", args, zygote);
    │                   │   ├── onrestart restart audioserver
    │                   │   ├── onrestart restart cameraserver
    │                   │   └── onrestart restart media  // 启动mediaserver
    │                   │       └── frameworks/av/media/mediaserver/mediaserver.rc
    │                   │           └── service media /system/bin/mediaserver
    │                   │               └── frameworks/av/media/mediaserver/main_mediaserver.cpp
    │                   ├── start servicemanager  //启动servicemanager
    │                   │   └── frameworks/native/cmds/servicemanager/main.cpp
    │                   ├── start hwservicemanager
    │                   └── start vndservicemanager
    └── return FirstStageMain(argc, argv);  //主要创建和挂载基本的文件系统，挂载特定分区，启用log等
        └── int FirstStageMain(int argc, char** argv) 
            ├── InstallRebootSignalHandlers();  //主要加载引导程序以及init阶段crash的进程处理
            │   └── void InstallRebootSignalHandlers()  
            │       └── InitFatalReboot(signal);
            ├── if (ForceNormalBoot(cmdline))  //查看androidboot.force_normal_boot 属性
            │   └── bool ForceNormalBoot(const std::string& cmdline)
            │       └── return cmdline.find("androidboot.force_normal_boot=1") != std::string::npos;
            ├── if (!DoFirstStageMount())
            │   ├── if (IsRecoveryMode()) // 是否处于recovery模式下，如果是则跳过第一个阶段
            │   ├── std::unique_ptr<FirstStageMount> handle = FirstStageMount::Create();该方法主要用于AVB校验
            │   └── return handle->DoFirstStageMount();
            │       └── bool FirstStageMount::DoFirstStageMount()
            │           ├── if (!InitDevices()) return false;
            │           └── if (!MountPartitions()) return false;  // 挂载分区详细步骤
            └── SetInitAvbVersionInRecovery(); //读取fstab
  ```
