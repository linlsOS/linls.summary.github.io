# android 系统按键

系统按键

# Author Info

 Field  | Info
--------|----------
 Author | linliangsong 
 Date   | 2025-3-25
 Email  | linliangsong@boe.com.cn 

# 参考文档
* [Android 修改按键音](https://blog.csdn.net/a546036242/article/details/129549096)
* [Android 获取/设置按键音（Touch Sounds）](https://blog.csdn.net/cshoney/article/details/13290181)

# android 按键音调试


* 打开、关闭按键音
```
Settings.System.getInt(getContentResolver(), Settings.System.SOUND_EFFECTS_ENABLED, 0);
```

* 按键音流程

```
* frameworks/base/media/java/android/media/AudioManager.java
  * public void  playSoundEffect(int effectType)
    * if (!querySoundEffectsEnabled(Process.myUserHandle().getIdentifier())) //这里判断按键音开关是否设置了，如果没有则直接返回
      * private boolean querySoundEffectsEnabled(int user) 
        * return Settings.System.getIntForUser(getContext().getContentResolver(),Settings.System.SOUND_EFFECTS_ENABLED, 0, user) != 0;
        * return;
    * service.playSoundEffect(effectType);
```

* 按键音问题，设置关于界面上下移动无按键音。
这里的问题是在ViewRootImpl.java中会去判断当前的按键是否被处理了，如果被处理则则返回，不发出按键音。
而ViewRootImpl.java中代码逻辑，如果当前窗口是可滑动，按键为上下按键，则返回按键被处理
* [0016_0423日报.md](0016_0423日报.md)
```
* frameworks/base/core/java/android/widget/ScrollView.java
  * public boolean dispatchKeyEvent(KeyEvent event) 
    * return super.dispatchKeyEvent(event) || executeKeyEvent(event);
      * public boolean executeKeyEvent(KeyEvent event) 
        * if (!canScroll())
        * if (event.getAction() == KeyEvent.ACTION_DOWN)
          * handled = arrowScroll(View.FOCUS_UP);
          * handled = fullScroll(View.FOCUS_DOWN);//所以这里返回按键被处理了，所以就在当前页面无声
```

