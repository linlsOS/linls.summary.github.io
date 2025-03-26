# MTK perfmgr trace

HAL层控制内核写入touch ftrace

# 参考文档

 * [android性能分析之Systrace](https://blog.csdn.net/zhzhangnews/article/details/106135403)

# trace log

```
  mtkPowerMsgHdl-749   (  621) [000] ...1 79281.114408: tracing_mark_write: C|749|touch|1
    kworker/u8:3-17292 (17292) [000] ...1 79281.214407: tracing_mark_write: C|749|touch|0
  mtkPowerMsgHdl-749   (  621) [002] ...1 79283.641542: tracing_mark_write: C|749|touch|1
    kworker/u8:6-17555 (17555) [000] ...1 79283.741559: tracing_mark_write: C|749|touch|0
  mtkPowerMsgHdl-749   (  621) [001] ...1 79285.497328: tracing_mark_write: C|749|touch|1
    kworker/u8:3-17292 (17292) [000] ...1 79285.597355: tracing_mark_write: C|749|touch|0
  mtkPowerMsgHdl-749   (  621) [003] ...1 79294.733903: tracing_mark_write: C|749|touch|1
    kworker/u8:3-17292 (17292) [002] ...1 79294.834089: tracing_mark_write: C|749|touch|0
```

# kernel perfmgr
```
* kernel-4.19/drivers/misc/mediatek/performance/perfmgr_main.c
  └── device_initcall(init_perfmgr);
      └── static int __init init_perfmgr(void)
          └── perfmgr_root = proc_mkdir("perfmgr", NULL);
              └── init_perfctl(perfmgr_root);
                  └── kernel-4.19/drivers/misc/mediatek/performance/perf_ioctl/perf_ioctl.c
                      └── int init_perfctl(struct proc_dir_entry *parent)
                          └── pe = proc_create("perf_ioctl", 0664, parent, &Fops);
                              └── static const struct file_operations Fops
                                  └── .compat_ioctl = device_ioctl,
                                      └── static long device_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
                                          └── switch (cmd)
                                              └── case FPSGO_TOUCH:
                                                  └── usrtch_ioctl(cmd, msgKM->frame_time);
                                                      └── kernel-4.19/drivers/misc/mediatek/performance/tchbst/user/utch.c
                                                          └── long usrtch_ioctl(unsigned int cmd, unsigned long arg)
                                                              └── switch (cmd)
                                                                  └── case FPSGO_TOUCH:
                                                                      └── ret = notify_touch(arg);
                                                                          └── if (action == 1)
                                                                              └── perfmgr_trace_count(1, "touch");
                                                                                  └── kernel-4.19/drivers/misc/mediatek/performance/base/utility.c
                                                                                      └── void perfmgr_trace_count(int val, const char *fmt, ...)
```

perfmgr_trace_count实现
```
// kernel-4.19/drivers/misc/mediatek/performance/base/utility.c

static unsigned long __read_mostly tracing_mark_write_addr;
static inline void __mt_update_tracing_mark_write_addr(void)
{
    if (unlikely(tracing_mark_write_addr == 0))
        tracing_mark_write_addr =
            kallsyms_lookup_name("tracing_mark_write");
}

void perfmgr_trace_count(int val, const char *fmt, ...)
{
    char log[128];
    va_list args;
    int len;

    if (powerhal_tid <= 0)
        return;
    
    memset(log, ' ', sizeof(log));
    va_start(args, fmt); 
    len = vsnprintf(log, sizeof(log), fmt, args);
    va_end(args);
    
    if (unlikely(len < 0))
        return;
    else if (unlikely(len == 128))
        log[127] = '\0';

    __mt_update_tracing_mark_write_addr();
    preempt_disable();

    event_trace_printk(tracing_mark_write_addr, "C|%d|%s|%d\n",
        powerhal_tid, log, val);

    preempt_enable();
}
```

# FPSGO_TOUCH
```
* vendor/mediatek/proprietary/hardware/power/lib/powerhal/perfservice_rsccfgtbl.h
  └── tRscConfig RscCfgTbl[]
      └── {}
          ├── .cmdID				  = PERF_RES_POWERHAL_TOUCH_BOOST_NOTIFY_FBC,
          ├── .cmdName			  = "PERF_RES_POWERHAL_TOUCH_BOOST_NOTIFY_FBC",
          └── .set_func			  = notifyFbcTouch,
              └── vendor/mediatek/proprietary/hardware/power/lib/powerhal/rsc_util/utility_fps.cpp
                  └── int notifyFbcTouch(int event, void *scn)
                      └── if (event)
                          └── fbcNotifyTouch(1);
                              └── vendor/mediatek/proprietary/external/performance/perfctl.cpp
                                  └── int fbcNotifyTouch(int enable)
                                      ├── check_perf_ioctl_valid()
                                      │   └── devfd = open(PATH_PERF_IOCTL, O_RDONLY);
                                      │       └── #define PATH_PERF_IOCTL "/proc/perfmgr/perf_ioctl"
                                      └── ioctl(devfd, FPSGO_TOUCH, &msg);
```
