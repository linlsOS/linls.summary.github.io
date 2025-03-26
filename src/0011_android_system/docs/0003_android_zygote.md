# android zygote

android zygote 简介


# menu

# 参考文档

* [0001_android_系统启动流程.md](0001_android_系统启动流程.md)
* [Zygote的启动流程](https://zhuanlan.zhihu.com/p/145565662)
* [深入研究源码：Android10.0系统启动流程（三）：Zygote](https://zhuanlan.zhihu.com/p/350204845)
* [JNI: 连接Java世界的JavaVM和JNIEnv](https://juejin.cn/post/6844903880115879950)
* [Android系统启动过程分析](https://www.cnblogs.com/Robin132929/p/12158006.html)
* [Android系统启动流程(3) —— 解析SystemServer进程启动过程](https://blog.csdn.net/lixiong0713/article/details/106756718)
* [Android 11进程启动分析（二）：Launcher启动流程分析](https://blog.csdn.net/u013028621/article/details/116533988)
* [Zygote进程原理简单介绍，源码解析](https://blog.csdn.net/qq_41872247/article/details/125211491)
* [Android技术探索](https://zhuanlan.zhihu.com/p/260414370)
* [【图文分析】Android系统启动到应用界面展示的全流程](https://juejin.cn/post/7280924632457494563)

# 简介

  zygote的启动是init进程通过rc文件拉起的服务，系统启动流程中有介绍init进程如何启动zygote进程.
  Zygote的作用分为两点: 1、启动SystemServer； 2、孵化应用进程


# 代码堆栈

```
* frameworks/base/cmds/app_process/app_main.cpp
  * AppRuntime runtime(argv[0], computeArgBlockSize(argc, argv));  //使用AppRuntime类实例化了一个虚拟机runtime，而AppRuntime则继承AndroidRuntime;
  * runtime.start("com.android.internal.os.ZygoteInit", args, zygote);  //启动虚拟机
    * frameworks/base/core/jni/AndroidRuntime.cpp
      * void AndroidRuntime::start(const char* className, const Vector<String8>& options, bool zygote)
        * if (startVm(&mJavaVM, &env, zygote, primary_zygote) != 0)  // 启动Java虚拟机
          * int AndroidRuntime::startVm(JavaVM** pJavaVM, JNIEnv** pEnv, bool zygote, bool primary_zygote)
            * if (JNI_CreateJavaVM(pJavaVM, pEnv, &initArgs) < 0) 
              * jint JNI_CreateJavaVM(JavaVM** p_vm, JNIEnv** p_env, void* vm_args)
                * jint JniInvocationImpl::JNI_CreateJavaVM(JavaVM** p_vm, JNIEnv** p_env, void* vm_args)
                  * return JNI_CreateJavaVM_(p_vm, p_env, vm_args);
                    * if (!FindSymbol(reinterpret_cast<FUNC_POINTER*>(&JNI_CreateJavaVM_), "JNI_CreateJavaVM")) 
                      * art/runtime/jni/java_vm_ext.cc
                        * extern "C" jint JNI_CreateJavaVM(JavaVM** p_vm, JNIEnv** p_env, void* vm_args) 
        * onVmCreated(env);
          * base/cmds/app_process/app_main.cpp
            * virtual void onVmCreated(JNIEnv* env)
              * mClass = reinterpret_cast<jclass>(env->NewGlobalRef(mClass));
        * if (startReg(env) < 0)  // 注册jni方法
          * int AndroidRuntime::startReg(JNIEnv* env)
            * if (register_jni_procs(gRegJNI, NELEM(gRegJNI), env) < 0)  //遍历注册jni 方法
    * base/core/java/com/android/internal/os/ZygoteInit.java 
      * public static void main(String argv[])  
        * firstpreload(bootTimingsTraceLog);
          * static void firstpreload(TimingsTraceLog bootTimingsTraceLog)
            * nativePreloadAppProcessHALs();
              * frameworks/base/core/jni/com_android_internal_os_ZygoteInit.cpp 
                * void android_internal_os_ZygoteInit_nativePreloadAppProcessHALs(JNIEnv* env, jclass) 
                  * android::GraphicBufferMapper::preloadHal(); // graphicbuffer 预加载
                    * void GraphicBufferMapper::preloadHal() 
                      * Gralloc2Mapper::preload();
                      * Gralloc3Mapper::preload();
                      * Gralloc4Mapper::preload();
            * maybePreloadGraphicsDriver();
              * nativePreloadGraphicsDriver();
                * void android_internal_os_ZygoteInit_nativePreloadGraphicsDriver(JNIEnv* env, jclass)
                  * zygote_preload_graphics();
                    * void zygote_preload_graphics()
                      * eglGetDisplay(EGL_DEFAULT_DISPLAY);
            * preloadSharedLibraries();
              * private static void preloadSharedLibraries()
                * System.loadLibrary("android");
                * System.loadLibrary("compiler_rt"); 
                * System.loadLibrary("jnigraphics");
                * System.loadLibrary("sfplugin_ccodec");
            * preloadTextResources();
              * private static void preloadTextResources() 
                * Hyphenator.init();
                * TextView.preloadFontCache();
        * zygoteServer = new ZygoteServer(isPrimaryZygote);  //构造zygoteServer实例,创建进程间通信的socket
        * if (startSystemServer)
          * Runnable r = forkSystemServer(abiList, zygoteSocketName, zygoteServer); // 创建systemserver
            * private static Runnable forkSystemServer(String abiList, String socketName, ZygoteServer zygoteServer) 
              * pid = Zygote.forkSystemServer(
                * frameworks/base/core/java/com/android/internal/os/Zygote.java
                  * int pid = nativeForkSystemServer(uid, gid, gids, runtimeFlags, rlimits, permittedCapabilities, effectiveCapabilities);
                    * static jint com_android_internal_os_Zygote_nativeForkSystemServer
              * return handleSystemServerProcess(parsedArgs);
                * private static Runnable handleSystemServerProcess(ZygoteArguments parsedArgs) 
                  * return ZygoteInit.zygoteInit(parsedArgs.mTargetSdkVersion, parsedArgs.mDisabledCompatChanges, parsedArgs.mRemainingArgs, cl);
                    * public static final Runnable zygoteInit(int targetSdkVersion, long[] disabledCompatChanges, String[] argv, ClassLoader classLoader)
                      * RuntimeInit.commonInit();
                      * ZygoteInit.nativeZygoteInit();
                      * return RuntimeInit.applicationInit(targetSdkVersion, disabledCompatChanges, argv, classLoader); 
          * r.run();
            * frameworks/base/services/java/com/android/server/SystemServer.java 
              * private void run() 
                * createSystemContext();  //Initialize the system context.
                * mSystemServiceManager = new SystemServiceManager(mSystemContext);
                * LocalServices.addService(SystemServiceManager.class, mSystemServiceManager); 
                * startBootstrapServices(t);  // 开始各种服务
                  * private void startBootstrapServices(@NonNull TimingsTraceAndSlog t)  //这里在服务开始前会添加trace可以用于分析服务启动时间，用于开机时间优化  
                    * mSystemServiceManager.startService(FileIntegrityService.class);
                    * Installer installer = mSystemServiceManager.startService(Installer.class);
                    * mActivityManagerService = ActivityManagerService.Lifecycle.startService(mSystemServiceManager, atm);
                    * mDataLoaderManagerService = mSystemServiceManager.startService(DataLoaderManagerService.class);
                    * mPowerManagerService = mSystemServiceManager.startService(PowerManagerService.class);
                    * mSystemServiceManager.startService(RecoverySystemService.Lifecycle.class);
                    * mDisplayManagerService = mSystemServiceManager.startService(DisplayManagerService.class);
                * startCoreServices(t);
                  * private void startCoreServices(@NonNull TimingsTraceAndSlog t)  // 这里只摘选部分服务
                    * mSystemServiceManager.startService(SystemConfigService.class);
                    * mSystemServiceManager.startService(BatteryService.class);
                    * mSystemServiceManager.startService(GpuService.class);
                * startOtherServices(t);
                  * private void startOtherServices(@NonNull TimingsTraceAndSlog t) 
                    * mSystemServiceManager.startService(ACCOUNT_SERVICE_CLASS);
                    * mSystemServiceManager.startService(CONTENT_SERVICE_CLASS);
                    * ServiceManager.addService("dynamic_system", dynamicSystem);
                    * ServiceManager.addService(Context.WINDOW_SERVICE, wm, /* allowIsolated= */ false, DUMP_FLAG_PRIORITY_CRITICAL | DUMP_FLAG_PROTO);
                    * startContentCaptureService(context, t);
                    * startAttentionService(context, t);
                    * mSystemServiceManager.startService(NetworkScoreService.Lifecycle.class);
                    * mSystemServiceManager.startService(ETHERNET_SERVICE_CLASS);
                    * mSystemServiceManager.startService(ADB_SERVICE_CLASS);
                    * mSystemServiceManager.startService(LauncherAppsService.class);
                    * mActivityManagerService.systemReady(()
                      * public void systemReady(final Runnable goingCallback, @NonNull TimingsTraceAndSlog t)
                        * mActivityTaskManager.onSystemReady();
                        * mUserController.onSystemReady();
                        * mAppOpsService.systemReady();
                        * mProcessList.onSystemReady();
                        * mAtmInternal.startHomeOnAllDisplays(currentUserId, "systemReady");
                          * frameworks/base/services/core/java/com/android/server/wm/ActivityTaskManagerService.java 
                            * public boolean startHomeOnAllDisplays(int userId, String reason)
                              * return mRootWindowContainer.startHomeOnAllDisplays(userId, reason);
                                * frameworks/base/services/core/java/com/android/server/wm/RootWindowContainer.java
                                  * boolean startHomeOnAllDisplays(int userId, String reason) 
                                    * homeStarted |= startHomeOnDisplay(userId, reason, displayId);
                                      * boolean startHomeOnDisplay(int userId, String reason, int displayId) 
                                        * return startHomeOnDisplay(userId, reason, displayId, false /* allowInstrumenting */, false /* fromHomeKey */);
                                          * boolean startHomeOnDisplay(int userId, String reason, int displayId, boolean allowInstrumenting, boolean fromHomeKey) 
                                            * result |= startHomeOnTaskDisplayArea(userId, reason, taskDisplayArea, allowInstrumenting, fromHomeKey);
                                              * boolean startHomeOnTaskDisplayArea(int userId, String reason, TaskDisplayArea taskDisplayArea, boolean allowInstrumenting, boolean fromHomeKey) 
                                                * mService.getActivityStartController().startHomeActivity(homeIntent, aInfo, myReason, taskDisplayArea);
                                                  * frameworks/base/services/core/java/com/android/server/wm/ActivityStartController.java 
                                                    * void startHomeActivity(Intent intent, ActivityInfo aInfo, String reason, TaskDisplayArea taskDisplayArea)
                                                      * mLastHomeActivityStartResult = obtainStarter(intent, "startHomeActivity: " + reason)..setOutActivity(tmpOutRecord).setCallingUid(0).setActivityInfo(aInfo).setActivityOptions(options.toBundle()).execute();
                      * startSystemUi(context, windowManagerF);                        
        * caller = zygoteServer.runSelectLoop(abiList); //
          * Runnable runSelectLoop(String abiList) 
            * while (true) {   // 这里是个死循环
```

# 总结
  
  1、zygote会通过forkSystemServe来创建SystemServer进程。
  2、之后通过handleSystemServerProcess来启动SystemServer进程。
  3、启动过程是通过反射调用SystemServer的main函数。
  4、SystemServer的main函数会调用run方法，在run方法中初始化了主线程的looper、系统的context等，之后启动了引导、核心、其他等三类系统服务。
  5、systemserver 抛出systemReady,启动launcher、systemui

# 启动一个新的进程流程

1、在一开始的时候，我们对mZygoteSocket的FD监听POLLIN事件（此时socketFDs 列表里面只有mZygoteSocket对象）。
2、另一个进程A通过ZygoteProcess.connect这个方法，来创建一个连接并发出消息（此时调用了Os.socket，创建Socket连接）。
3、Os.poll触发，此时一定是触发if (pollIndex == 0) 的条件，然后创建一个新的Zygote连接（此时调用了Os.accept）。
4、进程A通过在Zygote.connect这个方法的流程中，调用LocalSocketlmpl.connectLocal方法，确认连接。
5、然后第二次监听POLLIN的时候，socketFDs里面就有mZygoteSocket和我们在刚刚创建的新连接。
6、此时触发的是else if (pollIndex < usapPoolEventFDIndex) ，去fork一个新的进程，并且找到对应的main方法运行，子进程就算创建完成了。


# 孵化应用进程这种事为什么不交给SystemServer来做，而专门设计一个Zygote？

我们知道，应用在启动的时候需要做很多准备工作，包括启动虚拟机，加载各类系统资源等等，这些都是非常耗时的，如果能在zygote里就给这些必要的初始化工作做好，子进程在fork的时候就能直接共享，那么这样的话效率就会非常高。这个就是zygote存在的价值，这一点呢SystemServer是替代不了的，主要是因为SystemServer里跑了一堆系统服务，这些是不能继承到应用进程的。而且我们应用进程在启动的时候，内存空间除了必要的资源外，最好是干干净净的，不要继承一堆乱七八糟的东西。所以呢，不如给SystemServer和应用进程里都要用到的资源抽出来单独放在一个进程里，也就是这的zygote进程，然后zygote进程再分别孵化出SystemServer进程和应用进程。孵化出来之后，SystemServer进程和应用进程就可以各干各的事了。

# Zygote的IPC通信机制为什么不采用binder？如果采用binder的话会有什么问题么？

第一个原因，我们可以设想一下采用binder调用的话该怎么做，首先zygote要启用binder机制，需要打开binder驱动，获得一个描述符，再通过mmap进行内存映射，还要注册binder线程，这还不够，还要创建一个binder对象注册到serviceManager，另外AMS要向zygote发起创建应用进程请求的话，要先从serviceManager查询zygote的binder对象，然后再发起binder调用，这来来回回好几趟非常繁琐，相比之下，zygote和SystemServer进程本来就是父子关系，对于简单的消息通信，用管道或者socket非常方便省事。第二个原因，如果zygote启用binder机制，再fork出SystemServer，那么SystemServer就会继承了zygote的描述符以及映射的内存，这两个进程在binder驱动层就会共用一套数据结构，这显然是不行的，所以还得先给原来的旧的描述符关掉，再重新启用一遍binder机制，这个就是自找麻烦了。

# android 如何启动一个app

1、当我们点击Launcher上的应用图标后，会产生input事件。这些input事件会先经过SystemServer里的2个native循环线程进行读取分发，分别是InputReader负责读取input事件，InputDispatcher负责分发input事件，最后会分发给Launcher来处理。
2、Launcher收到input事件后，会去调用AMS.startActivity()来启动新的应用进程，这期间会先让Launcher进入Pause状态，接着通过socket通信，通知Zygote进程去fork新的应用进程
3、Zygote接收到AMS传过来的信息后，就会去fork应用进程，并进行应用进程的初始化，体现在ZygoteInit.zygoteInit（）方法里：