# Activity生命周期

Activity生命周期


# 参考文档

* [了解 Activity 生命周期](https://developer.android.google.cn/guide/components/activities/activity-lifecycle?hl=zh-cn)
* [Android：Activity的七个生命周期方法以及四个生命状态](https://blog.csdn.net/qq_28837549/article/details/104861949)

# Activity的七个生命周期方法

* onCreate: 开启activity的第一个方法，这个方法会初始化setContentLayout()方法（屏幕绘制）
* onStart: onCreate()方法完成后，此时activity进入了onStart（）方法，当前activity是用户可见状态，但是还不能交互，再此课做一些动画的初始化操作。
* onResume: onStart()后activity进入onResume方法，当前activity状态属于运行状态，（Running），此时的activity可见可交互。
* onPause: 在系统进行另一个activity时调用，通常用于确认对于持久性的数据保存更改，动画的停止以及任何其他可能消耗cpu的内容，他必须迅速的执行所需的操作，该方法执行后，下一个Activity才能开始执行，该方法执行后应该执行onStop()方法，
* onStop: 当Activity对与用户不在可见的时候调用，可能是被另一个Activity覆盖，或者退回到桌面，在onStop方法下系统内存紧张时，有可能会被系统回收
* onDestory: 在Activity被销毁前调用，这是Activity收到的最后调用，当Activity结束或者被系统销毁Activity实例的时候，会被调动该方法
* onRestart：在Activity被停止后再次启动的时候调用，比如从桌面回到应用中时，然后调用onStart方法().

# Activity的四个生命状态

Running >>> paused >>> stopped >>>killed

* 思考
* 思考
* 思考
