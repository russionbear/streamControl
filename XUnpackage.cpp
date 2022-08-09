#include "XUnpackage.h"
#include <iostream>

extern "C"
{
#include <libavformat/avformat.h>
}

using namespace std;

static double r2d(AVRational r)
{
	return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}


XUnpackage::XUnpackage()
{
    static bool isFirst = true;
    static std::mutex dmux;
    dmux.lock();
    if (isFirst)
    {
        // av_register_all(); // dropped in new version

        avformat_network_init();
        isFirst = false;
    }
    dmux.unlock();
    XUnpackage::getInstance(this);
}

XUnpackage::~XUnpackage()
{
}

XUnpackage* XUnpackage::getInstance(XUnpackage *upk){
    static XUnpackage *instance = NULL;
    if(upk!=NULL)
        instance = upk;
    return upk;
}


/**
 * @brief input stream may has not video or audio
 *
 * @param url
 * @return true
 * @return false
 */
bool XUnpackage::Open(const char *url)
{
    Close();

    // parameters for stream

    AVDictionary *opts = NULL;
    av_dict_set(&opts, "rtsp_transport", "tcp", 0);
    av_dict_set(&opts, "max_delay", "500", 0);

    // open stream

    int ret = avformat_open_input(
        &afCtx,
        url,
        NULL, // auto
        &opts);

    if (ret != 0)
    {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf) - 1);
        cout << "open " << url << " failed! :" << buf << endl;
        return false;
    }

    // get steam info and stream's duration

    ret = avformat_find_stream_info(afCtx, NULL);
    if (ret < 0)
    {
        cout << "find stream info error" << endl;
        return false;
    }

    totalMs = afCtx->duration / (AV_TIME_BASE / 1000);

    // find indexs of video and audio, and get its parameters

    videoStreamIndex = av_find_best_stream(
        afCtx,
        AVMEDIA_TYPE_VIDEO,
        -1, -1, NULL, NULL);

    if (videoStreamIndex != AVERROR_STREAM_NOT_FOUND)
    {
        VideoPar = avcodec_parameters_alloc();
        avcodec_parameters_copy(VideoPar, afCtx->streams[videoStreamIndex]->codecpar);
    }

    audioStreamIndex = av_find_best_stream(
        afCtx,
        AVMEDIA_TYPE_AUDIO,
        -1, -1, NULL, NULL);

    if (audioStreamIndex != AVERROR_STREAM_NOT_FOUND)
    {
        AudioPar = avcodec_parameters_alloc();
        avcodec_parameters_copy(AudioPar, afCtx->streams[audioStreamIndex]->codecpar);
    }

    cout << "videoStreamIndex:" << videoStreamIndex <<"\taudioStreamIndex:"<<audioStreamIndex<<endl;
    cout << "totalMs:" << totalMs << endl;

    av_dump_format(afCtx, 0, url, 0);
//    av_dump_format(afCtx, audioStreamIndex, NULL)
    return true;
}

void XUnpackage::Close()
{
    mux.lock();
    if (afCtx)
        avformat_close_input(&afCtx);
    if (VideoPar)
        avcodec_parameters_free(&VideoPar);
    if (AudioPar)
        avcodec_parameters_free(&AudioPar);
    mux.unlock();
}

/**
 * @brief remember to release/free
 * 
 * @return AVPacket* 
 */
AVPacket *XUnpackage::Read()
{
    mux.lock();
    if (!afCtx)
    {
        mux.unlock();
        return 0;
    }
    AVPacket *pkt = av_packet_alloc();

    int re = av_read_frame(afCtx, pkt);
    if (re != 0)
    {
        mux.unlock();
        av_packet_free(&pkt);
        return 0;
    }

    // convert seq to time(ms)
    pkt->pts = pkt->pts * (1000 * (r2d(afCtx->streams[pkt->stream_index]->time_base)));
    pkt->dts = pkt->dts * (1000 * (r2d(afCtx->streams[pkt->stream_index]->time_base)));

    mux.unlock();
    // cout << pkt->pts << " " << flush;
    return pkt;
}

bool XUnpackage::SeekByDouble(double pos)
{
    mux.lock();
    if(afCtx){
        avformat_flush(afCtx);
        long long seekPos = 0;
        seekPos = afCtx->streams[videoStreamIndex]->duration * pos;
        int re = av_seek_frame(afCtx, videoStreamIndex, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
        mux.unlock();
        return re >= 0;
    }
    mux.unlock();
    return false;
}

bool XUnpackage::SeekByInt(long long pos)
{
    mux.lock();
    if(afCtx){
        avformat_flush(afCtx);
        int re = av_seek_frame(afCtx, videoStreamIndex, pos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
        
        mux.unlock();
        return re >= 0;
    }
    mux.unlock();
    return false;
}

bool XUnpackage::IsAudio(AVPacket *pkt)
{
    return pkt->stream_index == audioStreamIndex;
}
