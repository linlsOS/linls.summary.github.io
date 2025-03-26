# ffmpeg decode demo

ffmpeg 播放demo简介

# 参考文档

* [avformat_alloc_context](https://blog.csdn.net/weixin_42123332/article/details/120675728)
* [0008_ffmpeg_audio_decodder_demo.cpp](refers/0008_ffmpeg_audio_decodder_demo.cpp)


# ffpeg 视频解码流程

FFmpeg视频解码流程主要涉及以下几个步骤：‌

 1、打开媒体文件：‌使用avformat_open_input函数读取和打开视频文件。‌  
 2、查找流信息：‌通过avformat_find_stream_info函数获取流信息，‌包括视频流的索引。‌  
 3、查找解码器：‌根据视频流信息的codec_id，‌使用avcodec_find_decoder找到对应的解码器。‌  
 4、初始化解码器上下文：‌使用给定的AVCodec初始化AVCodecContext，‌这是解码过程中的核心结构体。‌  
 5、初始化输出结构体：‌初始化输出文件、‌解码AVPacket和AVFrame结构体，‌用于存储解码后的视频帧。‌  
 6、开始读取帧：‌使用av_read_frame开始一帧一帧地读取视频数据。‌  
 7、发送和解码数据包：‌使用avcodec_send_packet发送数据包给解码器，‌然后使用avcodec_receive_frame接收解码后的帧。‌  
 8、格式转换和渲染：‌对解码后的帧进行格式转换，‌并进行渲染（‌如果需要的话）‌。‌  

这个流程图展示了FFmpeg解码过程的主要步骤，‌从打开媒体文件到解码每一帧数据，‌再到可能的格式转换和渲染1。‌在解码过程中，‌还需要注意释放申请的变量和内存，‌以确保资源被正确管理2。‌


# 代码实现

* 设置render，视频渲染区域
  
  ```cpp
      //判断渲染类型，给视频解码器设置视频播放器
      if(videoRenderType == VIDEO_RENDER_OPENGL) {
          m_VideoDecoder->SetVideoRender(VideoGLRender::GetInstance());
      } else if (videoRenderType == VIDEO_RENDER_ANWINDOW) {
          m_VideoRender = new NativeRender(jniEnv, surface);
          m_VideoDecoder->SetVideoRender(m_VideoRender);
      } else if (videoRenderType == VIDEO_RENDER_3D_VR) {
          m_VideoDecoder->SetVideoRender(VRGLRender::GetInstance());
      }
  ```

* 初始化解码线程

```cpp
    do {
        //1.创建封装格式上下文
        m_AVFormatContext = avformat_alloc_context();

        //2.打开文件
        error = avformat_open_input(&m_AVFormatContext, m_Url, NULL, NULL) ;
        if (error != 0) {
            ALOGE("avformat_open_input fail. error=%d",error);
            break;
        }

        //3.获取音视频流信息
        if (avformat_find_stream_info(m_AVFormatContext, NULL) < 0) {
            ALOGE("avformat_find_stream_info fail.");
            break;
        }

        //4.获取音视频流索引
        for (int i = 0; i < m_AVFormatContext->nb_streams; i++) {
            if (m_AVFormatContext->streams[i]->codecpar->codec_type == m_MediaType) {
                m_StreamIndex = i;
                break;
            }
        }

        if (m_StreamIndex == -1) {
            ALOGE("Fail to find stream index.");
            break;
        }
        //5.获取解码器参数
        AVCodecParameters *codecParameters = m_AVFormatContext->streams[m_StreamIndex]->codecpar;

        //6.获取解码器
        m_AVCodec = avcodec_find_decoder(codecParameters->codec_id);
        if (m_AVCodec == nullptr) {
            ALOGE("avcodec_find_decoder fail.");
            break;
        }

        //7.创建解码器上下文
        m_AVCodecContext = avcodec_alloc_context3(m_AVCodec);
        if (avcodec_parameters_to_context(m_AVCodecContext, codecParameters) != 0) {
            ALOGE(" avcodec_parameters_to_context fail.");
            break;
        }
        AVDictionary *pAVDictionary = nullptr;
        av_dict_set(&pAVDictionary, "buffer_size", "1024000", 0);
        av_dict_set(&pAVDictionary, "stimeout", "20000000", 0);
        av_dict_set(&pAVDictionary, "max_delay", "30000000", 0);
        av_dict_set(&pAVDictionary, "rtsp_transport", "tcp", 0);

        //8.打开解码器
        result = avcodec_open2(m_AVCodecContext, m_AVCodec, &pAVDictionary);
        if (result < 0) {
            ALOGE("avcodec_open2 fail. result=%d", result);
            break;
        }
        result = 0;

        m_Duration = m_AVFormatContext->duration / AV_TIME_BASE * 1000;//us to ms
        //创建 AVPacket 存放编码数据
        m_Packet = av_packet_alloc();
        //创建 AVFrame 存放解码后的数据
        m_Frame = av_frame_alloc();
    } while (false);

```
