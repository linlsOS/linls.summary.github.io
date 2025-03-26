# ffmpeg avcodec open2

avcodec_open2

# menu

* [参考文档](#参考文档)
* [代码堆栈](#代码堆栈)

# 参考文档

* [FFmpeg源代码简单分析：avcodec_open2()](https://blog.csdn.net/leixiaohua1020/article/details/44117891)

# 简介
avcodec_open2函数是FFmpeg库中的一个重要函数，‌用于打开编解码器并初始化编解码器上下文。‌

avcodec_open2函数的主要功能包括：‌

    参数检查与设置：‌在调用avcodec_open2之前，‌必须确保已经正确设置了AVCodecContext中的所有必要参数，‌否则可能会导致初始化失败。‌这包括设置编码器的输出格式（‌如像素格式、‌分辨率等）‌，‌确保这些参数与编解码器兼容。‌

    初始化codec线程：‌avcodec_open2函数会进行参数设置和检查，‌并对编解码器进行线程初始化。‌这涉及到一些基本的参数设置与检查，‌以及对编解码器的加锁，‌以确保线程安全。‌

    初始化codec：‌在初始化过程中，‌会创建一个AVCodecDescriptor对象，‌这个对象从一个内部的全局表格中搜索得到，‌用于描述编解码器的特性。‌

使用avcodec_open2函数时，‌需要传递三个参数：‌一个指向AVCodecContext的指针（‌包含编解码器的上下文信息）‌，‌一个指向AVCodec的常量指针（‌描述编解码器本身）‌，‌以及一个可选的AVDictionary指针（‌用于传递额外的选项）‌。‌通过检查这些参数并成功执行，‌可以确保编解码器正确初始化，‌为后续的编码或解码操作做好准备。‌在实际应用中，‌应当检查avcodec_open2的返回值，‌以妥善处理可能发生的错误情况

# 代码堆栈

* avcodec_open2
```
* avcodec_open2
  * if (avcodec_is_open(avctx)) //确认解码器是否打开
    * int avcodec_is_open(AVCodecContext *s)
      * return !!s->internal;
  * avctx->codec = codec;
  * lock_avcodec(codec);
  * avci->buffer_frame = av_frame_alloc();
  * avci->buffer_pkt = av_packet_alloc();
  * avci->es.in_frame = av_frame_alloc();
  * avci->ds.in_pkt = av_packet_alloc();
  * avci->last_pkt_props = av_packet_alloc();
  * avci->pkt_props = av_fifo_alloc(sizeof(*avci->last_pkt_props));
  * avctx->codec_descriptor = avcodec_descriptor_get(avctx->codec_id);
  * if (av_codec_is_encoder(avctx->codec))
    * ret = ff_encode_preinit(avctx);
      * int ff_encode_preinit(AVCodecContext *avctx)
        * int ff_encode_preinit(AVCodecContext *avctx)
  * else
    * ret = ff_decode_preinit(avctx);
      * int ff_decode_preinit(AVCodecContext *avctx)
        * ret = decode_bsfs_init(avctx);
          * static int decode_bsfs_init(AVCodecContext *avctx)
            * ret = av_bsf_list_parse_str(avctx->codec->bsfs, &avci->bsf);
            * ret = avcodec_parameters_from_context(avci->bsf->par_in, avctx);//codec 参数赋值
            * ret = av_bsf_init(avci->bsf);
              * int av_bsf_init(AVBSFContext *ctx)
                * if (ctx->filter->codec_ids) {           //check that the codec is supported
                  * for (i = 0; ctx->filter->codec_ids[i] != AV_CODEC_ID_NONE; i++)
                    * if (ctx->par_in->codec_id == ctx->filter->codec_ids[i])
                      * break;
  * if (av_codec_is_decoder(avctx->codec))

```

* 