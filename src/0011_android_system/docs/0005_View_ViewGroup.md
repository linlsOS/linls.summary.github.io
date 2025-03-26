# View ViewGroup

View与ViewGroup关系

# Author Info

 Field  | Info
--------|----------
 Author | linliangsong 
 Date   | 2025-3-25
 Email  | linliangsong@boe.com.cn 

# 参考文档

* [Android中View和ViewGroup有什么区别和联系](https://blog.csdn.net/m0_46368082/article/details/137029845)

# View与ViewGroup关系
 ```
 1. 职责：

    View：View是所有UI组件的父类，它负责绘制自己的内容，并处理与其相关的用户事件。每个View对象都会在屏幕上显示为一个矩形区域，并且可以响应用户的触摸、点击等交互操作。

    ViewGroup：ViewGroup是View的子类，它除了具备View的所有特性外，还负责管理子View的布局。ViewGroup可以包含多个子View，并负责测量、布局这些子View，以及处理事件的分发。简而言之，ViewGroup是一个用于存放其他View（和ViewGroup）对象的布局容器。

2. 事件处理：

    View：View处理它自身范围内的事件。如果一个事件发生在View的范围内，View会先处理这个事件，如果事件没有被View处理，它会传递给父View。

    ViewGroup：ViewGroup可以截取事件，在事件传递给子View之前，ViewGroup可以先处理事件，或者决定不将事件传递给子View。ViewGroup有onInterceptTouchEvent方法来判断是否截取事件，以及dispatchTouchEvent方法来分发事件。

联系：

1. 继承关系：

    ViewGroup继承自View，这意味着所有的ViewGroup都是View的实例，它们拥有View的全部特性。

2. 复合关系：

    View可以成为ViewGroup的一部分。在ViewGroup中，可以添加其他View或ViewGroup作为其子View，从而构建复杂的布局结构。

3. 事件传递：

    View和ViewGroup之间存在事件传递的关系。事件首先由ViewGroup处理，如果ViewGroup决定不处理事件，或者事件没有被消费，它才会传递给子View。

4. 性能考量：

    在性能敏感的应用中，开发者需要谨慎地使用自定义View和自定义ViewGroup，因为其实现可能会导致性能问题。例如，不必要的View绘制和事件处理都可能引起性能瓶颈。
 ```

