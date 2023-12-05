# android zygote

android zygote 简介


# menu

# 参考文档

* [0001_android_系统启动流程.md](0001_android_系统启动流程.md)
* [Zygote的启动流程](https://zhuanlan.zhihu.com/p/145565662)
* [深入研究源码：Android10.0系统启动流程（三）：Zygote](https://zhuanlan.zhihu.com/p/350204845)
* [JNI: 连接Java世界的JavaVM和JNIEnv](https://juejin.cn/post/6844903880115879950)
* [Android系统启动过程分析](https://www.cnblogs.com/Robin132929/p/12158006.html)

# 简介

  zygote的启动是init进程通过rc文件拉起的服务，系统启动流程中有介绍init进程如何启动zygote进程的，这里主要介绍zygote在启动过程中执行了什么


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
          r.run();                   
        * caller = zygoteServer.runSelectLoop(abiList); //
          * Runnable runSelectLoop(String abiList) 
```
# 总结

* 思考
* 思考
* 思考
