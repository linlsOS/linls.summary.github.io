# onSaveInstanceState

说下onSaveInstanceState()方法的作用 ? 何时会被调用？

# 参考文档

* [Android应用开发—onSaveInstanceState方法什么时候被调用?](https://blog.csdn.net/voidreturn/article/details/79207344)
* [onSavedInstanceState()和onRestoreInstanceState()理解](https://blog.csdn.net/shouniezhe/article/details/47705001?spm=1001.2101.3001.6661.1&utm_medium=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7ERate-1-47705001-blog-121893938.235%5Ev39%5Epc_relevant_3m_sort_dl_base4&depth_1-utm_source=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7ERate-1-47705001-blog-121893938.235%5Ev39%5Epc_relevant_3m_sort_dl_base4&utm_relevant_index=1)

Activity 中的 onSaveInstanceState() 方法和 onRestoreInstanceState() 方法并不是生命周期方法，它们不同于 onCreate()、onPause() 等生命周期方法，它们并不一定会被触发。  
当应用遇到意外情况（如：内存不足、用户直接按Home键），由系统销毁一个 Activity 时，onSaveInstanceState() 方法就会被调用。但是当用户主动去销毁一个 Activity 时，例如在应用  
中按返回键，onSaveInstanceState() 方法就不会被调用。因为在这种情况下，用户的行为决定了不需要保存Activity的状态。通常onSaveInstanceState() 方法只适合用于保存一些临时性  
的状态，而onPause() 方法适合用于数据的持久化保存。 另外，当屏幕的方向发生了改变， Activity 会被销毁并重新创建，如果你想在 Activity 被销毁前缓存一些数据，并且在 Activity   
被重新创建后恢复缓存的数据。可以重写 Activity 中的 onSaveInstanceState() 方法和 onRestoreInstanceState()方法。

onSaveInstanceState()是在系统感觉需要销毁Activity时调用的，它被传入一个参数Bundle，这个Bundle可以被认为是个 Map 字典之类的东西，用“键－值”的形式来保存数据

onSaveInstanceState() 方法的主要目的是保存和 Activity 的状态有关的数据，当系统在销毁 Activity 时，如果它希望 Activity 下次出现的样子跟之前完全一样，那么它就会调用onSaveInstanceState()，  
否则就不调用。所以要明白这一点：onSaveInstanceState() 方法并不是永远都会调用。比如，当用户在一个 Activity 点击返回键时，就不会调用，因为用户此时明确知道这个 Activity 是要被销毁的，并不期望下  
次它的样子跟现在一样（当然开发者可以使它保持临死时的表情，你非要这样做，系统也没办法），所以就不用调用onSaveInstanceState()。现在应该明白了：在onPause()、onStop() 以及 onDestroy() 中需要保  
存的是那些需要永久化的数据，而不是保存用于恢复状态的数据，状态数据有专门的方法：onSaveInstanceState()。数据保存在一个 Bundle 中，Bundle 被系统永久化。当再调用 Activity 的onCreate()时，原先  
保存的 Bundle就被传入，以恢复上一次临死时的模样，如果上次被销毁时没有保存 Bundle，则为 null。 还没完呢，如果你没有实现自己的 onSaveInstanceState()，但是 Activity 上控件的样子可能依然能被保  
存并恢复。原来 Activity 类已实现了onSaveInstanceState()，在 onSaveInstanceState() 的默认实现中，会调用所有控件的相关方法，把控件们的状态都保存下来，比如 EditText 中输入的文字、CheckBox  
是否被选中等等。然而不是所有的控件都能被保存，这取决于你是否在 layout 文件中为控件赋了一个名字(android:id)。有名的就存，无名的不管。既然有现成的可用，那么我们到底还要不要自己实现   
onSaveInstanceState() 方法呢？这就得看情况了，如果你自己的派生类中有变量影响到UI，或你程序的行为，当然就要把这个变量也保存了，那么就需要自己实现，否则就不需要，但大多数情况肯定需要自己实现一下下了。
别忘了在实现中调用父类的 onSaveInstanceState() 方法。 


# onSaveInstanceState 执行的情况

* 当用户按下HOME键时。 这是显而易见的，系统不知道你按下HOME后要运行多少其他的程序，自然也不知道activity A是否会被销毁，故系统会调用onSaveInstanceState，让用户有机会保存某些非永久性的数据。以下几种情况的分析都遵循该原则
* 长按HOME键，选择运行其他的程序时。
* 按下电源按键（关闭屏幕显示）时。
* 从activity A中启动一个新的activity时。
* 屏幕方向切换时，例如从竖屏切换到横屏时。 

# onRestoreInstanceState

onRestoreInstanceState用于恢复之前临时保存的activity状态,触发时机：

1、activity被回收  
  当activity被切换到后台或者启动新的activity后，原有activity就会被销毁，再次回来时，就会执行onRestoreInstanceState了。

2、屏幕旋转、改变系统语言、字体大小变化


onSaveInstanceState()会在onPause()或onStop()之前执行，onRestoreInstanceState()会在onStart()和onResume()之间执行。


onRestoreInstanceState()会跟onSaveInstanceState()并不会成对出现，onSaveInstanceState()需要调用的时，activity可能销毁，也可能没有销毁，只有在activity销毁重建的时候onRestoreInstanceState()才会调用。