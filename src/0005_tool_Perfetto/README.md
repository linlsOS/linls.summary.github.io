# README

perfetto、systrace主要是依赖内核ftrace，在此基础上加入android定义的msg格式形成atrace

# docs

NO.  |文件名称|摘要
:---:|:--|:--
0011 | [trace-cmd_kernelshark](docs/0011_trace-cmd_kernelshark.md) | kernelshark显示trace-cmd 数据
0010 | [MTK_perfmgr_trace](docs/0010_MTK_perfmgr_trace.md) | HAL层控制内核写入touch ftrace
0009 | [ atrace_irq](docs/0009_%20atrace_irq.md) | atrace irq工作原理分析，从而知道内核的trace string不符合atrace，所以无法被perfetto解析
0008 | [HIDL_Native_Trace](docs/0008_HIDL_Native_Trace.md) | 可执行程序的Trace
0007 | [APP_Trace](docs/0007_APP_Trace.md) | APP中加入自定义trace
0006 | [ printk_console分析](docs/0006_%20printk_console分析.md) | 根据printk console理解ftrace原理
0005 | [手动抓取ftrace_event](docs/0005_手动抓取ftrace_event.md) | 手动操作一下获取ftrace event并转换到kernelshark显示
0004 | [ftrace_I2C](docs/0004_ftrace_I2C.md) | 使用ftrace观察I2C，EEPROM是扩展板上的
0003 | [trace-cmd](docs/0003_trace-cmd.md) | 理解trace-cmd工作原理
0002 | [atrace](docs/0002_atrace.md) | atrace后台抓取trace数据方法
0001 | [systrace](docs/0001_systrace.md) | Platform-Tools无systrace，对应的Android后台时atrace
