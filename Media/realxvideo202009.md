这里用来暂时记录最近realx视频需求测试所面临的一些知识点。

webrtc的关键帧策略：https://blog.jianchihu.net/webrtc-research-keyframe-request.html

webrtc名词库：https://webrtcglossary.com/

看起来 PLI 比 FIR 更为 “常见”（FIR是更严格重置的场景）。那究竟什么时候会触发PLI，什么时候会触发FIR呢

关键帧请求场景

Jitter buffer 是什么

h264标准中的视频编码长期参考帧(LTR)：https://zhuanlan.zhihu.com/p/103542912


ice 建联的过程还挺复杂的，从首帧优化开发计划有一些ref链接可以看一下
https://bytedance.feishu.cn/docs/doccnU7fECzW8ARaSfQUBQMqs0g

sdp（多端之间的协商控制）

后端的RTC技术分享：https://bytedance.feishu.cn/docs/doccnfr1cruE5ZIY3FL777a5JqJ?fromFolderUrl=https%253A%252F%252Fbytedance.feishu.cn%2523%2523Fj2zileqP2ZfU331

## SP相关

如下图，当新用户订阅时，最新的帧是[13], 服务器会下发最近的I帧，最近的 SP帧[8]， SP 帧到当前帧之间的帧[9-13]；如果是原来的方案虽然帧数量变多了，但其实只下发了一个I帧和若干SP，可以证明前者的数据量会更少吗？

通过SVC也可以降低首帧时需要传输的帧数量，问题是在首帧到达以后是不是就默认关闭SVC？这部分会不会因为耦合出现问题

reference finder是什么？应该就是确认当前解码器是否对某一帧可解码吧

