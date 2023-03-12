# i2ctool

i2c tool 介绍

# i2cdetect

* 检测I2C总线上是否挂载了设备
```
i2cdetect -r -y 6 //查看总线6 下挂载了哪些设备
```

# i2cdump

* 查看I2C 器件上的寄存器值
```
 i2cdump -f -y 6 0x53 //dump 总线6 下 0x43 这个器件的寄存器
```

# i2cget

* 查看I2C器件上寄存器的地址
```
i2cget -f -y 6 0x53 0x02 //6 表示I2C总线上，0x53设备地址，0x02偏移地址
```

# i2cset

* 设置I2C器件上寄存器的值
```
i2cset -f -y 6 0x53 0x02 0x08 b  //0x08 表示写入值
```