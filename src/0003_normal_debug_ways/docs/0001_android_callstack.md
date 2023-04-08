# Android CallStack

Android backtrace

# c++

* 头文件
```
#include <utils/CallStack.h>
```
* 代码引用
```c++
android::CallStack callstack;
callstack.update();
callstack.log("markdebug");
```
* so 依赖
  * 依赖libutilscallstack

# java

* 参考文档
  * [Android 打印堆栈的几种方法](https://blog.csdn.net/deng0zhaotai/article/details/44957673)
* 代码引用
```java
import java.lang.Exception

Exception e = new Exception("add exception");
e.printStackTrace();
```