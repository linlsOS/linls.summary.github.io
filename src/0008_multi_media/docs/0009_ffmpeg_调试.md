# ffmpeg 调试

ffmepg 调试遇到的问题

# 参考文档

* [C++引用ffmpeg出错#error missing -D__STDC_CONSTANT_MACROS / #define __STDC_CONSTANT_MACROS](https://blog.csdn.net/jctian000/article/details/84580813)
* [FFMPEG Tips (1) 如何打印日志](https://zhuanlan.zhihu.com/p/23284574#:~:text=%E9%80%9A%E8%BF%87%20FFMPEG%20%E7%9A%84%20av_log_set_callback%20%28%29%20%E6%B3%A8%E5%86%8C%E4%B8%80%E4%B8%AA%20LOG%20callback,syslog_init%20%28%29%20%E5%90%8E%EF%BC%8C%E5%B0%B1%E5%8F%AF%E4%BB%A5%E4%BD%BF%E7%94%A8%20av_log%20%28%29%20%E5%9C%A8%20Android%20%E5%B9%B3%E5%8F%B0%E8%BE%93%E5%87%BA%E8%B0%83%E8%AF%95%E6%97%A5%E5%BF%97%E4%BA%86%E3%80%82)

# android studio 引入.h 提示出错

```
include/libavutil/common.h:45:2: error: missing -D__STDC_CONSTANT_MACROS / #define __STDC_CONSTANT_MACROS
```

* 解决方案

在libavutil/common.h中添加如下定义
```h
#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
# include "stdint.h"
#endif

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif
```

# 在安卓项目中打开ffmpeg的日志

默认ffmpeg的日志并不打印，需要添加对应函数的调用,在主函数中调用syslog_init，就可以打印对应ffmpeso的日志

```cpp
#define  ALOG(level,TAG, ...)  ((void)__android_log_vprint(level, TAG, __VA_ARGS__))
#define SYS_LOG_TAG "ffmpeglib_demo"

static void syslog_print(void *ptr, int level, const char *fmt, va_list vl){
    switch (level) {
        case AV_LOG_DEBUG:
            ALOG(ANDROID_LOG_DEBUG, SYS_LOG_TAG, fmt, vl);
            break;
        case AV_LOG_VERBOSE:
            ALOG(ANDROID_LOG_VERBOSE, SYS_LOG_TAG, fmt, vl);
            break;
        case AV_LOG_INFO:
            ALOG(ANDROID_LOG_INFO, SYS_LOG_TAG, fmt, vl);
            break;
        case AV_LOG_WARNING:
            ALOG(ANDROID_LOG_WARN, SYS_LOG_TAG, fmt, vl);
            break;
        case AV_LOG_ERROR:
            ALOG(ANDROID_LOG_ERROR, SYS_LOG_TAG, fmt, vl);
            break;
    }
}

static void syslog_init(){
    av_log_set_callback(syslog_print);
}

```
