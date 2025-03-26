# Vibrator Driver

分析vibrator驱动处理原理

# 驱动工作原理

```
* drivers/misc/mediatek/vibrator/vibrator_drv.c
  └── module_init(vib_mod_init);
      └── static struct platform_driver vibrator_driver
          └── .probe = vib_probe,
              └── static int vib_probe(struct platform_device *pdev)
                  ├── ret = devm_led_classdev_register(&pdev->dev, &led_vibr);
                  │   └── static struct led_classdev led_vibr
                  │       ├── .name       = "vibrator",
                  │       └── .groups     = vibr_group,
                  │           └── static const struct attribute_group *vibr_group[]
                  │               └── &activate_group,
                  │                   └── static struct attribute_group activate_group
                  │                       └── .attrs = activate_attrs,
                  │                           └── static struct attribute *activate_attrs[]
                  │                               └── &dev_attr_activate.attr,
                  │                                   └── static DEVICE_ATTR(activate, 0644, vibr_activate_show, vibr_activate_store);
                  │                                       └── vibr_activate_store
                  │                                           └── static ssize_t vibr_activate_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
                  │                                               └── vibrator_enable(dur, activate);
                  │                                                   ├── hrtimer_cancel(&g_mt_vib->vibr_timer);
                  │                                                   ├── atomic_set(&g_mt_vib->vibr_state, 1);
                  │                                                   ├── queue_work(g_mt_vib->vibr_queue, &g_mt_vib->vibr_onwork);
                  │                                                   │   └── vibr_Enable();
                  │                                                   │       └── vibr_Enable_HW();
                  │                                                   │           └── drivers/misc/mediatek/vibrator/mt6765/vibrator.c
                  │                                                   │               └── void vibr_Enable_HW(void)
                  │                                                   │                   └── pmic_set_register_value(PMIC_RG_LDO_VIBR_EN, 1);
                  │                                                   └── hrtimer_start(&g_mt_vib->vibr_timer, ktime_set(dur / 1000, (dur % 1000) * 1000000), HRTIMER_MODE_REL);
                  │                                                       └── static enum hrtimer_restart vibrator_timer_func(struct hrtimer *timer)
                  │                                                           ├── atomic_set(&vibr->vibr_state, 0);
                  │                                                           └── queue_work(vibr->vibr_queue, &vibr->vibr_offwork);
                  ├── hrtimer_init(&vibr->vibr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
                  └── vibr->vibr_timer.function = vibrator_timer_func;
                      └── vibrator_timer_func
                          └── static enum hrtimer_restart vibrator_timer_func(struct hrtimer *timer)
                              ├── atomic_set(&vibr->vibr_state, 0);
                              └── queue_work(vibr->vibr_queue, &vibr->vibr_offwork);
```
实际使用的功能驱动：drivers/leds/regulator-vibrator.c