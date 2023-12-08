# Activity的启动流程

Activity的启动流程


# 参考文档

* [Android11（API30）中Activity的启动流程—从startActivity到onCreate](https://blog.csdn.net/yu749942362/article/details/107978083)
* [Activity的启动流程这一篇够了](https://juejin.cn/post/6924198995313033224)
* [面试：Activity的启动流程简述](https://blog.csdn.net/cpcpcp123/article/details/122055099)


# 启动步骤

1. Launcher进程请求AMS

2. AMS发送创建应用进程请求

3. Zygote进程接受请求并孵化应用进程

4. 应用进程启动ActivityThread

5. 应用进程绑定到AMS

6. AMS发送启动Activity的请求

7. ActivityThread的Handler处理启动Activity的请求


# 代码堆栈

```
* frameworks/base/core/java/android/app/Activity.java
* frameworks/base/services/core/java/com/android/server/am/ActivityManagerService.java
  * public int startActivity(IApplicationThread caller, String callingPackage,
    * return mActivityTaskManager.startActivity(caller, callingPackage, null, intent,
      * frameworks/base/services/core/java/com/android/server/wm/ActivityTaskManagerService.java 
        * public final int startActivity(IApplicationThread caller, String callingPackage,
          * return startActivityAsUser(caller, callingPackage, callingFeatureId, intent, resolvedType,
            * private int startActivityAsUser(IApplicationThread caller, String callingPackage,
              * return getActivityStartController().obtainStarter(intent, "startActivityAsUser")
                * frameworks/base/services/core/java/com/android/server/wm/ActivityTaskManagerService.java 
                  * ActivityStartController getActivityStartController() 
```

* 思考
* 思考
* 思考
