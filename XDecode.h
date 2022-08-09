#ifndef XDECODE_H
#define XDECODE_H

struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
#include <mutex>


/**
 * @brief 
 * should call Rev after Send in a suitble time, or the decode may not run
 */
class XDecode
{
public:	
    bool isAudio = false;
    int threadCount = 8;

	virtual bool Open(AVCodecParameters *para);
	virtual bool Send(AVPacket *pkt);
	virtual AVFrame* Recv();
	virtual void Close();
	virtual void Clear();

	XDecode();
	virtual ~XDecode();

protected:
	AVCodecContext *codecCtx = NULL;
	std::mutex mux;
};

#endif // XDECODE_H
