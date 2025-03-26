# ffmpeg avformat find stream info

avformat_find_stream_info 简介

# menu

* [1 参考文档](#1-参考文档)
* [2 简介](#2-简介)
* [3 代码堆栈](#3-代码堆栈)
  * [3.1 avcodec_parameters_to_context](#31-avcodec_parameters_to_context)

# 1 参考文档

* [FFmpeg源码分析：avformat_find_stream_info分析码流信息](https://zhuanlan.zhihu.com/p/640854999)
* [0012_ffmpeg_avformat_open_input.md](0012_ffmpeg_avformat_open_input.md)
* [ffmpeg之avformat_find_stream_info函数详细解释](https://blog.csdn.net/qq_51282224/article/details/130993130)
* [FFmpeg源代码简单分析：avformat_find_stream_info()](https://blog.csdn.net/leixiaohua1020/article/details/44084321)
* [ffmpeg 源代码简单分析 ： av_read_frame()](https://blog.csdn.net/leixiaohua1020/article/details/12678577)


# 2 简介

用于获取媒体文件中每个音视频流的详细信息的函数，包括解码器类型、采样率、声道数、码率、关键帧等信息
avformat_find_stream_info函数的主要功能是通过读取媒体文件的多个数据包来获取流信息。‌

这个函数特别适用于那些没有头部信息的文件格式，‌如MPEG，‌它通过读取多个数据包来获取流信息。‌此外，‌对于支持repeat mode的格式，‌如MPEG-2，‌该函数还会计算真实的帧率。‌重要的是，‌这个函数不会修改逻辑文件位置，‌读取的数据包会被缓存起来供后续处理使用。‌

在实现上，‌avformat_find_stream_info通过多个循环来完成工作，‌这些循环围绕着每个流中的info结构展开，‌特别是处理DTS（‌解码时间戳）‌相关的信息。‌这个函数在libavformat包下，‌是FFmpeg库的一部分，‌用于解析媒体文件并提取流信息，‌这对于理解和处理媒体文件的内容非常重要。‌

尽管这个函数不会修改逻辑文件位置，‌但它确实会读取并缓存数据包，‌以便在后续处理中使用。‌这种缓存机制允许函数在需要时重新访问和处理这些数据包，‌从而提取出有用的流信息。‌此外，‌该函数还考虑了用户可能不需要的所有信息，‌通过缓存未使用的数据包来避免浪费时间和资源

主要包含如下几个关键步骤
1、av_parser_init 遍历查找解码器
2、avcodec_parameters_to_context 音视频参数赋值
3、find_probe_decoder 获取对应解码器
4.打开解码器：avcodec_open2()
5.读取完整的一帧压缩编码的数据：read_frame_internal()
注：av_read_frame()内部实际上就是调用的read_frame_internal()。
6.解码一些压缩编码数据：try_decode_frame() 

# 3 代码堆栈

## avformat_find_stream_info
```
* int avformat_find_stream_info(AVFormatContext *ic, AVDictionary **options)
  ├── for (unsigned i = 0; i < ic->nb_streams; i++)
  │   └── sti->parser = av_parser_init(st->codecpar->codec_id);  // st->codecpar->codec_id的获取可以看avformat_open_input如何获取如何获取音视频参数
  │       └── AVCodecParserContext *av_parser_init(int codec_id)
  │           └── while ((parser = av_parser_iterate(&i))) 
  │               ├── const AVCodecParser *av_parser_iterate(void **opaque)
  │               │   ├── uintptr_t i = (uintptr_t)*opaque;
  │               │   └── const AVCodecParser *p = parser_list[i];
  │               │       └── static const AVCodecParser * const parser_list[] = { &ff_aac_parser, …………};
  │               └── s->parser = parser;
  ├── ret = avcodec_parameters_to_context(avctx, st->codecpar);
  │   └── int avcodec_parameters_to_context(AVCodecContext *codec, const AVCodecParameters *par)// 这里拷贝音视频参数
  ├── codec = find_probe_decoder(ic, st, st->codecpar->codec_id); // 获取解码器，这里st->codecpar->codec_id在avformat_open_input中获取
  │   └── static const AVCodec *find_probe_decoder(AVFormatContext *s, const AVStream *st, enum AVCodecID codec_id)
  │       ├── if (codec_id == AV_CODEC_ID_H264)
  │       │   └── return avcodec_find_decoder_by_name("h264");
  │       │       └── static const AVCodec *find_codec_by_name(const char *name, int (*x)(const AVCodec *))
  │       │           └── while ((p = av_codec_iterate(&i))) {
  │       │               └── const AVCodec *av_codec_iterate(void **opaque)
  │       │                   ├── uintptr_t i = (uintptr_t)*opaque;
  │       │                   ├── const AVCodec *c = codec_list[i];
  │       │                   └── ff_thread_once(&av_codec_static_init, av_codec_init_static);
  │       ├── codec = ff_find_decoder(s, st, codec_id);
  │       │   └── const AVCodec *ff_find_decoder(AVFormatContext *s, const AVStream *st, enum AVCodecID codec_id)
  │       │       ├── switch (st->codecpar->codec_type)
  │       │       │   ├── case AVMEDIA_TYPE_VIDEO:
  │       │       │   │   └── return s->video_codec;
  │       │       │   ├── case AVMEDIA_TYPE_AUDIO:
  │       │       │   │   └── return s->audio_codec;
  │       │       │   └──  case AVMEDIA_TYPE_SUBTITLE:
  │       │       │       └── return s->subtitle_codec;
  │       │       └── return avcodec_find_decoder(codec_id);
  │       │           └── return find_codec(id, av_codec_is_decoder);
  │       │               └── static const AVCodec *find_codec(enum AVCodecID id, int (*x)(const AVCodec *))
  │       │                   ├── id = remap_deprecated_codec_id(id);
  │       │                   └── while ((p = av_codec_iterate(&i))) {
  │       │                       └── const AVCodec *av_codec_iterate(void **opaque)
  │       │                           ├── uintptr_t i = (uintptr_t)*opaque;
  │       │                           ├── const AVCodec *c = codec_list[i]; //遍历codc 中查找匹配的解码器
  │       │                           └── ff_thread_once(&av_codec_static_init, av_codec_init_static);            
  │       └──  while ((probe_codec = av_codec_iterate(&iter))) {
  │           ├── const AVCodec *av_codec_iterate(void **opaque)
  │           ├── const AVCodec *c = codec_list[i];
  │           └── ff_thread_once(&av_codec_static_init, av_codec_init_static);
  ├── avcodec_open2(avctx, codec, options ? &options[i] : &thread_opt) 
  │   └── int attribute_align_arg avcodec_open2(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options)
  │       ├── if (avcodec_is_open(avctx))
  │       └── lock_avcodec(codec);
  ├── ret = read_frame_internal(ic, pkt1);
  │   └── static int read_frame_internal(AVFormatContext *s, AVPacket *pkt)
  │       ├── ret = ff_read_packet(s, pkt);
  │       │   └── int ff_read_packet(AVFormatContext *s, AVPacket *pkt)
  │       │       └── err = s->iformat->read_packet(s, pkt);
  │       └── parse_packet(s, pkt, st->index, 1);  //解析出来AVPacket
  │           └── static int parse_packet(AVFormatContext *s, AVPacket *pkt, int stream_index, int flush)
  │               └── len = av_parser_parse2(sti->parser, sti->avctx, &out_pkt->data, &out_pkt->size, data, size, pkt->pts, pkt->dts, pkt->pos);
  │                   └── int av_parser_parse2(AVCodecParserContext *s, AVCodecContext *avctx, uint8_t **poutbuf, int *poutbuf_size, const uint8_t *buf, int buf_size, int64_t pts, int64_t dts, int64_t pos)
  │                       └── index = s->parser->parser_parse(s, avctx, (const uint8_t **) poutbuf,  poutbuf_size, buf, buf_size);
  ├── try_decode_frame(ic, st, pkt, (options && i < orig_nb_streams) ? &options[i] : NULL);// If still no information, we try to open the codec and to decompress the frame.
  │   └── static int try_decode_frame(AVFormatContext *s, AVStream *st, const AVPacket *avpkt, AVDictionary **options)
  │       ├── codec = find_probe_decoder(s, st, st->codecpar->codec_id);
  │       └── ret = avcodec_open2(avctx, codec, options ? options : &thread_opt);
  └── ff_rfps_calculate(ic);// 帧率计算
      └── void ff_rfps_calculate(AVFormatContext *ic)
```




## 3.1 avcodec_parameters_to_context

这里是将AVCodecParameters拷贝给AVCodecContext

```cpp
 int avcodec_parameters_to_context(AVCodecContext *codec,
                                  const AVCodecParameters *par)
{
    codec->codec_type = par->codec_type;
    codec->codec_id   = par->codec_id;
    codec->codec_tag  = par->codec_tag;

    codec->bit_rate              = par->bit_rate;
    codec->bits_per_coded_sample = par->bits_per_coded_sample;
    codec->bits_per_raw_sample   = par->bits_per_raw_sample;
    codec->profile               = par->profile;
    codec->level                 = par->level;

    switch (par->codec_type) {
    case AVMEDIA_TYPE_VIDEO:
        codec->pix_fmt                = par->format;
        codec->width                  = par->width;
        codec->height                 = par->height;
        codec->field_order            = par->field_order;
        codec->color_range            = par->color_range;
        codec->color_primaries        = par->color_primaries;
        codec->color_trc              = par->color_trc;
        codec->colorspace             = par->color_space;
        codec->chroma_sample_location = par->chroma_location;
        codec->sample_aspect_ratio    = par->sample_aspect_ratio;
        codec->has_b_frames           = par->video_delay;
        break;
    case AVMEDIA_TYPE_AUDIO:
        codec->sample_fmt       = par->format;
        codec->channel_layout   = par->channel_layout;
        codec->channels         = par->channels;
        codec->sample_rate      = par->sample_rate;
        codec->block_align      = par->block_align;
        codec->frame_size       = par->frame_size;
        codec->delay            =
        codec->initial_padding  = par->initial_padding;
        codec->trailing_padding = par->trailing_padding;
        codec->seek_preroll     = par->seek_preroll;
        break;
    case AVMEDIA_TYPE_SUBTITLE:
        codec->width  = par->width;
        codec->height = par->height;
        break;
    }

    if (par->extradata) {
        av_freep(&codec->extradata);
        codec->extradata = av_mallocz(par->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
        if (!codec->extradata)
            return AVERROR(ENOMEM);
        memcpy(codec->extradata, par->extradata, par->extradata_size);
        codec->extradata_size = par->extradata_size;
    }

    return 0;
}
```
