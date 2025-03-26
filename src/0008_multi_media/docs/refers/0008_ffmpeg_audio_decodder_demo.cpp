#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
 
// ...
 
AVCodecContext *codec_ctx = NULL;
AVCodec *codec = NULL;
AVFrame *frame = NULL;
AVPacket packet;
 
// 初始化解码器上下文
codec_ctx = ...; // 从输入文件中获取
 
// 打开解码器
codec = avcodec_find_decoder(codec_ctx->codec_id);
if (codec == NULL) {
    fprintf(stderr, "无法找到解码器\n");
    return;
}
if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
    fprintf(stderr, "无法打开解码器\n");
    return;
}
 
// 为解码帧分配内存
frame = av_frame_alloc();
 
// 解码循环
while (av_read_frame(fmt_ctx, &packet) >= 0) {
    if (packet.stream_index == audio_stream_index) {
        int got_frame = 0;
        if (avcodec_decode_audio4(codec_ctx, frame, &got_frame, &packet) < 0) {
            fprintf(stderr, "解码错误\n");
            break;
        }
        if (got_frame) {
            // 处理解码后的音频帧
            // ...
        }
    }
    av_packet_unref(&packet);
}
 
// 释放资源
av_frame_free(&frame);
avcodec_close(codec_ctx);
// ...
