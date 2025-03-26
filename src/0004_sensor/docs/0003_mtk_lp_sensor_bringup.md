# mtk lp sensor bringup

mtk l/p（light & proximity） sensor bringup

# menu

* [1 简介](#1-简介)
* [2 lp sensor 驱动加载](#2-lp-sensor-驱动加载)
  * [2.1 代码修改主要包含几个部分](#21-代码修改主要包含几个部分)
  * [2.2 代码修改](#22-代码修改)
  * [2.3 bring up l/p sensor 问题排查](#23-bring-up-lp-sensor-问题排查)
* [3 proximity sensor 的校准](#3-proximity-sensor-的校准)
* [4 light sensor 的校准](#4-light-sensor-的校准)
* [5 light sensor上报由亮度、可见光、红外光](#5-light-sensor上报由亮度可见光红外光)

# 1 简介
  
  这里主要介绍MTK 平台下 lp sensor 的bring up。这里主要包含几个任务：  
  （1）light & proximity sensor 的驱动加载；  
  （2）proximity sensor 的校准；
  （3）light sensor 的校准  
  （4）light sensor的上报由单一亮度，修改为亮度、可见光、红外光三个数据的上报；  



# 2 lp sensor 驱动加载

  这里介绍的sensor是挂载在AP侧

## 2.1 代码修改主要包含几个部分
      
    * dts 配置中断、引脚、i2c地址等信息
    * 配置供电
    * 添加新增器件的编译控制宏
    * 打开项目中的L/P sensor 配置
  
## 2.2 代码修改
      
    此部分代码修改需要根据原理图、具体项目、cpu进行配置
    * dts 配置中断、引脚、i2c地址等信息
  
      ```diff
      diff --git a/arch/arm64/boot/dts/mediatek/project.dts b/arch/arm64/boot/dts/mediatek/project.dts
      --- a/arch/arm64/boot/dts/mediatek/project.dts
      +++ b/arch/arm64/boot/dts/mediatek/project.dts
                      reg = <0x36>;
                      status = "okay";
              };
      +      ltr578@53 {
      +               compatible = "mediatek,alsps";
      +               reg = <0x53>;
      +               interrupt-parent = <&pio>;
      +               interrupts = <6 IRQ_TYPE_LEVEL_LOW 6 0>;
      +               debounce = <10 0>;
      +               pinctrl-names = "pin_default", "pin_cfg";
      +               pinctrl-0 = <&alsps_intpin_default>;
      +               pinctrl-1 = <&alsps_intpin_cfg>;
      +               status = "okay";
      +               i2c_num = <6>;
      +               i2c_addr = <0x53 0x0 0x0 0x00>;
      +               polling_mode_ps = <0>;
      +               polling_mode_als = <1>;
      +               power_id = <0xffff>;
      +               power_vol = <0>;
      +               //irq-gpio = <&pio 6 0x00>;               
      +      };
      +&pio {
      +       alsps_intpin_cfg: alspspincfg {
      +
      +               pins_cmd_dat {
      +                       pinmux = <PINMUX_GPIO6__FUNC_GPIO6>;
      +                       slew-rate = <0>;
      +                       bias-pull-up = <MTK_PUPD_SET_R1R0_01>;
      +               };
      +       };
      +
      +       alsps_intpin_default: alspsdefaultcfg {
      +
      +       };
      +
      +};
      ```
    
    * 配置供电
  
      ```diff
      diff --git a/drivers/misc/mediatek/dws/cpuinfo/project.dws b/drivers/misc/mediatek/dws/cpuinfo/project.dws
      --- a/drivers/misc/mediatek/dws/cpuinfo/project.dws
      +++ b/drivers/misc/mediatek/dws/cpuinfo/project.dws
                  <gpio182>
                       <inpull_en>true</inpull_en>
                       <inpull_selhigh>false</inpull_selhigh>
                       <def_dir>OUT</def_dir>
      -                <out_high>false</out_high>
      +                <out_high>true</out_high>
                       <varName0>GPIO_SUB_VCAMA_LDO_EN</varName0>
                       <smt>false</smt>
                       <ies>true</ies> 
                  <gpio182> 
      ```
  
    * 添加新增器件的编译控制宏
  
      ```
      diff --git a/arch/arm64/configs/project_defconfig b/arch/arm64/configs/project_defconfig
      --- a/arch/arm64/configs/project_defconfig
      +++ b/arch/arm64/configs/project_defconfig
       CONFIG_TOUCHSCREEN_MTK=y
       CONFIG_TOUCHSCREEN_FTS=y
      +CONFIG_MTK_LTR578=y
       CONFIG_MICROTRUST_TEE_SUPPORT=y
       CONFIG_TEE=y
      
      diff --git a/drivers/misc/mediatek/sensors-1.0/alsps/Kconfig b/drivers/misc/mediatek/sensors-1.0/alsps/Konfig
      --- a/drivers/misc/mediatek/sensors-1.0/alsps/Kconfig
      +++ b/drivers/misc/mediatek/sensors-1.0/alsps/Kconfig
       source "drivers/misc/mediatek/sensors-1.0/alsps/cm36558/Kconfig"
       source "drivers/misc/mediatek/sensors-1.0/alsps/ltr303/Kconfig"
       source "drivers/misc/mediatek/sensors-1.0/alsps/ltr559/Kconfig"
      +source "drivers/misc/mediatek/sensors-1.0/alsps/ltr578/Kconfig"
       source "drivers/misc/mediatek/sensors-1.0/alsps/stk3x1x/Kconfig"
      diff --git a/drivers/misc/mediatek/sensors-1.0/alsps/Makefile b/drivers/misc/mediatek/sensors-1.0/alsps/Makfile
      --- a/drivers/misc/mediatek/sensors-1.0/alsps/Makefile
      +++ b/drivers/misc/mediatek/sensors-1.0/alsps/Makefile
       obj-$(CONFIG_MTK_CM36558) += cm36558/
       obj-$(CONFIG_MTK_LTR303) += ltr303/
       obj-$(CONFIG_MTK_LTR559)       +=  ltr559/
      +obj-$(CONFIG_MTK_LTR578)       +=  ltr578/
      ```
  
    * 打开项目中的L/P sensor 配置
  
      ```
      diff --git a/boe/project/ProjectConfig.mk b/boe/project/ProjectConfig.mk
      --- a/boe/project/ProjectConfig.mk
      +++ b/boe/project/ProjectConfig.mk
       CUSTOM_HAL_SUB_IMGSENSOR = gc2375h_mipi_raw
       CUSTOM_HIFI_SENSORS = no
       CUSTOM_KERNEL_ACCELEROMETER = yes
      -CUSTOM_KERNEL_ALSPS = yes
      +CUSTOM_KERNEL_ALSPS = no
      +CUSTOM_KERNEL_ALS = yes
      +CUSTOM_KERNEL_PS = yes
       CUSTOM_KERNEL_BAROMETER = no
       CUSTOM_KERNEL_CAMERA = camera
       CUSTOM_KERNEL_DEVICE_ORIENTATION = yes
      ```
  
## 2.3 bring up l/p sensor 问题排查
  
    * 通过dumpsys sensorservice查找不到新增的sensor  
      通过日志确认是否probe走完了，如果确认走了但是sensor list中并未有对应的sensor,则需确认是否有注册了 
      在probe中添加如下代码进行sensor注册
      ```cpp
      strncpy(alsps_devinfo.name, LTR578_DEV_NAME, sizeof(LTR578_DEV_NAME));
  	sensorlist_register_deviceinfo(ID_LIGHT, &alsps_devinfo);
  
  	strncpy(alsps_devinfo.name, LTR578_DEV_NAME, sizeof(LTR578_DEV_NAME));
  	sensorlist_register_deviceinfo(ID_PROXIMITY, &alsps_devinfo);
      ```
      
    * 通过应用可以获取sensor列表,但是在列表中打开sensor 没有数据      
      确认gpio 的out标志位是否是被拉高的状态，即便配置了GPIO也有可能在其它dts中被拉低了，可以通过命令查看对应的gpio是否正确
      ```sh
      sh:  # cat /sys/devices/platform/pinctrlxxxx/mt_gpio                                                          
      pins base: 73
      PIN: (MODE)(DIR)(DOUT)(DIN)(DRIVE)(SMT)(IES)(PULL_EN)(PULL_SEL)(R1 R0)
      
      # cat /sys/devices/platform/pinctrlxxxx/mt_gpio |grep 182                                                  
      182: 0111000110
  
      强拉gpio
      echo set 182  0111000110 > /sys/devices/platform/pinctrlxxxx/mt_gpio
      ```

# 3 proximity sensor 的校准

  * 为什么需要进行p sensor 校准？  
    p sensor 就是我们常说的距感，常见的使用场景是电话接听时贴近耳朵屏幕灭屏，原理屏幕屏幕亮屏，此部分就是通过p sensor 上报是否有遮挡。  
    遮挡物由靠近逐步远离p sensor，当 p sensor 读取的数据小于门限值上报远离；遮挡物由远及近慢慢贴近p sensor, 当p sensor读取数据大于门限值上报靠近。  
    这里的校准就是为了获取靠近、远离的高低门限，因为每个器件都会有差异，另外装机后屏幕的透光率也会对此有影响，所以需要对p sensor 进行校准。  
    底噪: 是指p sensor 没有遮挡时读取的数据  
    校准一般会取2～3个点，然后进行高低门限计算，将高低门限设置到器件中进行保存。校准结束后取两个点进行测试确认是否可以正常识别靠近、远离。
    校准的计算过程中一定要多次读取数据取均值。

# 4 light sensor 的校准
    
  * 为什么需要进行light sensor校准  

    在light sensor上一般都会有玻璃的透光孔，玻璃的透光率会有差异，另外工人在整机贴合的过程中缝隙也会有差异，器件本身也存在差异，这样就会导致不同的成品在  
    相同的光照环境下上报的light sensor数据会有较大差异。这样就会导致相同的动态背光曲线在相同环境下表现会有很大差异，所以需要对整机进行 light sensor校准。  


  * 如何进行 light sensor 校准  

    light sensor 的校准分为亮光校准和暗光校准。  

    light sensor 亮光校准是指在指定的光箱给定指定光照强度的光源用照度计读取当前光照的强度，将light sensor 的设备放置在同照度计相同为值读取光照强度的数据乘以系数后同照度计的数值相同。  

    light sensor 暗光校准是指在指定的光箱内关闭灯光用照度计确认当前光照强度是否为0，将light sesnor的设备防治在同照度计相同位置，读取此时light sensor的数值记录，后续light sensor     

    数据上报时减去从值。   

    校准过程需要多次读取亮度取均值，如果有偏离上限或者下限的数值应该舍弃，否则后续的数值则会有较大偏差。  


# 5 light sensor上报由亮度、可见光、红外光
  
  * 动态背光需要的值是亮度，为什么需要上报光照、可见光、红外光？  

    此部分是硬件要求，硬件需要调整偏光的数据，在界面上显示方便其对应的调试   

  * 代码修改

    ```   
    drivers/misc/mediatek/sensors-1.0/alsps/ltr578/ltr578.c | 77 ++++++++++++++++++++++++++++++++++-----------------
     
     diff --git a/drivers/misc/mediatek/sensors-1.0/alsps/ltr578/ltr578.c b/drivers/misc/mediatek/sensors-1.0/alsps/ltr578/ltr578.c
     --- a/drivers/misc/mediatek/sensors-1.0/alsps/ltr578/ltr578.c
     +++ b/drivers/misc/mediatek/sensors-1.0/alsps/ltr578/ltr578.c
     
     -static int ltr578_als_read(struct i2c_client *client, u16* data)
     +static int ltr578_als_read(struct i2c_client *client, u16* data, int* value)
      {
             int alsval = 0, clearval = 0;
             int32_t luxdata_int;
     @@ -693,7 +693,11 @@ static int ltr578_als_read(struct i2c_client *client, u16* data)
             }
     
             alsval = (buf[2] * 256 * 256) + (buf[1] * 256) + buf[0];
     +       if(value != NULL) {
     +               value[1] = alsval;
     +       }
     
             ret = ltr578_master_recv(client, LTR578_CLEAR_DATA_0, buf, 0x03);
             if (ret < 0) {
     @@ -701,8 +705,13 @@ static int ltr578_als_read(struct i2c_client *client, u16* data)
             }
     
             clearval = (buf[2] * 256 * 256) + (buf[1] * 256) + buf[0];
     +       if(value != NULL) {
     +               value[2] = clearval;
     +       }
     
     drivers/misc/mediatek/sensors-1.0/alsps/alsps.c | 25 +++++++++++++++----------
     diff --git a/drivers/misc/mediatek/sensors-1.0/alsps/alsps.c b/drivers/misc/mediatek/sensors-1.0/alsps/alsps.c
     --- a/drivers/misc/mediatek/sensors-1.0/alsps/alsps.c
     +++ b/drivers/misc/mediatek/sensors-1.0/alsps/alsps.c
     
      static struct alsps_init_info *alsps_init_list[MAX_CHOOSE_ALSPS_NUM] = {0};
     
     -int als_data_report_t(int value, int status, int64_t time_stamp)
     +int als_data_report_t(int* value, int status, int64_t time_stamp)
      {
             int err = 0;
             struct alsps_context *cxt = NULL;
             if (cxt->is_get_valid_als_data_after_enable == false) {
                     event.handle = ID_LIGHT;
                     event.flush_action = DATA_ACTION;
     -               event.word[0] = value + 1;
     +               event.word[0] = value[0] + 1;
                     err = sensor_input_event(cxt->als_mdev.minor, &event);
                     cxt->is_get_valid_als_data_after_enable = true;
             }
     -       if (value != last_als_report_data) {
     +       if (value[0] != last_als_report_data) {
                     event.handle = ID_LIGHT;
                     event.flush_action = DATA_ACTION;
     -               event.word[0] = value;
     +               event.word[0] = value[0];
     +               event.word[1] = value[1];
     +               event.word[2] = value[2];
                     event.status = status;
                     err = sensor_input_event(cxt->als_mdev.minor, &event);
                     if (err >= 0)
     -                       last_als_report_data = value;
     +                       last_als_report_data = value[0];
             }
             return err;
      }
     -int als_data_report(int value, int status)
     +int als_data_report(int* value, int status)
      {
             return als_data_report_t(value, status, 0);
      }
      static void als_work_func(struct work_struct *work)
      {
             struct alsps_context *cxt = NULL;
     -       int value, status;
     +       int value[3], status;
             int64_t nt;
             struct timespec time;
             int err;
             time = get_monotonic_coarse();
             nt = time.tv_sec * 1000000000LL + time.tv_nsec;
             /* add wake lock to make sure data can be read before system suspend */
     -       err = cxt->als_data.get_data(&value, &status);
     +       err = cxt->als_data.get_data(value, &status);
             if (err) {
                     pr_err("get alsps data fails!!\n");
                     goto als_loop;
             } else {
     -               cxt->drv_data.als_data.values[0] = value;
     +               cxt->drv_data.als_data.values[0] = value[0];
     +               cxt->drv_data.als_data.values[1] = value[1];
     +               cxt->drv_data.als_data.values[2] = value[2];
                     cxt->drv_data.als_data.status = status;
                     cxt->drv_data.als_data.time = nt;
             }
     @@ -211,7 +216,7 @@ static void als_work_func(struct work_struct *work)
                     }
             }
     -       als_data_report(cxt->drv_data.als_data.values[0],
     +       als_data_report(cxt->drv_data.als_data.values,
                             cxt->drv_data.als_data.status);
     
    diff --git a/mediatek/proprietary/hardware/sensor/sensors-1.0/AmbienteLight.cpp b/mediatek/proprietary/hardware/sensor/sensors-1.0/AmbienteLight.cpp
    --- a/mediatek/proprietary/hardware/sensor/sensors-1.0/AmbienteLight.cpp
    +++ b/mediatek/proprietary/hardware/sensor/sensors-1.0/AmbienteLight.cpp
                     mPendingEvent.timestamp = event->time_stamp;
                 else
                     mPendingEvent.timestamp = android::elapsedRealtimeNano();
    -                mPendingEvent.light = event->word[0];
    +                //mPendingEvent.light= event->word[0];
    +                mPendingEvent.data[0] = event->word[0];
    +                mPendingEvent.data[1] = event->word[1];
    +                mPendingEvent.data[2] = event->word[2];
             } else if (event->flush_action == CALI_ACTION){
                 mSensorSaved.getCalibrationInt32(ALS_CALI_SAVED_DIR, ALS_TAG_CALI, alstempCali, 2);
                 if(event->word[0] == 0){

     +++ b/interfaces/sensors/1.0/default/convert.cpp
                  break;
              }
     
     +        case SensorType::LIGHT:{
     +            dst->u.data[0] = src.data[0];
     +            dst->u.data[1] = src.data[1];
     +            dst->u.data[2] = src.data[2];
     +            break;
     +        }
     +
              case SensorType::DEVICE_ORIENTATION:
     -        case SensorType::LIGHT:
     +
     ```

     ```diff
     diff --git a/base/core/java/android/hardware/Sensor.java b/base/core/java/android/hardware/Sensor.java
     --- a/base/core/java/android/hardware/Sensor.java
     +++ b/base/core/java/android/hardware/Sensor.java
                  3, // SENSOR_TYPE_GEOMAGNETIC_FIELD
                  3, // SENSOR_TYPE_ORIENTATION
                  3, // SENSOR_TYPE_GYROSCOPE
     -            1, // SENSOR_TYPE_LIGHT
     +            3, // SENSOR_TYPE_LIGHT
                  1, // SENSOR_TYPE_PRESSURE
                  1, // SENSOR_TYPE_TEMPERATURE
                  1, // SENSOR_TYPE_PROXIMITY
     diff --git a/native/services/sensorservice/SensorDeviceUtils.cpp b/native/services/sensorservice/SensorDeviceUtils.cpp
     --- a/native/services/sensorservice/SensorDeviceUtils.cpp
     +++ b/native/services/sensorservice/SensorDeviceUtils.cpp
              case SensorTypeV2_1::MAGNETIC_FIELD_UNCALIBRATED:
              case SensorTypeV2_1::GYROSCOPE_UNCALIBRATED:
              case SensorTypeV2_1::ACCELEROMETER_UNCALIBRATED:
     +        case SensorTypeV2_1::LIGHT:
                  axes = 3;
                  break;
              case SensorTypeV2_1::DEVICE_ORIENTATION:
     -        case SensorTypeV2_1::LIGHT:
              case SensorTypeV2_1::PRESSURE:
              case SensorTypeV2_1::TEMPERATURE:
              case SensorTypeV2_1::PROXIMITY:

    ```




     


