# MTK Sensor bringup

MTK Sensor bring up

# menu 

# 参考文档

* [联发科 （MTK） sensor bring up](https://blog.csdn.net/jxx1823862645/article/details/123110787)
* [MTK sensorhub3.0 sensor bringup](https://blog.csdn.net/xjh24778451/article/details/132555063)
* [MTK Sensor框架 及信息传递详解](https://blog.csdn.net/wh2526422/article/details/120786350)
* [MTK sensor 框架](https://blog.csdn.net/wangjun7121/article/details/61916427?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522169984323616777224447646%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fblog.%2522%257D&request_id=169984323616777224447646&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~blog~first_rank_ecpm_v1~rank_v31_ecpm-1-61916427-null-null.nonecase&utm_term=SENSOR&spm=1018.2226.3001.4450)
* [0002_MTK_Sensors_Customer_Document_v1.0.3.pdf](0002_MTK_Sensors_Customer_Document_v1.0.3.pdf)
* [MTK 平台sensor arch 介绍-hal](https://blog.csdn.net/jxx1823862645/article/details/125059469)

# MTK sensor 架构

  MTK Sensor 分为 AP 和 SCP 两大部分，AP (CA5x ，CA7x 系列 主芯片)， SCP（CM4）协处理器。负责处理 Sensor 数据。实际使用中，也可以关闭 SCP 只走 AP 这路实现 sensor 功能。
  器件是挂载在AP侧还是在SCP侧，需要从原理图上进行确认。挂在AP侧与SCP 侧的驱动是有差异的，需要确认了才能同FAE 要相对应的驱动。
