# ActivityA2ActivityB

Activity A 启动另一个Activity B 会调用哪些方法？如果B是透明主题的又或则是个DialogActivity呢

# 参考文档

* [Activity A启动另一个Activity B会回调哪些方法？如果Activity B是完全透明呢？如果启动的是一个Dialog呢？](https://www.cnblogs.com/aademeng/articles/10890396.html)
* [Android面试题-Activity A跳转Activity B，再按返回键，生命周期执行的顺序](https://blog.csdn.net/u013700040/article/details/105458684)

# ActivityA_2_ActivityB

一、Activity  A启动另一个Activity  B回调那些方法

    1、A界面==onCreate() ----->A界面==onStart()------> A界面==onResume() ---->A界面==onPause()

        ----> B界面==onCreate() ---->B界面==onStart()------->B界面==onResume()-----A界面==onStop()

       Activity B显示后 点击返回按钮 回调的方法

        B界面==onPause()------>A界面===onRestart()---->A界面==onStart()---->A界面==onResume()

        ----->B界面==onStop()------>B界面==onDestroy()

二、Activity  A启动另一个Activity  B  如果Activity B完全透明  会回调那些方法

1、A界面==onCreate()---> A界面==onStart()------> A界面==onResume()---->A界面==onPause()

       ----->B界面==onCreate()----> B界面==onStart()---->B界面==onResume()

 Activity B显示后 点击返回按钮 回调的方法

    B界面==onPause()----->A界面==onResume()---->B界面==onStop()----->B界面==onDestroy()

三、Activity  A启动一个完全透明的Dialog 回调哪些方法

1、A界面==onCreate()---> A界面==onStart()------> A界面==onResume()

