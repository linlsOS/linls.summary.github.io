# ffmpeg avformat alloc context

avformat_alloc_context 分析

# 参考文档

* [avformat_alloc_context](https://blog.csdn.net/weixin_42123332/article/details/120675728)


# 函数作用

函数用来申请AVFormatContext类型变量并初始化默认参数
该函数用于分配空间创建一个AVFormatContext对象，并且强调使用avformat_free_context方法来清理并释放该对象的空间。
* 分配空间，创建一个 AVFormatContext对象。
* avformat_free_context() 可用于释放上下文和所有内容

# 代码分析

```cpp
AVFormatContext *avformat_alloc_context(void)
{
    //分配空间，创建 FFFormatContext
    FFFormatContext *const si = av_mallocz(sizeof(*si));
    AVFormatContext *s;

    if (!si)
        return NULL;

    s = &si->pub;
    s->av_class = &av_format_context_class;
    s->io_open  = io_open_default;  // 回调函数赋值
    s->io_close = io_close_default;  // 回调函数赋值

    av_opt_set_defaults(s);

    si->pkt = av_packet_alloc();
    si->parse_pkt = av_packet_alloc();
    if (!si->pkt || !si->parse_pkt) {
        avformat_free_context(s);
        return NULL;
    }

    si->offset = AV_NOPTS_VALUE;
    si->raw_packet_buffer_remaining_size = RAW_PACKET_BUFFER_SIZE;
    si->shortest_end = AV_NOPTS_VALUE;

    return s;
}
```

# 代码堆栈

* avformat_alloc_context  //通过堆栈查看如下frgment主要接口、结构图四处是哈
```
* libavformat/options.c 
  └── AVFormatContext *avformat_alloc_context(void)
      ├── FFFormatContext *const si = av_mallocz(sizeof(*si)); // 申请FFFormatContext 空间
      ├── s = &si->pub;  //获取AVFormatContext
      ├── s->av_class = &av_format_context_class;
      ├── s->io_open  = io_open_default;  /回调函数赋值
      ├── s->io_close = io_close_default;  // 回调函数赋值
      ├── av_opt_set_defaults(s);
      │   └── void av_opt_set_defaults(void *s)
      │       └── av_opt_set_defaults2(s, 0, 0);
      │           └── void av_opt_set_defaults2(void *s, int mask, int flags)//不断取出对应的AVOption对象，将AVOption对象中存储的初始值赋值给s对象
      │               └── while ((opt = av_opt_next(s, opt)))
      ├── si->pkt = av_packet_alloc();
      └── si->parse_pkt = av_packet_alloc();
          └── AVPacket *av_packet_alloc(void)
              └── get_packet_defaults(pkt);   //pts 初始化
                  └── static void get_packet_defaults(AVPacket *pkt)
                      ├── pkt->pts             = AV_NOPTS_VALUE;
                      └── pkt->dts             = AV_NOPTS_VALUE;
```

* init
```
* player.init(mCurrentVideoBean.getVideoPath(),VIDEO_RENDER_ANWINDOW,mSurface);
  └── mNativePlayerHandle = native_Init(url, videoRenderType, surface);
      ├── FFMediaPlayer *player = new FFMediaPlayer();
      └── player->Init(env, obj, const_cast<char *>(url), renderType, surface);
          ├── m_VideoDecoder = new VideoDecoder(url);
          │   └── Init(url, AVMEDIA_TYPE_VIDEO);
          │       └── m_MediaType = mediaType;
          ├── m_AudioDecoder = new AudioDecoder(url);
          │   └── Init(url, AVMEDIA_TYPE_VIDEO);
          │       └── m_MediaType = mediaType;
          ├── m_VideoRender = new NativeRender(jniEnv, surface);
          ├── m_VideoDecoder->SetVideoRender(m_VideoRender);
          ├── m_AudioRender = new OpenSLRender();
          ├── m_AudioDecoder->SetAudioRender(m_AudioRender);
          ├── m_VideoDecoder->SetMessageCallback(this, PostMessage);
          └── m_AudioDecoder->SetMessageCallback(this, PostMessage);
```

* play

```
* player.play();
  └── native_Play(mNativePlayerHandle);
      └── ffMediaPlayer->Play();
          ├── m_VideoDecoder->Start();
          │   └── StartDecodingThread();
          │       └── m_Thread = new thread(DoAVDecoding, this);
          │           ├── if (decoder->InitFFDecoder() != 0) 
          │           │   ├── m_AVFormatContext = avformat_alloc_context();
          │           │   ├── error = avformat_open_input(&m_AVFormatContext, m_Url, NULL, NULL) ;
          │           │   ├── if (avformat_find_stream_info(m_AVFormatContext, NULL) < 0)
          │           │   ├── for (int i = 0; i < m_AVFormatContext->nb_streams; i++) 
          │           │   │   └── if (m_AVFormatContext->streams[i]->codecpar->codec_type == m_MediaType) 
          │           │   │       └── m_StreamIndex = i;
          │           │   ├── AVCodecParameters *codecParameters = m_AVFormatContext->streams[m_StreamIndex]->codecpar;
          │           │   ├── m_AVCodec = avcodec_find_decoder(codecParameters->codec_id);
          │           │   ├── m_AVCodecContext = avcodec_alloc_context3(m_AVCodec);
          │           │   ├── if (avcodec_parameters_to_context(m_AVCodecContext, codecParameters) != 0)
          │           │   ├── result = avcodec_open2(m_AVCodecContext, m_AVCodec, &pAVDictionary);
          │           │   ├── m_Packet = av_packet_alloc();
          │           │   └── m_Frame = av_frame_alloc();
          │           ├── decoder->OnDecoderReady();
          │           │   ├── m_MsgCallback(m_MsgContext, MSG_DECODER_READY, 0);
          │           │   ├── m_VideoRender->Init(m_VideoWidth, m_VideoHeight, dstSize);
          │           │   ├── m_RGBAFrame = av_frame_alloc();
          │           │   ├── int bufferSize = av_image_get_buffer_size(DST_PIXEL_FORMAT, m_RenderWidth, m_RenderHeight, 1);
          │           │   └── m_FrameBuffer = (uint8_t *) av_malloc(bufferSize * sizeof(uint8_t));
          │           └── decoder->DecodingLoop();
          │               └── if (DecodeOnePacket() != 1) 
          │                   └── int result = av_read_frame(m_AVFormatContext, m_Packet);
          │                       └── while (result == 0) 
          │                           ├── if (avcodec_send_packet(m_AVCodecContext, m_Packet) == AVERROR_EOF)
          │                           ├── while (avcodec_receive_frame(m_AVCodecContext, m_Frame) == 0) 
          │                           │   ├── UpdateTimeStamp();
          │                           │   ├── AVSync();
          │                           │   └── OnFrameAvailable(m_Frame);
          │                           ├── av_packet_unref(m_Packet);
          │                           └── result = av_read_frame(m_AVFormatContext, m_Packet);   
          └── m_AudioDecoder->Start();
```

