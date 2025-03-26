# ffmpeg_ffplay

ffplay 分析

# menu

# 参考文档

* [ffplay Documentation](https://ffmpeg.org/ffplay.html)
* [ffmpeg、ffplay、ffprobe 常用命令详解](https://zhuanlan.zhihu.com/p/627834524)
* [[总结]FFMPEG命令行工具之ffplay详解](https://blog.csdn.net/ice_ly000/article/details/87906003?spm=1001.2101.3001.6661.1&utm_medium=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7EOPENSEARCH%7ERate-1-87906003-blog-133438491.235%5Ev38%5Epc_relevant_anti_vip_base&depth_1-utm_source=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7EOPENSEARCH%7ERate-1-87906003-blog-133438491.235%5Ev38%5Epc_relevant_anti_vip_base&utm_relevant_index=1)
* [ffmpeg、ffplay、ffprobe命令使用](https://www.cnblogs.com/lifexy/p/12871270.html)
* [ffplay---源码解析（零）：ffmpeg-4.2.1代码架构简述](https://blog.csdn.net/qq_42024067/article/details/104882893)
* [FFplay源代码分析：整体流程图](https://blog.csdn.net/leixiaohua1020/article/details/11980843)
* [ffmpeg处理RTMP流媒体的命令大全](https://blog.csdn.net/leixiaohua1020/article/details/12029543)
* [FFMPEG](https://blog.csdn.net/leixiaohua1020/category_1360795_2.html)
* [FFMPEG - ffplay源代码分析](https://www.cnblogs.com/schips/p/11525418.html)

# 代码流程

![0005_ffplay_code.png](images/0005_ffplay_code.png)


# 代码堆栈

```
* main
  * if (SDL_Init (flags))
  * SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
  * SDL_EventState(SDL_USEREVENT, SDL_IGNORE);
  * window = SDL_CreateWindow(program_name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, default_width, default_height, flags);
  * SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  * is = stream_open(input_filename, file_iformat);
    * is = av_mallocz(sizeof(VideoState));   //数据结构申请
    * is->filename = av_strdup(filename);    //获取播放文件名
    * if (frame_queue_init(&is->pictq, &is->videoq, VIDEO_PICTURE_QUEUE_SIZE, 1) < 0)//初始化
    * if (packet_queue_init(&is->videoq) < 0 ||   //
    * is->read_tid = SDL_CreateThread(read_thread, "read_thread", is);//读取线程同SDL线程绑定
      * static int read_thread(void *arg)
        * pkt = av_packet_alloc();
        * ic = avformat_alloc_context();
        * ic->interrupt_callback.callback = decode_interrupt_cb;
        * err = avformat_open_input(&ic, is->filename, is->iformat, &format_opts);
        * st_index[AVMEDIA_TYPE_VIDEO] = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, st_index[AVMEDIA_TYPE_VIDEO], -1, NULL, 0);
          * decoder = ff_find_decoder(ic, st, par->codec_id);
        * stream_component_open(is, st_index[AVMEDIA_TYPE_AUDIO]);
          * codec = avcodec_find_decoder(avctx->codec_id);
            * find_codec(id, av_codec_is_decoder)
          * decoder_init
          * decoder_start
            * audio_thread
            * video_thread
              * ret = get_video_frame(is, frame);
                * if ((got_picture = decoder_decode_frame(&is->viddec, frame, NULL)) < 0)
                  * ret = avcodec_receive_frame(d->avctx, frame);
                  * ret = queue_picture(is, frame, pts, duration, frame->pkt_pos, is->viddec.pkt_serial);
                    * frame_queue_push(&is->pictq);
                  * av_frame_unref(frame);
            * subtitle_thread
        * ret = stream_component_open(is, st_index[AVMEDIA_TYPE_VIDEO]);
        * stream_component_open(is, st_index[AVMEDIA_TYPE_SUBTITLE]);
        * for (;;) 
          * if (is->seek_req)
            * ret = avformat_seek_file(is->ic, -1, seek_min, seek_target, seek_max, is->seek_flags);
            * packet_queue_flush(&is->videoq);
            * packet_queue_put(&is->videoq, pkt);
            * packet_queue_put_nullpacket(&is->videoq, pkt, is->video_stream);
          * ret = av_read_frame(ic, pkt);
          * packet_queue_put(&is->videoq, pkt);
  * event_loop
    * refresh_loop_wait_event(cur_stream, &event);
      * video_refresh(is, &remaining_time);
        * video_display(is);
          * video_open(is); //SDL Window设置

```

# 理解ffmpeg解码流程：
```
* 查找解码器
* 根据查找的解码器初始化AVCodecContext
* 根据初始化的AVCodecContext 打开解码器
* 初始化packet、初始化frame（packet是数据接收的最小单元）
* 循环读取数据，获取帧数据，进行解码

解码结束
* 关闭解码器（停止数据的收取、数据）
* 释放AVCodecContext
* 释放packet
* 释放frame
```

# ffmpeg 音视频播放流程
```
* 调用avformat_alloc_context，创建封装格式上下文
* avformat_open_input, 打开文件
```

