#  YUV2RGB各种图像原始数据之间的转换

 YUV，RGB各种图像原始数据之间的转换

# 参考文档

* [FFMPEG 实现 YUV，RGB各种图像原始数据之间的转换](https://blog.csdn.net/leixiaohua1020/article/details/14215391)



* 
在FFMPEG中，图像原始数据包括两种：planar和packed。planar就是将几个分量分开存，比如YUV420中，data[0]专门存Y，data[1]专门存U，data[2]专门存V。而packed则是打包存，所有数据都存在data[0]中
