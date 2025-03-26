# bluethooth discont have noise

玩愤怒的小鸟断开蓝牙有杂音

# 日志分析

日志看有近1.2s的时间没有音频数据，如果音频数据不足进行播放则会有破音或者杂音，请音频部分确认对应策略是否可以进行优化。
```
03-22 17:56:46.371864   658 25749 W AF::TrackBase: TrackBase(716): uid 10143 tried to pass itself off as 1041
03-22 17:56:46.372190   658 25749 D AF::Track: Track, track(0xb40000765c243740): mFastIndex -1, mStreamType 3, mId 716, mFrameCount 36000, mSampleRate 48000, mFormat 1, mChannelCount 2, mTrackCount 661, thread 0xb4000077ecce4920, sessionId 0, AudioOut_96D, r_submix-0
03-22 17:56:46.380126   658   880 W APM_AudioPolicyManager: releaseOutput() no output for client 1333
03-22 17:56:46.443630   658  9813 D MtkAudioResamplerDyn: MtkAudioResamplerDyn InBuffer::reset!!
03-22 17:56:46.468238   658   975 D AF::PatchTrack: releaseBuffer(713) Not enough data, wait for buffer to fill
03-22 17:56:46.640291   658  9813 D MtkAudioResamplerDyn: MtkAudioResamplerDyn InBuffer::reset!!
03-22 17:56:46.766890   658   975 D AF::PatchTrack: releaseBuffer(713) Not enough data, wait for buffer to fill
03-22 17:56:46.944310   658  9813 D MtkAudioResamplerDyn: MtkAudioResamplerDyn InBuffer::reset!!
03-22 17:56:47.067536   658   975 D AF::PatchTrack: releaseBuffer(713) Not enough data, wait for buffer to fill
03-22 17:56:47.244113   658  9813 D MtkAudioResamplerDyn: MtkAudioResamplerDyn InBuffer::reset!!
03-22 17:56:47.365556   658   975 D AF::PatchTrack: releaseBuffer(713) Not enough data, wait for buffer to fill
03-22 17:56:47.547429   658  9813 D MtkAudioResamplerDyn: MtkAudioResamplerDyn InBuffer::reset!!
03-22 17:56:47.571618   658 24871 W AudioFlinger_Threads: createTrack_l(): mismatch between requested flags (00000104) and output flags (00000006)
03-22 17:56:47.572925   658 24871 D AudioFlinger_Threads: Client defaulted notificationFrames to 960 for frameCount 4100BTAudioHalStream: adev_close_output_stream
```