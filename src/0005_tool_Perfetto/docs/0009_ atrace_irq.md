#  atrace irq

atrace irq工作原理分析，从而知道内核的trace string不符合atrace，所以无法被perfetto解析

# steps

* atrace -b 40000 irq -t 20 > /data/local/tmp/trace_output &
* adb pull /data/local/tmp/trace_output .
* python ~/Downloads/platform-tools_r33.0.0-linux/platform-tools/systrace/systrace.py --from-file trace_output -o output.htm

# atrace irq

k_categories
```
* frameworks/native/cmds/atrace/atrace.cpp
  * static const TracingCategory k_categories[]
    * { "irq",        "IRQ Events",   0, { { REQ,      "events/irq/enable" }, { OPT,      "events/ipi/enable" }, } }
```

# irq trace events
```
* include/trace/events/irq.h
  ├── TRACE_EVENT(irq_handler_entry, ..., TP_printk("irq=%d name=%s", __entry->irq, __get_str(name))
  └── TRACE_EVENT(irq_handler_exit, ..., TP_printk("irq=%d ret=%s", __entry->irq, __entry->ret ? "handled" : "unhandled")
```

# trace使用
```
* kernel/irq/handle.c
  └── irqreturn_t __handle_irq_event_percpu(struct irq_desc *desc, unsigned int *flags)
      └── for_each_action_of_desc(desc, action)
          ├── trace_irq_handler_entry(irq, action);
          ├── res = action->handler(irq, action->dev_id);
          └── trace_irq_handler_exit(irq, action, res);
```

# kernel trace log
```
<idle>-0     (-----) [000] d.h1 79281.108822: irq_handler_entry: irq=125 name=fts_ts
<idle>-0     (-----) [000] dnh1 79281.108888: irq_handler_exit: irq=125 ret=handled
```
* 思考
* 思考
* 思考
