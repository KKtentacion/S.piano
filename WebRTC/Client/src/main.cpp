#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdio>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
}

#define OUTPUT_URL "rtp://your_server_ip:5004" // 服务器 RTP 推流地址

void stream_video(const std::string &input_file) {
    avformat_network_init(); // 初始化网络

    AVFormatContext *input_fmt_ctx = nullptr;
    AVCodecContext *input_codec_ctx = nullptr;
    AVFormatContext *output_fmt_ctx = nullptr;
    AVStream *video_stream = nullptr;

    // 1. 打开输入视频文件
    if (avformat_open_input(&input_fmt_ctx, input_file.c_str(), nullptr, nullptr) != 0) {
        std::cerr << "无法打开输入文件: " << input_file << std::endl;
        return;
    }

    // 2. 查找流信息
    if (avformat_find_stream_info(input_fmt_ctx, nullptr) < 0) {
        std::cerr << "无法找到流信息" << std::endl;
        return;
    }

    // 3. 查找视频流
    int video_stream_index = -1;
    for (unsigned int i = 0; i < input_fmt_ctx->nb_streams; i++) {
        if (input_fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }
    if (video_stream_index == -1) {
        std::cerr << "未找到视频流" << std::endl;
        return;
    }

    // 4. 获取输入流的解码器
    const AVCodec *input_codec = avcodec_find_decoder(input_fmt_ctx->streams[video_stream_index]->codecpar->codec_id);
    if (!input_codec) {
        std::cerr << "找不到解码器" << std::endl;
        return;
    }

    input_codec_ctx = avcodec_alloc_context3(input_codec);
    avcodec_parameters_to_context(input_codec_ctx, input_fmt_ctx->streams[video_stream_index]->codecpar);

    // 5. 打开解码器
    if (avcodec_open2(input_codec_ctx, input_codec, nullptr) < 0) {
        std::cerr << "无法打开解码器" << std::endl;
        return;
    }

    // 6. 初始化输出流
    avformat_alloc_output_context2(&output_fmt_ctx, nullptr, "rtp", OUTPUT_URL);
    if (!output_fmt_ctx) {
        std::cerr << "无法创建输出上下文" << std::endl;
        return;
    }

    // 7. 创建新的视频流
    video_stream = avformat_new_stream(output_fmt_ctx, nullptr);
    if (!video_stream) {
        std::cerr << "无法创建视频流" << std::endl;
        return;
    }

    avcodec_parameters_copy(video_stream->codecpar, input_fmt_ctx->streams[video_stream_index]->codecpar);
    video_stream->codecpar->codec_tag = 0;

    // 8. 打开输出 URL
    if (!(output_fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&output_fmt_ctx->pb, OUTPUT_URL, AVIO_FLAG_WRITE) < 0) {
            std::cerr << "无法打开输出文件" << std::endl;
            return;
        }
    }

    // 9. 写入流头信息
    if (avformat_write_header(output_fmt_ctx, nullptr) < 0) {
        std::cerr << "写入流头信息失败" << std::endl;
        return;
    }

    // 10. 读取帧并推流
    AVPacket pkt;
    while (av_read_frame(input_fmt_ctx, &pkt) >= 0) {
        if (pkt.stream_index == video_stream_index) {
            pkt.stream_index = video_stream->index;
            pkt.pts = av_rescale_q(pkt.pts, input_fmt_ctx->streams[video_stream_index]->time_base, video_stream->time_base);
            pkt.dts = av_rescale_q(pkt.dts, input_fmt_ctx->streams[video_stream_index]->time_base, video_stream->time_base);
            pkt.duration = av_rescale_q(pkt.duration, input_fmt_ctx->streams[video_stream_index]->time_base, video_stream->time_base);

            if (av_interleaved_write_frame(output_fmt_ctx, &pkt) < 0) {
                std::cerr << "写入帧失败" << std::endl;
                break;
            }
        }
        av_packet_unref(&pkt);
    }

    // 11. 结束推流
    av_write_trailer(output_fmt_ctx);
    avcodec_free_context(&input_codec_ctx);
    avformat_close_input(&input_fmt_ctx);
    avio_closep(&output_fmt_ctx->pb);
    avformat_free_context(output_fmt_ctx);

    std::cout << "视频推流完成" << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "使用方法: " << argv[0] << " <输入视频文件>" << std::endl;
        return -1;
    }

    std::string input_file = argv[1];
    stream_video(input_file);
    return 0;
}
