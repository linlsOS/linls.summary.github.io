# sensor bringup

sensor bringup

# menu

* [参考文档](#参考文档)
* [sensor bring up步骤](#sensor-bring-up步骤)

# 参考文档

* [Sensor bringup 中的一些问题总结](https://zhuanlan.zhihu.com/p/537527289)
* [联发科 （MTK） sensor bring up](https://blog.csdn.net/jxx1823862645/article/details/123110787)
* [高通（QCOM）sensor bring up](https://blog.csdn.net/jxx1823862645/article/details/123133230)
* [MTK sensorhub3.0 sensor bringup](https://blog.csdn.net/xjh24778451/article/details/132555063)
* [MTK Sensor框架 及信息传递详解](https://blog.csdn.net/wh2526422/article/details/120786350)
* [MTK sensor 框架](https://blog.csdn.net/wangjun7121/article/details/61916427?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522169984323616777224447646%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fblog.%2522%257D&request_id=169984323616777224447646&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~blog~first_rank_ecpm_v1~rank_v31_ecpm-1-61916427-null-null.nonecase&utm_term=SENSOR&spm=1018.2226.3001.4450)
* [0001_MTK_Sensors_Customer_Document_v1.0.3.pdf](0001_MTK_Sensors_Customer_Document_v1.0.3.pdf)
* [MTK 平台sensor arch 介绍-hal](https://blog.csdn.net/jxx1823862645/article/details/125059469)


# 器件 bring up步骤

* 学会看电路图、原理图  
  这是地层驱动基础必学的技能，需要知道同这个器件挂在哪儿、哪条I2C总线、供电引脚是否供电，另外在porting驱动后如果器件挂载不成功也需要确认是软件问题还是硬件问题

* 查看器件对应规格书  
  一般器件都会有对应的规格书，这些规格书我们可以从官网或者找对应器件的FAE、原厂提供。规格书中包含I2C地址、VCC电压、中断、GPIO控制等信息，此部分的配置都需要通过规格书、原理图进行确认

* porting 驱动
  一般器件都会由FAE提供驱动，提供平台信息、系统版本等信息FAE提供匹配度较高的驱动合入代码进行编译，调试。

* 验证
  器件器件功能是否OK
  在驱动合入后更新镜像，看开机日志查看器件是否正常加载，日志是否有跟器件相关的错误。另外部分器件可以通过命令查看当前器件是否已经成功加载。

# 常见故障排除
  * 确认硬件提高的器件同原理图是否是相同的，之前遇到硬件贴的器件同原理图不同（不同器件I2C地址可能不同）
  * 确认I2C地址是否正确，可以通过i2ctool 查看对应的I2C总线下是否有挂载对应I2C 地址的设备
  * 确认部分应该供电的是否正常供电了，用万用表测量对应的点位确认供电是否正常
  * 确认I2C 通信是否正常，这里需要硬件协助确认I2C的clk跟data数据是否同规格书匹配
  * 请硬件协助确认器件焊接是否正常

# MTK sensor 架构

  MTK Sensor 分为 AP 和 SCP 两大部分，AP (CA5x ，CA7x 系列 主芯片)， SCP（CM4）协处理器。负责处理 Sensor 数据。实际使用中，也可以关闭 SCP 只走 AP 这路实现 sensor 功能。
  器件是挂载在AP侧还是在SCP侧，需要从原理图上进行确认。挂在AP侧与SCP 侧的驱动是有差异的，需要确认了才能同FAE 要相对应的驱动。