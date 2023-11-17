# mtk g sensor bringup

mtk g sensor bring

# menu

* [1 简介](#1-简介)
* [2 g sensor bring up 代码实现](#2-g-sensor-bring-up-代码实现)
* [3 功能确认](#3-功能确认)
* [4 g sensor校准](#4-g-sensor校准)
  * [4.1 g sensor 为什么需要校准](#41-g-sensor-为什么需要校准)
  * [4.2 g sensor 校准逻辑](#42-g-sensor-校准逻辑)

# 1 简介

  这里介绍g sensor的bring up是 sensorhub侧；    
  g sensor 的校准在平台已经有实现，这里简单介绍实现逻辑；

# 2 g sensor bring up 代码实现

  ```diff
  * 新增编译宏，用于控制新增的器件
  diff --git a/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/cpuinfo/project/ProjectConfig.mk b/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/cpuinfo/project/ProjectConfig.mk
  --- a/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/cpuinfo/project/ProjectConfig.mk
  +++ b/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/cpuinfo/project/ProjectConfig.mk
   #CFG_BMP280_SUPPORT = yes
   CFG_ACCGYRO_SUPPORT = yes
   CFG_MC3416_SUPPORT = yes
  +CFG_BMA4XY_SUPPORT = yes
   CFG_ACC_NO_FIFO = yes
   #CFG_LIS2HH12_SUPPORT = yes
   CFG_STEP_COUNTER_SUPPORT = yes
  
  * 配置I2C地址、默认方向
  diff --git a/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/cpuinfo/project/cust/accGyro/cust_accGyro.c b/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/cpuinfo/project/cust/accGyro/cust_accGyro.c
  --- a/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/cpuinfo/project/cust/accGyro/cust_accGyro.c
  +++ b/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/cpuinfo/project/cust/accGyro/cust_accGyro.c
       {
           .name = "mc34xx",
           .i2c_num = 0,
  -        .direction = 5,
  +        .direction = 0,
           .i2c_addr = {0x4c, 0},
           .eint_num = 12,
       },
   #endif
   
  +#ifdef CFG_BMA4XY_SUPPORT
  +    {
  +        .name = "bma4xy",
  +        .i2c_num = 0,
  +        .direction = 0,
  +        .i2c_addr = {0x18, 0},
  +        .eint_num = 12,
  +    },
  +#endif
  +
   };
  diff --git a/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/cpuinfo/project/cust/overlay/overlay.c b/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/cpuinfo/project/cust/overlay/overlay.c
  --- a/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/cpuinfo/project/cust/overlay/overlay.c
  +++ b/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/cpuinfo/project/cust/overlay/overlay.c                                                       \
   void accGyroOverlayRemap(void)
   {
   ACC_GYRO_OVERLAY_REMAP_START
  +    ACC_GYRO_OVERLAY_REMAP(bma4xy);
       ACC_GYRO_OVERLAY_REMAP(mc34xx);
  +
   ACC_GYRO_OVERLAY_REMAP_END
   
       return;
  diff --git a/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/cpuinfo/project/inc/overlay_sensor.h b/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/cpuinfo/project/inc/overlay_sensor.h
  --- a/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/cpuinfo/project/inc/overlay_sensor.h
  +++ b/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/cpuinfo/project/inc/overlay_sensor.h
   * Overlay0: ACCGYRO
   *****************************************************************************/
   #define OVERLAY_SECTION_ACCGYRO                    \
  +    OVERLAY_ONE_OBJECT(bma4xy, bma4xy)             \
       OVERLAY_ONE_OBJECT(mc34xx, mc34xx)
   
   #ifdef OVERLAY_SECTION_ACCGYRO
  
  * 如果有大块数据需要进行拷贝则需要配置DMA，否则在初始化过程中容易出现各种错误
  
  diff --git a/mediatek/proprietary/tinysys/freertos/source/drivers/CM4_A/cpuinfo/i2c/src/i2cchre-plat.c b/mediatek/proprietary/tinysys/freertos/source/drivers/CM4_A/cpuinfo/i2c/src/i2cchre-plat.c
  --- a/mediatek/proprietary/tinysys/freertos/source/drivers/CM4_A/cpuinfo/i2c/src/i2cchre-plat.c
  +++ b/mediatek/proprietary/tinysys/freertos/source/drivers/CM4_A/cpuinfo/i2c/src/i2cchre-plat.c
   #include <plat/inc/rtc.h>
   #include <wakelock.h>
  
  -#define I2C_MAX_QUEUE_DEPTH     24
  -#define I2C_MAX_TXBUF_SIZE      8
  +#define I2C_MAX_QUEUE_DEPTH     32//24
  +#define I2C_MAX_TXBUF_SIZE      64
   #define I2C_RECORD_LEN          2
  
   static struct i2c_info_buf rec_info_buf;
  
   diff --git a/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/cpuinfo/platform/platform.mk b/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/cpuinfo/platform/platform.mk
  old mode 100644
  new mode 100755
  --- a/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/cpuinfo/platform/platform.mk
  +++ b/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/cpuinfo/platform/platform.mk
   CFG_TEST_PROGRAM_SUPPORT = no
   CFG_CACHE_2WAY_SUPPORT = no
   CFG_CM4_MODIFICATION = yes
  -CFG_I2C_CH0_DMA_SUPPORT = no
  -CFG_I2C_CH1_DMA_SUPPORT = no
  +CFG_I2C_CH0_DMA_SUPPORT = yes
  +CFG_I2C_CH1_DMA_SUPPORT = yes
  
   CFG_CNN_TO_SCP_BUF_SIZE = 0x0
   CFG_SCP_TO_CNN_BUF_SIZE = 0x0
  ```


# 3 功能确认

  * 安装sensor apk, 打开 g sensor查看上报的数据是否正常。分别将设备水平、垂直放置，看对应轴的数据是否趋近9.8
  * 打开自动旋转功能，确认横竖屏是否可以自动旋转


# 4 g sensor校准

## 4.1 g sensor 为什么需要校准

    每款产品对于g sensor的误差都会有自己的定义，g sensor 小幅度的误差并不会对横竖屏自动旋转会有影响，但是对于计步器类似的虚拟sensor就会有较大影响，所以需要对其进行校准。  

## 4.2 g sensor 校准逻辑
  
    * 如何进行g sensor校准：进入工厂模式后，单项测试中，选择 g sensor校准选项，平放手机，校准即可
    * 工模下校准流程，工模下的数据通过ioctl和driver层进行交互
      * 在第1步中进行校准后，通过ioctl读取20个数据，取平均做校准，这里会有个是否抖动的判断，即数值上下抖动超过阈值则判断为抖动
      * 计算得到校准数据后，通过ioctl 把校准的数据写入driver内
      * 同时把校准数据保存到nvram中
    * 校准一次，重新开机时如何把校准数据保存：
      * 在做校准时，会把数据保存到nvram中；开机时，会先从nvram中读取之前校准的数据，然后通过ioctl把校准数据设置到driver，所以校准一次就可以实现sensor的使用。
      * 开机过程中的nvram_daemon会去读取nvram中的值写入driver，从而生效

