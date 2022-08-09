#include "XDecode.h"

extern "C"
{
#include "libavcodec/avcodec.h"
}

#include <iostream>
#include <QDebug>

using namespace std;


XDecode::XDecode()
{
}

XDecode::~XDecode()
{
}

bool XDecode::Open(AVCodecParameters *para)
{
    if (!para)
        return false;

    Close();

    // get codec
    AVCodec *codec = avcodec_find_decoder(para->codec_id);
    if (!codec)
    {
        cout << "can't find the codec id " << para->codec_id << endl;
        return false;
    }

    mux.lock();

    // get codecCtx
    codecCtx = avcodec_alloc_context3(codec);
    if (!codecCtx)
    {
        mux.unlock();
        cout << "get codecCtx failed" << endl;
        return false;
    }

    // set paramters
    avcodec_parameters_to_context(codecCtx, para);
    codecCtx->thread_count = threadCount;

    // init codecCtx
    int re = avcodec_open2(
        codecCtx,
        0, 0 // don't know
    );
    if (re != 0)
    {
        avcodec_free_context(&codecCtx);
        mux.unlock();
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) - 1);
        cout << "avcodec_open2  failed! :" << buf << endl;
        return false;
    }
    mux.unlock();
    return true;
}

/**
 * @brief will free pkt no matter suc or failed
 * 
 * @param pkt 
 * @return true 
 * @return false 
 */
bool XDecode::Send(AVPacket *pkt)
{
    if (!pkt || pkt->size <= 0 || !pkt->data)return false;
	mux.lock();
	if (!codecCtx)
	{
		mux.unlock();
		return false;
	}
	int re = avcodec_send_packet(codecCtx, pkt);
	mux.unlock();
	av_packet_free(&pkt);
//    cout << "send"<<endl;
	if (re != 0)return false;
	return true;
}

AVFrame *XDecode::Recv()
{
    mux.lock();
	if (!codecCtx)
	{
		mux.unlock();
		return NULL;
	}
	AVFrame *frame = av_frame_alloc();
	int re = avcodec_receive_frame(codecCtx, frame);
	mux.unlock();
	if (re != 0)
	{
		av_frame_free(&frame);
		return NULL;
	}
     qDebug() << "["<<frame->linesize[0] << "] ";
	return frame;
}


void XDecode::Close()
{
    mux.lock();
	if (codecCtx)
	{
		avcodec_close(codecCtx);
		avcodec_free_context(&codecCtx);
	}
	mux.unlock();
}
void XDecode::Clear()
{
    mux.lock();
	if (codecCtx)
	{
		avcodec_free_context(&codecCtx);
	}
	mux.unlock();
}
