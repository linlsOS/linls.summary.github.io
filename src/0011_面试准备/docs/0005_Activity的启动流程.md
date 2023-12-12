# Activity的启动流程

Activity的启动流程


# 参考文档

* [Android11（API30）中Activity的启动流程—从startActivity到onCreate](https://blog.csdn.net/yu749942362/article/details/107978083)
* [Activity的启动流程这一篇够了](https://juejin.cn/post/6924198995313033224)
* [面试：Activity的启动流程简述](https://blog.csdn.net/cpcpcp123/article/details/122055099)
* [面试必备：Android（9.0）Activity启动流程(一)](https://juejin.cn/post/6844903959581163528)
* [面试必备：Android（9.0）Activity启动流程(二)](https://juejin.cn/post/6844903959589552142)
* [Android13 Activity启动流程分析](https://juejin.cn/post/7212444005064785977)


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
  ├── startActivity(intent)
  ├── startActivity(intent, null)
  ├── startActivityForResult(intent, -1)
  └── startActivityForResult(intent, -1, null)
      └── public void startActivityForResult(String who, Intent intent, int requestCode, @Nullable Bundle options)
          ├── Instrumentation.ActivityResult ar = mInstrumentation.execStartActivity(this, mMainThread.getApplicationThread(), mToken, who, intent, requestCode, options);
          │   └── frameworks/base/core/java/android/app/Instrumentation.java
          │       └── public ActivityResult execStartActivity(Context who, IBinder contextThread, IBinder token, Activity target, Intent intent, int requestCode, Bundle options) 
          │           ├── final ActivityMonitor am = mActivityMonitors.get(i);  // 更新 ActivityMonitor
          │           ├── result = am.onStartActivity(intent);
          │           ├── int result = ActivityTaskManager.getService().startActivity(whoThread, who.getBasePackageName(), who.getAttributionTag(), intent, intent.resolveTypeIfNeeded(who.getContentResolver()), token, target != null ? target.mEmbeddedID : null, requestCode, 0, null, options); //调用 System Activity Manager
          │           │   ├── frameworks/base/core/java/android/app/ActivityTaskManager.java 
          │           │   │   └── public static IActivityTaskManager getService() 
          │           │   │       └── return IActivityTaskManagerSingleton.get();
          │           │   │           ├── final IBinder b = ServiceManager.getService(Context.ACTIVITY_TASK_SERVICE);  // 获取原始的IBinder对象
          │           │   │           └── return IActivityTaskManager.Stub.asInterface(b);  //转换为 Service 的 RPC 接口，返回一个类型为IActivityTaskManager的对象
          │           │   └── frameworks/base/services/core/java/com/android/server/wm/ActivityTaskManagerService.java
          │           │       └── public final int startActivity(IApplicationThread caller, String callingPackage,
          │           │           └── return startActivityAsUser(caller, callingPackage, callingFeatureId, intent, resolvedType,
          │           │               └── private int startActivityAsUser(IApplicationThread caller, String callingPackage,
          │           │                   └── return getActivityStartController().obtainStarter(intent, "startActivityAsUser").setCaller(caller).setCallingPackage(callingPackage).setCallingFeatureId(callingFeatureId).setResolvedType(resolvedType).setResultTo(resultTo).setResultWho(resultWho).setRequestCode(requestCode).setStartFlags(startFlags).setProfilerInfo(profilerInfo).setActivityOptions(bOptions).setUserId(userId).execute(); //获取 ActivityStarter 并设置对应参数
          │           │                       ├── frameworks/base/services/core/java/com/android/server/wm/ActivityStartController.java 
          │           │                       │   └── ActivityStarter obtainStarter(Intent intent, String reason) // 获取ActivityStarter
          │           │                       └── frameworks/base/services/core/java/com/android/server/wm/ActivityStarter.java 
          │           │                           └── int execute() 
          │           │                               └── res = executeRequest(mRequest);
          │           │                                   └── private int executeRequest(Request request) 
          │           │                                       └── mLastStartActivityResult = startActivityUnchecked(r, sourceRecord, voiceSession, request.voiceInteractor, startFlags, true /* doResume */, checkedOptions, inTask,restrictedBgActivity, intentGrants);//调用该方法时表示大部分初步的权限检查已经完成，执行 Trace，以及异常处理；
          │           │                                           └── private int startActivityUnchecked(final ActivityRecord r, ActivityRecord sourceRecord,
          │           │                                               └── result = startActivityInner(r, sourceRecord, voiceSession, voiceInteractor,startFlags, doResume, options, inTask, restrictedBgActivity, intentGrants);//启动 Activity，并更新全局的 task 栈帧信息
          │           │                                                   └── int startActivityInner(final ActivityRecord r, ActivityRecord sourceRecord,
          │           │                                                       ├── computeSourceStack();
          │           │                                                       ├── mTargetStack.startActivityLocked(mStartActivity,
          │           │                                                       ├── mRootWindowContainer.resumeFocusedStacksTopActivities(mTargetStack, mStartActivity, mOptions);
          │           │                                                       │   └── frameworks/base/services/core/java/com/android/server/wm/RootWindowContainer.java 
          │           │                                                       │       └── boolean resumeFocusedStacksTopActivities(ActivityStack targetStack, ActivityRecord target, ActivityOptions targetOptions)
          │           │                                                       │           └── result |= focusedStack.resumeTopActivityUncheckedLocked(target, targetOptions);
          │           │                                                       │               └── frameworks/base/services/core/java/com/android/server/wm/ActivityStack.java 
          │           │                                                       │                   └── boolean resumeTopActivityUncheckedLocked(ActivityRecord prev, ActivityOptions options)
          │           │                                                       │                       ├── result = resumeTopActivityInnerLocked(prev, options);
          │           │                                                       │                       │   └── private boolean resumeTopActivityInnerLocked(ActivityRecord prev, ActivityOptions options)
          │           │                                                       │                       │       ├── ActivityRecord next = topRunningActivity(true /* focusableOnly */);
          │           │                                                       │                       │       └── mStackSupervisor.startSpecificActivity(next, true, true);
          │           │                                                       │                       │           └── frameworks/base/services/core/java/com/android/server/wm/ActivityStackSupervisor.java 
          │           │                                                       │                       │               └── void startSpecificActivity(ActivityRecord r, boolean andResume, boolean checkConfig) // 主要是判断目标 Activity 所在的应用是否正在运行。如果已经在运行就直接启动，否则就启动新进程
          │           │                                                       │                       │                   └── mService.startProcessAsync(r, knownToBeDead, isTop, isTop ? "top-activity" : "activity");
          │           │                                                       │                       └── final ActivityRecord next = topRunningActivity(true /* focusableOnly */);
          │           │                                                       └── mRootWindowContainer.updateUserStack(mStartActivity.mUserId, mTargetStack);
          │           └── checkStartActivityResult(result, intent);
          └── mMainThread.sendActivityResult(mToken, who, requestCode,ar.getResultCode(), ar.getResultData());
```

```
* frameworks/base/core/java/android/os/ZygoteProcess.java 
  * public final Process.ProcessStartResult start(@NonNull final String processClass,
    * return startViaZygote(processClass, niceName, uid, gid, gids,
      * private Process.ProcessStartResult startViaZygote(@NonNull final String processClass,
        * return zygoteSendArgsAndGetResult(openZygoteSocketIfNeeded(abi),
          * private ZygoteState openZygoteSocketIfNeeded(String abi) throws ZygoteStartFailedEx 
            * attemptConnectionToPrimaryZygote();
```

总结：

1. Launcher进程请求AMS
2. AMS发送创建应用进程请求
3. Zygote进程接受请求并孵化应用进程
4. 应用进程启动ActivityThread
5. 应用进程绑定到AMS
6. AMS发送启动Activity的请求
7. ActivityThread的Handler处理启动Activity的请求
  

常见应用启动方法：
startActivity - 开发者所能接触到的接口，启动一个应用(Activity)，可以选择一个回调接收启动的结果；
startActivityAsUser - 以指定用户的身份启动应用，这是个隐藏接口，只对预装的应用开放；
startActivityAsCaller - 以调用者(即启动本当前 Activity 的 APP)的身份启动应用，也是个隐藏接口，主要用于 resolver 和 chooser Activity；
这些接口的实现都和上面代码类似，即最终都会调用到 Instrumentation 的接口

* 思考
* 思考
* 思考
