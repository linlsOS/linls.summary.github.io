#  printk console分析

根据printk console理解ftrace原理

# 参考文档

* [ftrace - Function Tracer](https://www.kernel.org/doc/Documentation/trace/ftrace.txt)
* [在LINUX KERNEL中添加私有的FTRACE EVENT](https://www.freesion.com/article/452185539/)
  * 由于是完全添加自己的trace point，所以在include/trace/events/目录下添加自己的event文件，我命名为：test_driver.h，代码如下：
  * 可以看到在定义trace的时候，有两种方式：
    * 一种是类，class，这种使用在，如果你定义的多个时间的信息(参数)是相同，但是可能是不同的时间点或者某种上下文前后，就可以使用，比单独定义event要省事些
    * 一种是直接定义event。目的很单纯的获取一种都有的信息。
  * /sys/kernel/debug/tracing/events/test_driver/
* [linux trace point机制](https://blog.csdn.net/geshifei/category_12247566.html)
* [trace系列3 - trace event学习笔记](https://blog.csdn.net/jasonactions/article/details/121098704)
* [Linux TraceEvent - 我见过的史上最长宏定义](https://blog.csdn.net/21cnbao/article/details/106913378)

# 引入trace point的背景

当需要获取内核的debug信息时，通常你会通过以下printk的方式打印信息：

```
void trace_func() {
    // ...省略

    printk(输出信息)

    // ...省略
}
```
缺点:
  * 内核中printk是统一控制的，各个模块的printk都会被打印，无法只打印需要关注的模块。
  * 如果需要修改/新增打印信息，需要修改所有受影响的printk语句。这些printk分散在代码多处，每个地方都需要修改。
  * 嵌入式系统中，如果printk信息量大，console（如果有）有大量的打印输出，用户无法在console输入命令，影响人机交互。

# 内核解决方案

内核采用”插桩”的方法抓取log，”插桩”也称为trace point。每种trace point有一个name、一个enable开关、一系列桩函数、注册桩函数的函数、卸载桩函数的函数。”桩函数”功能类似于printk，不过”桩函数”并不会把信息打印到console，而是输出到内核的ring buffer（环形缓冲区），缓冲区中的信息通过debugfs对用户呈现。

<!-- table 2*3 -->
NO.   | 数据结构                                 | 代码路径
------|-----------------------------------------|------
 1    | * DEFINE_TRACE(name)
        * DECLARE_TRACE(name, proto, args)      |include/linux/tracepoint.h      
 2    | struct tracepoint                       |include/linux/tracepoint-defs.h

 * trace point依次执行桩函数，每个桩函数实现不同的debug功能。内核通过register_trace_##name将桩函数添加到trace point中，通过unregister_trace_##name从trace point中移除。（注：##表示字符串连接）。
 * 内核通过DEFINE_TRACE(name)定义struct tracepoint变量来描述trace point。
 
 ```
 struct tracepoint {
    const char *name;       /* Tracepoint name */
    struct static_key key;
    int (*regfunc)(void);
    void (*unregfunc)(void);
    struct tracepoint_func __rcu *funcs;
};
 ```   
  * name: trace point的名字，内核中通过hash表管理所有的trace point，找到对应的hash slot后，需要通过name来识别具体的trace point。
  * key: trace point状态，1表示disable，0表示enable。
  * regfunc: 添加桩函数的函数
  * unregfunc: 卸载桩函数的函数
  * funcs: trace point中所有的桩函数链表

内核通过#define DECLARE_TRACE(name, proto, args)定义trace point用到的函数，定义的函数原型如下（从代码中摘取了几个，不止以下3个）：
```
#define __DECLARE_TRACE(name, proto, args, cond, data_proto, data_args)
extern struct tracepoint __tracepoint_##name;
static inline void trace_##name(proto)
{
        if (static_key_false(&__tracepoint_##name.key))
                        __DO_TRACE(&__tracepoint_##name,
                        TP_PROTO(data_proto),
                        TP_ARGS(data_args),
                        TP_CONDITION(cond),,);
 }
 register_trace_##name(void (*probe)(data_proto), void *data)
 {      
         return tracepoint_probe_register(&__tracepoint_##name,
                                                 (void *)probe, data);
 }
 unregister_trace_##name(void (*probe)(data_proto), void *data)
 {
         return tracepoint_probe_unregister(&__tracepoint_##name,(void *)probe, data);
 }
```
使用trace point必须要通过register_trace_##name将桩函数（也就是我们需要的debug函数）添加到trace point中，这个工作只能通过moudule或者修改内核代码实现，对于开发者来说，操作比较麻烦。ftrace开发者们意识到了这点，所以提供了trace event功能，开发者不需要自己去注册桩函数了，易用性较好

# trace_events set_event处理流程
```
* kernel-4.9/kernel/trace/trace_events.c
  └── fs_initcall(event_trace_init);
      └── static __init int event_trace_init(void)
          └── ret = early_event_add_tracer(d_tracer, tr);
              └── ret = create_event_toplevel_files(parent, tr);
                  └── entry = tracefs_create_file("set_event", 0644, parent, tr, &ftrace_set_event_fops);
                      └── static const struct file_operations ftrace_set_event_fops
                          ├── .read = seq_read,
                          └── .write = ftrace_event_write,
                              └── static ssize_t ftrace_event_write(struct file *file, const char __user *ubuf, size_t cnt, loff_t *ppos)
                                  └── ret = ftrace_set_clr_event(tr, parser.buffer + !set, set);
                                      ├── match = strsep(&buf, ":");        --> match: printk, event: console
                                      └── ret = __ftrace_set_clr_event(tr, match, sub, event, set);
                                          └── ret = __ftrace_set_clr_event_nolock(tr, match, sub, event, set);
                                              └── list_for_each_entry(file, &tr->events, list)
                                                  ├── call = file->event_call;
                                                  ├── name = trace_event_name(call);
                                                  └── ret = ftrace_event_enable_disable(file, set);
                                                      └── return __ftrace_event_enable_disable(file, enable, 0);
                                                          └── case 1:
                                                              └── ret = call->class->reg(call, TRACE_REG_REGISTER, file);
                                                                  └── int trace_event_reg(struct trace_event_call *call, enum trace_reg type, void *data)
                                                                      └── switch (type)
                                                                          └── case TRACE_REG_REGISTER:
                                                                              └── return tracepoint_probe_register(call->tp, call->class->probe, file);
```

set_event对应的trace_parser介绍如下，每个event通过空格隔开
```
/*
 * struct trace_parser - servers for reading the user input separated by spaces
 * @cont: set if the input is not complete - no final space char was found
 * @buffer: holds the parsed user input
 * @idx: user input length
 * @size: buffer size
 */
struct trace_parser {
        bool            cont;
        char            *buffer;
        unsigned        idx;
        unsigned        size;
};
```

printk:console格式解析说明
```
// kernel-4.9/kernel/trace/trace_events.c

static int ftrace_set_clr_event(struct trace_array *tr, char *buf, int set)
{
        char *event = NULL, *sub = NULL, *match;
        int ret;

        /*
         * The buf format can be <subsystem>:<event-name>
         *  *:<event-name> means any event by that name.
         *  :<event-name> is the same.
         *
         *  <subsystem>:* means all events in that subsystem
         *  <subsystem>: means the same.
         *
         *  <name> (no ':') means all events in a subsystem with
         *  the name <name> or any event that matches <name>
         */

        match = strsep(&buf, ":");
        if (buf) {
                sub = match;
                event = buf;
                match = NULL;

                if (!strlen(sub) || strcmp(sub, "*") == 0)
                        sub = NULL;
                if (!strlen(event) || strcmp(event, "*") == 0)
                        event = NULL;
        }

        ret = __ftrace_set_clr_event(tr, match, sub, event, set);

        /* Put back the colon to allow this to be called again */
        if (buf)
                *(buf - 1) = ':';

        return ret;
}
```

printk:console定义
```
// kernel-4.9/include/trace/events/printk.h

#undef TRACE_SYSTEM
#define TRACE_SYSTEM printk

#if !defined(_TRACE_PRINTK_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_PRINTK_H

#include <linux/tracepoint.h>

TRACE_EVENT(console,
        TP_PROTO(const char *text, size_t len),

        TP_ARGS(text, len),

        TP_STRUCT__entry(
                __dynamic_array(char, msg, len + 1)
        ),

        TP_fast_assign(
                /*
                 * Each trace entry is printed in a new line.
                 * If the msg finishes with '\n', cut it off
                 * to avoid blank lines in the trace.
                 */
                if ((len > 0) && (text[len-1] == '\n'))
                        len -= 1;

                memcpy(__get_str(msg), text, len);
                __get_str(msg)[len] = 0;
        ),

        TP_printk("%s", __get_str(msg))
);
#endif /* _TRACE_PRINTK_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
```
* 以TRACE_EVENT分析
  * get_test_driver_data: 函数名，自动加trace_前缀
  * TP_PROTO : 函数参数原型
  * TP_ARGS : 参数名，和TP_PROTO保持一致
  * TP_STRUCT__entry : 结构体声明
  * TP_fast_assign : 参数赋值
  * TP_printk : 字符串输出格式

# TRACE_EVENT

TRACE_EVENT多次扩展原理 
```
* include/trace/events/printk.h                                 --> 第一次加载
  ├── #define TRACE_SYSTEM printk
  ├── TRACE_EVENT(console, TP_PROTO(const char *text, size_t len), ...)
  │   └── include/linux/tracepoint.h
  │       └── #define TRACE_EVENT(name, proto, args, struct, assign, print)   DECLARE_TRACE(name, PARAMS(proto), PARAMS(args))
  │           └── #define DECLARE_TRACE(name, proto, args)                __DECLARE_TRACE(name, PARAMS(proto), PARAMS(args),  ...
  └── #include <trace/define_trace.h>
      └── #ifndef TRACE_INCLUDE_PATH
          └── # define __TRACE_INCLUDE(system) <trace/events/system.h>
              └── include/trace/events/printk.h                 --> 二次加载
                  └── TRACE_EVENT(console, TP_PROTO(const char *text, size_t len), ...)
                      └── include/trace/define_trace.h
                          └── #define TRACE_EVENT(name, proto, args, tstruct, assign, print)   DEFINE_TRACE(name)
                              └── include/linux/tracepoint.h
                                  └── #define DEFINE_TRACE(name)                      DEFINE_TRACE_FN(name, NULL, NULL);
                                      └── #define DEFINE_TRACE_FN(name, reg, unreg)     static const char __tpstrtab_##name[]
```

# trace_console_rcuidle

调用关系
```
* kernel/printk/printk.c
  └── static void call_console_drivers(const char *ext_text, size_t ext_len, const char *text, size_t len)
      └── trace_console_rcuidle(text, len);
```

宏定义
```
* include/trace/events/printk.h
  └── TRACE_EVENT(console, TP_PROTO(const char *text, size_t len), ...)
      └── #define TRACE_EVENT(name, proto, args, struct, assign, print)   DECLARE_TRACE(name, PARAMS(proto), PARAMS(args))
          └── #define DECLARE_TRACE(name, proto, args)      __DECLARE_TRACE(name, PARAMS(proto), PARAMS(args), ...)
              └── #define __DECLARE_TRACE(name, proto, args, cond, data_proto, data_args)  ... __DECLARE_TRACE_RCU(name, PARAMS(proto), PARAMS(args), ...
                  └── #define __DECLARE_TRACE_RCU(name, proto, args, cond, data_proto, data_args) static inline void trace_##name##_rcuidle(proto) ...
```

  

