# atrace

atrace后台抓取trace数据方法

# 参考文档

* [通过 atrace 捕获内核事件](https://source.android.google.cn/docs/core/tests/debug/ftrace?hl=zh-cn#atrace)
* [atrace 源码解析](https://juejin.cn/post/6926806644533755917)
* frameworks/native/cmds/atrace

# atrace

* atrace –help
```
usage: atrace [options] [categories...]
options include:
  -a appname      enable app-level tracing for a comma separated list of cmdlines; * is a wildcard matching any process
  -b N            use a trace buffer size of N KB
  -c              trace into a circular buffer
  -f filename     use the categories written in a file as space-separated
                    values in a line
  -k fname,...    trace the listed kernel functions
  -n              ignore signals
  -s N            sleep for N seconds before tracing [default 0]
  -t N            trace for N seconds [default 5]
  -z              compress the trace dump
  --async_start   start circular trace and return immediately
  --async_dump    dump the current contents of circular trace buffer
  --async_stop    stop tracing and dump the current contents of circular
                    trace buffer
  --stream        stream trace to stdout as it enters the trace buffer
                    Note: this can take significant CPU time, and is best
                    used for measuring things that are not affected by
                    CPU performance, like pagecache usage.
  --list_categories
                  list the available tracing categories
 -o filename      write the trace to the specified file instead
                    of stdout.
```

* 调用关系

```
* frameworks/native/cmds/atrace/atrace.cpp
  * int main(int argc, char **argv)
    * if (argc == 2 && 0 == strcmp(argv[1], "--help")) 
      * showHelp(argv[0]);
        * fprintf(stderr, "usage: %s [options] [categories...]\n", cmd);
```

* atrace –list_categories

```
         gfx - Graphics
       input - Input
        view - View System
     webview - WebView
          wm - Window Manager
          am - Activity Manager
          sm - Sync Manager
       audio - Audio
       video - Video
      camera - Camera
         hal - Hardware Modules
         res - Resource Loading
      dalvik - Dalvik VM
          rs - RenderScript
      bionic - Bionic C Library
       power - Power Management
          pm - Package Manager
          ss - System Server
    database - Database
     network - Network
         adb - ADB
    vibrator - Vibrator
        aidl - AIDL calls
       nnapi - NNAPI
         rro - Runtime Resource Overlay
         pdx - PDX services
       sched - CPU Scheduling
         irq - IRQ Events
         i2c - I2C Events
        freq - CPU Frequency
        idle - CPU Idle
        disk - Disk I/O
         mmc - eMMC commands
        sync - Synchronization
       workq - Kernel Workqueues
  memreclaim - Kernel Memory Reclaim
  regulators - Voltage and Current Regulators
  binder_driver - Binder Kernel driver
  binder_lock - Binder global lock trace
   pagecache - Page cache
      memory - Memory
     thermal - Thermal event
```

* 调用关系

```
* frameworks/native/cmds/atrace/atrace.cpp
  └── int main(int argc, char **argv)
      └── initVendorCategories();
          ├── g_atraceHal = IAtraceDevice::getService();                                          --> 通过IAtraceDevice HIDL获取
          │   └── Return<void> ret = g_atraceHal->listCategories([](const auto& list){})
          │       ├── for (const auto& category : list)
          │       └── g_vendorCategories.emplace_back(category.name, category.description, false);
          └── for (;;)
              ├── static struct option long_options[]
              │   └── {"list_categories",   no_argument, nullptr,  0 },
              ├── ret = getopt_long(argc, argv, "a:b:cf:k:ns:t:zo:", long_options, &option_index);
              └── switch(ret)
                  └── case 0:
                      └── else if (!strcmp(long_options[option_index].name, "list_categories"))
                          └── listSupportedCategories();
                              ├── for (size_t i = 0; i < arraysize(k_categories); i++)
                              │   ├── const TracingCategory& c = k_categories[i];                                   --> kernel分类
                              │   │   └── static const TracingCategory k_categories[]
                              │   │       ├── { "input",      "Input",                    ATRACE_TAG_INPUT, { } },  --> input类型
                              │   │       │   └── system/core/libcutils/include/cutils/trace.h
                              │   │       │       └── #define ATRACE_TAG_INPUT            (1<<2)
                              │   │       └── { "irq",        "IRQ Events",   0, { { REQ,      "events/irq/enable" }, { OPT,      "events/ipi/enable" },    --> irq分类
                              │   └── if (isCategorySupported(c)) 
                              │       └── printf("  %10s - %s\n", c.name, c.longname);
                              └── for (const auto &c : g_vendorCategories)
                                  └── printf("  %10s - %s (HAL)\n", c.name.c_str(), c.description.c_str());      
```

# 后台运行

```
atrace -z -b 40000 sched freq idle am wm gfx view binder_driver hal dalvik camera input res -t 15 > /data/local/tmp/trace_output &
```
```
adb pull /data/local/tmp/trace_output .
```
```
python systrace.py --from-file trace_output -o output.html
```
```
* frameworks/native/cmds/atrace/atrace.cpp
  └── int main(int argc, char **argv)
      ├── initVendorCategories();
      │   ├── g_atraceHal = IAtraceDevice::getService();                                          --> Categories分内核、SoC芯片定制，通过IAtraceDevice HIDL获取SoC芯片定制
      │   └── Return<void> ret = g_atraceHal->listCategories([](const auto& list){})
      │       ├── for (const auto& category : list)
      │       └── g_vendorCategories.emplace_back(category.name, category.description, false);
      ├── for (;;)
      │   ├── static struct option long_options[]
      │   │   └── {"list_categories",   no_argument, nullptr,  0 },
      │   └── ret = getopt_long(argc, argv, "a:b:cf:k:ns:t:zo:", long_options, &option_index);
      │       └── switch(ret)
      │           └── case 0:
      │               └── else if (!strcmp(long_options[option_index].name, "list_categories"))
      │                   ├── listSupportedCategories();
      │                   │   ├── for (size_t i = 0; i < arraysize(k_categories); i++)
      │                   │   │   └── const TracingCategory& c = k_categories[i];                                   --> kernel分类
      │                   │   │       └── static const TracingCategory k_categories[]
      │                   │   │           ├── { "input",      "Input",                    ATRACE_TAG_INPUT, { } },  --> input类型
      │                   │   │           │   └── system/core/libcutils/include/cutils/trace.h
      │                   │   │           │       └── #define ATRACE_TAG_INPUT            (1<<2)
      │                   │   │           └── { "irq",        "IRQ Events",   0, { { REQ,      "events/irq/enable" }, { OPT,      "events/ipi/enable" },    --> irq分类 
      │                   │   └── if (isCategorySupported(c))
      │                   │       └── printf("  %10s - %s\n", c.name, c.longname);      
      │                   └── for (const auto &c : g_vendorCategories)    
      │                       └── printf("  %10s - %s (HAL)\n", c.name.c_str(), c.description.c_str());
      ├── ok &= setUpUserspaceTracing();
      ├── ok &= setUpKernelTracing();
      ├── ok &= setUpVendorTracing();
      ├── ok &= startTrace();
      ├── stopTrace();
      └── if (ok && traceDump && !onlyUserspace)
          └── if (!g_traceAborted)
              ├── outFd = open(g_outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);                   --> 打开trace记录输出文件
              ├── dumpTrace(outFd);
              │   ├── int traceFD = open((g_traceFolder + k_tracePath).c_str(), O_RDWR);            --> 打开trace输出文件
              │   ├── char buf[4096];
              │   ├── ssize_t rc;
              │   └── while ((rc = TEMP_FAILURE_RETRY(read(traceFD, buf, sizeof(buf)))) > 0)        --> 循环读取
              │       └── android::base::WriteFully(outFd, buf, rc)                                 --> 写入输出文件
              └── close(outFd);                      
```

# trace log 格式

同时每一行的格式需要是
```
<线程名>-<线程id>  <000> ...1 <时间-秒>: tracing_mark_write: <B|E>|<进程id>|<TAG>
```

时间可以通过以下函数获取，但是观察到有100多 ms 的偏差
```
float now_in_seconds = systemTime() / 1000000000.0f;
nsecs_t systemTime()
{
    struct timespec t;
    t.tv_sec = t.tv_nsec = 0;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return nsecs_t(t.tv_sec)*1000000000LL + t.tv_nsec;
}
```

这样就可以通过 hook 对 /sys/kernel/debug/tracing/trace_marker 文件的写入，来得到应用的 trace，再补全格式就能给 systrace.py 进行解析了。

```
TRACE:
# tracer: nop
#
# entries-in-buffer/entries-written: 21/24   #P:1
# enabled events:
# kernel time now: 453985.091953
# UTC time:	2019-07-11 06:34:35.321188
# android time:	2019-07-11 14:34:35.321188
#
#                              _-----=> irqs-off
#                             / _----=> need-resched
#                            | / _---=> hardirq/softirq
#                            || / _--=> preempt-depth
#                            ||| /     delay
#           TASK-PID   CPU#  ||||    TIMESTAMP  FUNCTION
#              | |       |   ||||       |         |
          atrace-4367  [000] ...1 453966.016965: tracing_mark_write: trace_event_clock_sync: parent_ts=453966.187500
          atrace-4367  [000] ...1 453966.016978: tracing_mark_write: trace_event_clock_sync: realtime_ts=1562826856246
  UICompThread_0-270   [000] ...1 453967.735306: tracing_mark_write: S|240|OVL0-DSI|856112
 frame_update_wo-113   [000] ...1 453967.754543: tracing_mark_write: F|113|OVL0-DSI|856112
  UICompThread_0-270   [000] ...1 453969.752747: tracing_mark_write: S|240|OVL0-DSI|856113
 frame_update_wo-113   [000] ...1 453969.772422: tracing_mark_write: F|113|OVL0-DSI|856113
  UICompThread_0-270   [000] ...1 453971.767667: tracing_mark_write: S|240|OVL0-DSI|856114
 frame_update_wo-113   [000] ...1 453971.790263: tracing_mark_write: F|113|OVL0-DSI|856114
  UICompThread_0-270   [000] ...1 453973.768107: tracing_mark_write: S|240|OVL0-DSI|856115
 frame_update_wo-113   [000] ...1 453973.790901: tracing_mark_write: F|113|OVL0-DSI|856115
  UICompThread_0-270   [000] ...1 453975.781464: tracing_mark_write: S|240|OVL0-DSI|856116
 frame_update_wo-113   [000] ...1 453975.808737: tracing_mark_write: F|113|OVL0-DSI|856116
  UICompThread_0-270   [000] ...1 453977.782070: tracing_mark_write: S|240|OVL0-DSI|856117
```

