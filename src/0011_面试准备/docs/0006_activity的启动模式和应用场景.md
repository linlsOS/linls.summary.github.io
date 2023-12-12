# activity的启动模式和应用场景

activity的启动模式和应用场景

# 参考文档

* [Android中Activity的启动模式（LaunchMode）和使用场景](https://blog.csdn.net/sinat_14849739/article/details/78072401)
* [Android进阶之Activity启动模式和应用场景详解](https://juejin.cn/post/7023294162363482143)

Activity的启动模式有四种：standard、singleTop、singleTask和singleInstance。

# standard：标准模式

standard 是标准启动模式，当我们没有指定 Activity 的启动模式时，默认就是这种模式。在 standard 模式下，每次启动一个 Activity 都会创建一个新的实例，它的 onCreate、onStart 以及 onResume均会被调用。这个新创建的 Activity将会放在启动它的 Activity 所在的任务栈的栈顶。

# singleTop

singleTop 是栈顶复用模式。在这种模式下，如果新启动的 Activity 已经在任务栈的栈顶了，那么就不会重新创建新的实例，而是调用这个 Activity 的 onPause、onNewIntent 以及 onResume 方法。如果新启动的 Activity 不是位于栈顶，那么还是会重新创建

# singleTask

singleTask 是栈内复用模式。当一个具有 singleTask 模式的 Activity 启动后，比如 Activity A，系统会首先寻找是否存在所需的任务栈，如果不存在，就重新创建一个任务栈，然后创建 A 的实例后把 A 放入到栈中。如果存在 A 所需要的任务栈，这时要看 A 是否在栈中有实例存在，如果有，那么系统就会把它调到栈顶并且调用它的 onNewIntent 方法，如果不存在，就创建 A 的实例并把 A 压入栈中

# singleInstance

singleInstance 是单实例模式。这种模式是 singleTask 的加强版，它除了具有 singleTask 的所有特性外，还加强了一点，那就是此种模式的 Activity 只能单独位于一个任务栈中。
例如：Activity A 是 singleInstance 模式，当 A 启动后，系统会创建一个新的任务栈，然后 A 独自在这个新的任务栈中，由于栈内复用的特性，后续的请求均不会创建新的 Activity，除非这个栈被销毁了；


# 启动模式设置详解

* 在AndroidMainifest的Activity配置进行设置

```
<activity
android:launchMode="启动模式"
//属性
//standard：标准模式
//singleTop：栈顶复用模式
//singleTask：栈内复用模式
//singleInstance：单例模式
//如不设置，Activity的启动模式默认为**标准模式（standard）**
```

* 通过Intent设置标志位

Intent inten = new Intent (ActivityA.this,ActivityB.class);
intent.addFlags(Intent,FLAG_ACTIVITY_NEW_TASK);
startActivity(intent);
FLAG_ACTIVITY_SINGLE_TOP:指定启动模式为栈顶复用模式（SingleTop）
FLAG_ACTIVITY_NEW_TASK:指定启动模式为栈内复用模式（SingleTask）
FLAG_ACTIVITY_CLEAR_TOP:所有位于其上层的Activity都要移除，SingleTask模式默认具有此标记效果
FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS:具有该标记的Activity不会出现在历史Activity的列表中，即无法通过历史列表回到该Activity上

Intent设置方式的优先级 > Manifest设置方式，即 以前者为准。Manifest设置方式无法设定 FLAG_ACTIVITY_CLEAR_TOP；Intent设置方式 无法设置单例模式（SingleInstance）；

# 启动模式的实际应用场景

* SingleTask模式的运用场景   
  最常见的应用场景就是保持我们应用开启后仅仅有一个Activity的实例                

* SingleTop模式的运用场景  
  假设你在当前的Activity中又要启动同类型的Activity，此时建议将此类型Activity的启动模式指定为SingleTop，能够降低Activity的创建，节省内存

* SingleInstance模式的运用场景
  
* standard 运用场景
  Activity 的启动默认就是这种模式。在 standard 模式下，每次启动一个 Activity 都会创建一个新的实例；在正常应用中正常打开和关闭页面就可以了，退出整个app就关闭所有的页面