# dma cfg

mtk i2c dma 配置

# i2c dma

* 当需要连续发送大量数据，考虑采用DMA进行数据传输，fifo最大才8 byte
* 当在器件调试时，如果初始化过程中需要写入大量块数据，请确认是否开了DMA

# diff

```diff
diff --git a/mediatek/proprietary/tinysys/freertos/source/drivers/CM4_A/mt6765/i2c/src/i2cchre-plat.c b/mediatek/proprietary/tinysys/freertos/source/drivers/CM4_A/mt
--- a/mediatek/proprietary/tinysys/freertos/source/drivers/CM4_A/mt6765/i2c/src/i2cchre-plat.c
+++ b/mediatek/proprietary/tinysys/freertos/source/drivers/CM4_A/mt6765/i2c/src/i2cchre-plat.c
@@ -54,8 +54,8 @@
 #include <plat/inc/rtc.h>
 #include <wakelock.h>

-#define I2C_MAX_QUEUE_DEPTH     24
-#define I2C_MAX_TXBUF_SIZE      8
+#define I2C_MAX_QUEUE_DEPTH     32
+#define I2C_MAX_TXBUF_SIZE      128
 #define I2C_RECORD_LEN          2

 static struct i2c_info_buf rec_info_buf;

 diff --git a/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/mt6765/platform/platform.mk b/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/mt67
--- a/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/mt6765/platform/platform.mk
+++ b/mediatek/proprietary/tinysys/freertos/source/project/CM4_A/mt6765/platform/platform.mk
@@ -83,8 +83,8 @@ CFG_FEATURE03_SUPPORT = no
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
