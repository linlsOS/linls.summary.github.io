# camera 流程分析

camera 流程分析

# 参考文档
* [Android 10 Camera源码分析10：Camera Android架构(转载).md](https://github.com/zhoujinjianmm/zhoujinjian/blob/main/Android/Android%2010%20Camera%E6%BA%90%E7%A0%81%E5%88%86%E6%9E%9010%EF%BC%9ACamera%20Android%E6%9E%B6%E6%9E%84(%E8%BD%AC%E8%BD%BD).md)
* [Android : Camera2/HAL3 框架分析](https://www.cnblogs.com/blogs-of-lxl/p/10651611.html)
* [camera启动之---HwServiceManager进程启动过程](https://blog.csdn.net/weixin_43503508/article/details/107702905)
* [camerahal之---hal3主要接口实现(1)](https://blog.csdn.net/weixin_43503508/article/details/107827025)
* [camerahal之---rkisp-hal3内部实现(2)](https://blog.csdn.net/weixin_43503508/article/details/107851220)
* [Android Camera HAL3简介](https://deepinout.com/android-camera/android-camera-hal3-intro.html)
# camera 

```
* frameworks/av/services/camera/libcameraservice/common/hidl/HidlProviderInfo.cpp
  * status_t HidlProviderInfo::initializeHidlProvider
    * hardware::Return<void> ret = interface->getCameraIdList // 获取camera id list
      * status_t res = parseDeviceName(name, &major, &minor, &type, &id);
```