# ffmpeg  av demux

av demux 解复用读取音视频包 

# 参考文档

* [FFmpeg实战 - 解复用与解码](https://blog.csdn.net/m0_73759312/article/details/140784264)
ffmpeg源码分析：avcodec_send_packet和avcodec_receive_frame（原avcodec_decode_video2()）https://blog.csdn.net/yangguoyu8023/article/details/107714311
* [FFmpeg源码分析：avcodec_send_packet()与avcodec_receive_frame()音视频解码](https://blog.csdn.net/u011686167/article/details/123242718)
* [FFmpeg的H.264解码器源代码简单分析：解析器（Parser）部分](https://blog.csdn.net/leixiaohua1020/article/details/45001033)
* [FFmpeg的H.264解码器源代码简单分析：解码器主干部分](https://blog.csdn.net/leixiaohua1020/article/details/45042755)
* [FFmpeg学习：frame的复制和拷贝](https://www.cnblogs.com/zjacky/p/16587147.html)

# ffmpeg 数据流
![0004_ffmpeg_demux_codec.png](images/0004_ffmpeg_demux_codec.png)


# 代码堆栈
```
* int av_read_frame(AVFormatContext *s, AVPacket *pkt)
  * FFFormatContext *const si = ffformatcontext(s);
  * ret = read_frame_internal(s, pkt);
    * static int read_frame_internal(AVFormatContext *s, AVPacket *pkt)
      * while (!got_packet && !si->parse_queue) {
        * ret = ff_read_packet(s, pkt);
          * int ff_read_packet(AVFormatContext *s, AVPacket *pkt)
            * for (;;) {
              * err = s->iformat->read_packet(s, pkt);
              * err = av_packet_make_refcounted(pkt);
              * if ((err = probe_codec(s, st, pkt1)) < 0)
                * static int probe_codec(AVFormatContext *s, AVStream *st, const AVPacket *pkt)
        * if ((ret = parse_packet(s, pkt, pkt->stream_index, 0)) < 0)
          * static int parse_packet(AVFormatContext *s, AVPacket *pkt, int stream_index, int flush)
            * len = av_parser_parse2(sti->parser, sti->avctx, &out_pkt->data, &out_pkt->size, data, size, pkt->pts, pkt->dts, pkt->pos);
            * int av_parser_parse2(AVCodecParserContext *s, AVCodecContext *avctx, uint8_t **poutbuf, int  *poutbuf_size, const uint8_t *buf, int buf_size, int64_t pts, int64_t dts, int64_t pos)
              * index = s->parser->parser_parse(s, avctx, (const uint8_t **) poutbuf, poutbuf_size, buf, buf_size); // 这里对应音视频解码的解析
                * libavcodec/h264_parser.c
                  * static int h264_parse(AVCodecParserContext *s, AVCodecContext *avctx, const uint8_t **poutbuf, int *poutbuf_size, const uint8_t *buf, int buf_size)
                    * if (!p->got_first) 
                      * ff_h264_decode_extradata(avctx->extradata, avctx->extradata_size, &p->ps, &p->is_avc, &p->nal_length_size, avctx->err_recognition, avctx); //首次解析SPS、PPS
                    * parse_nal_units(s, avctx, buf, buf_size); // 解析NALU，从SPS、PPS、SEI等中获得一些基本信息
```

*  h264_parse 函数功能
```
（1）如果是第一次解析，则首先调用ff_h264_decode_extradata()解析AVCodecContext的extradata（里面实际上存储了H.264的SPS、PPS）。
（2）如果传入的flags 中包含PARSER_FLAG_COMPLETE_FRAMES，则说明传入的是完整的一帧数据，不作任何处理；如果不包含PARSER_FLAG_COMPLETE_FRAMES，则说明传入的不是完整的一帧数据而是任意一段H.264数据，则需要调用h264_find_frame_end()通过查找“起始码”（0x00000001或者0x000001）的方法，分离出完整的一帧数据。
（3）调用parse_nal_units()完成了NALU的解析工作。
```

# avcodec_send_packet
```
* int attribute_align_arg avcodec_send_packet(AVCodecContext *avctx, const AVPacket *avpkt)
  ├── if (!avcodec_is_open(avctx) || !av_codec_is_decoder(avctx->codec))  //编码器是否打开
  ├── ret = av_packet_ref(avci->buffer_pkt, avpkt);  //avpkt 拷贝到avci->buffer_pkt
  │   └── int av_packet_ref(AVPacket *dst, const AVPacket *src)
  │       └── ret = av_packet_copy_props(dst, src);
  ├── ret = av_bsf_send_packet(avci->bsf, avci->buffer_pkt);
  │   └── int av_bsf_send_packet(AVBSFContext *ctx, AVPacket *pkt)
  │       ├── ret = av_packet_make_refcounted(pkt);
  │       └── av_packet_move_ref(bsfi->buffer_pkt, pkt);
  │           └── void av_packet_move_ref(AVPacket *dst, AVPacket *src)
  │               └── *dst = *src;
  └── ret = decode_receive_frame_internal(avctx, avci->buffer_frame);
      └── static int decode_receive_frame_internal(AVCodecContext *avctx, AVFrame *frame)
          ├── if (avctx->codec->receive_frame) //判断AVCodec是否存在 receive_frame 这个函数指针,如果存在
          └── else
              └── ret = decode_simple_receive_frame(avctx, frame);
                  └── static int decode_simple_receive_frame(AVCodecContext *avctx, AVFrame *frame)
                      └── while (!frame->buf[0]) {
                          └── ret = decode_simple_internal(avctx, frame, &discarded_samples); 
                              └── static inline int decode_simple_internal(AVCodecContext *avctx, AVFrame *frame, int64_t *discarded_samples)
                                  ├── if (HAVE_THREADS && avctx->active_thread_type & FF_THREAD_FRAME)
                                  │   └── ret = ff_thread_decode_frame(avctx, frame, &got_frame, pkt);
                                  └── else
                                      └── ret = avctx->codec->decode(avctx, frame, &got_frame, pkt); // 调用了AVCodec的decode函数
```

# avcodec_receive_frame
```
* int attribute_align_arg avcodec_receive_frame(AVCodecContext *avctx, AVFrame *frame)
  └── ret = decode_receive_frame_internal(avctx, frame);
      └── static int decode_receive_frame_internal(AVCodecContext *avctx, AVFrame *frame)
          ├── if (avctx->codec->receive_frame)   //这里判定对应的解码器是否包含receive_frame
          └── } else
              └── ret = decode_simple_receive_frame(avctx, frame);
                  └── static int decode_simple_receive_frame(AVCodecContext *avctx, AVFrame *frame)
                      └── while (!frame->buf[0]) {
                          └── ret = decode_simple_internal(avctx, frame, &discarded_samples);
                              └── static inline int decode_simple_internal(AVCodecContext *avctx, AVFrame *frame, int64_t *discarded_samples)
                                  └── ret = ff_decode_get_packet(avctx, pkt);
                                      ├── int ff_decode_get_packet(AVCodecContext *avctx, AVPacket *pkt)
                                      │   ├── ret = av_bsf_receive_packet(avci->bsf, pkt);//取出pkt并将ctx->internal->buffer_pkt指向一个空白的pkt
                                      │   └── ret = apply_param_change(avctx, pkt);  //判断channels、width、height、sample_rate是否需要更新
                                      └── ret = avctx->codec->decode(avctx, frame, &got_frame, pkt); // 传入数据,根据不同编码进行解码，如下以H264为例
                                          └── libavcodec/h264dec.c
                                              └── static int h264_decode_frame(AVCodecContext *avctx, void *data, int *got_frame, AVPacket *avpkt)
                                                  ├── buf_index = decode_nal_units(h, buf, buf_size);
                                                  └── ret = finalize_frame(h, pict, h->next_output_pic, got_frame);
                                                      └── static int finalize_frame(H264Context *h, AVFrame *dst, H264Picture *out, int *got_frame)
                                                          └── ret = output_frame(h, dst, out);
                                                              └── static int output_frame(H264Context *h, AVFrame *dst, H264Picture *srcp)
                                                                  └── static int output_frame(H264Context *h, AVFrame *dst, H264Picture *srcp)
                                                                      └── ret = av_frame_ref(dst, srcp->needs_fg ? srcp->f_grain : srcp->f);
                                                                          └── int av_frame_ref(AVFrame *dst, const AVFrame *src)
                                                                              ├── ret = frame_copy_props(dst, src, 0);//拷贝各种属性值，创建side_data指针并将src中的值赋值给它
                                                                              └── ret = av_frame_get_buffer(dst, 0);
                                                                                  └── int av_frame_get_buffer(AVFrame *frame, int align)
                                                                                      ├── if (frame->width > 0 && frame->height > 0)
                                                                                      │   └── return get_video_buffer(frame, align); // 获取视频buff
                                                                                      └── else if (frame->nb_samples > 0 && (frame->channel_layout || frame->channels > 0))
                                                                                          └── return get_audio_buffer(frame, align); // 获取音频buff
```