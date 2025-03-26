# ffmpeg avcodec find decoder

avcodec_find_decoder

# 参考文档

* [FFmpeg源代码简单分析：av_find_decoder()和av_find_encoder()](https://blog.csdn.net/leixiaohua1020/article/details/44084557)
* [[ffmpeg] avcodec_alloc_context3 解析](https://blog.csdn.net/dss875914213/article/details/134841189)

# 简介

* avcoec_find_decoder
在FFmpeg库中，‌avcodec_find_encoder函数是一个重要的函数，‌它允许开发者查找并获取一个特定的视频编码器的信息。‌这个函数通过枚举AVCodecID来查找注册的编码器，‌返回一个指向AVCodec结构的指针，‌该结构包含了编码器的详细信息，‌如编码器的名称、‌支持的像素格式、‌支持的分辨率等。‌通过这个函数，‌开发者可以确定FFmpeg库是否支持他们需要的特定编码格式，‌从而进行相应的视频编码操作。‌  

此外，‌avcodec_find_encoder函数还提供了通过编码器名称查找的功能，‌即avcodec_find_encoder_by_name，‌这使得开发者可以根据编码器的名称直接获取到对应的编码器信息。‌这种灵活性使得FFmpeg库能够适应不同的应用场景和需求。‌  

总的来说，‌avcodec_find_encoder函数是FFmpeg库中用于视频编码的关键功能之一，‌它提供了查找和获取特定视频编码器信息的能力，‌从而使得开发者能够根据需要选择合适的编码器进行视频编码操作。  

* avodec_alloc_context3  
  主要是创建了 AVCodecContext ,并给结构体参数赋予初值。  
  初值设置主要分成两块:  
    1) 所有编码器都相同的部分；  
    2) 每个编码器独有的参数设置。  

#  代码堆栈‌

* avcodec_find_decoder
```
* libavcodec/allcodecs.c
  └── const AVCodec *avcodec_find_decoder(enum AVCodecID id)
      └── return find_codec(id, av_codec_is_decoder);
          └── static const AVCodec *find_codec(enum AVCodecID id, int (*x)(const AVCodec *))
              ├── id = remap_deprecated_codec_id(id);  // return id, do nothing
              └── while ((p = av_codec_iterate(&i)))  // 遍历codec list，找到最佳解码器
                  └── const AVCodec *av_codec_iterate(void **opaque)
                      ├── const AVCodec *c = codec_list[i];
                      ├── ff_thread_once(&av_codec_static_init, av_codec_init_static); // ff_thread_once保证在多线程调用的时候，函数只执行一次
                      └── *opaque = (void*)(i + 1);
```

* avcodec_alloc_context3
```
* libavcodec/options.c 
  * AVCodecContext *avcodec_alloc_context3(const AVCodec *codec)
    * AVCodecContext *avctx= av_malloc(sizeof(AVCodecContext));
    * if (init_context_defaults(avctx, codec) < 0)
      * static int init_context_defaults(AVCodecContext *s, const AVCodec *codec)
        * s->get_buffer2         = avcodec_default_get_buffer2;
        * s->get_format          = avcodec_default_get_format;
        * s->get_encode_buffer   = avcodec_default_get_encode_buffer;
        * s->execute             = avcodec_default_execute;
        * s->execute2            = avcodec_default_execute2; 
```
