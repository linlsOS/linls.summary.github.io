# ffmpeg avfilter

ffmpeg 滤镜

# 参考文档

* [FFmpeg原始帧处理-滤镜API用法详解](https://www.cnblogs.com/leisure_chn/p/10429145.html#:~:text=%E4%B8%80%E4%B8%AA%E6%BB%A4%E9%95%9C%E7%9A%84%E8%BE%93%E5%87%BA%E5%8F%AF%E4%BB%A5%E8%BF%9E)
* [(原)ffmpeg过滤器开发和理解](https://www.cnblogs.com/lihaiping/p/avfilter.html#:~:text=%E6%9C%80%E8%BF%91%E5%AD%A6%E4%B9%A0%E4%BA%86ffmpe)


# ffmpeg 滤镜
在 FFmpeg 中，滤镜(filter)处理的是未压缩的原始音视频数据(RGB/YUV视频帧，PCM音频帧等)。一个滤镜的输出可以连接到另一个滤镜的输入，多个滤镜可以连接起来，构成滤镜链/滤镜图，各种滤镜的组合为 FFmpeg 提供了丰富的音视频处理功能。

比较常用的滤镜有：scale、trim、overlay、rotate、movie、yadif。scale 滤镜用于缩放，trim 滤镜用于帧级剪切，overlay 滤镜用于视频叠加，rotate 滤镜实现旋转，movie 滤镜可以加载第三方的视频，yadif 滤镜可以去隔行。
