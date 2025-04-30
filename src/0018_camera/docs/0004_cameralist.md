# cameralist

获取camera 列表

# Author Info

 Field  | Info
--------|----------
 Author | linliangsong 
 Date   | 2025-4-1
 Email  | linliangsong@boe.com.cn 

# 参考文档

# 目的

  通过获取camera 列表，查看整个camera注册流程

# 代码实现

```cpp
    try {
    String[] cameraIdList = manager.getCameraIdList();
    for (String cameraId : cameraIdList) {
    System.out.println("Camera ID: " + cameraId);
    }
    } catch (CameraAccessException e) {
    e.printStackTrace();
    }
```

# 代码堆栈

```
* public String[] getCameraIdList()
  * return CameraManagerGlobal.get().getCameraIdList();
    * frameworks/base/core/java/android/hardware/camera2/CameraManager.java 
      * connectCameraServiceLocked();
      * cameraIds = extractCameraIdListLocked();
      * sortCameraIds(cameraIds); 
```


