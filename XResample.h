#ifndef XRESAMPLE_H
#define XRESAMPLE_H

struct AVCodecParameters;
struct AVFrame;
struct SwrContext;
#include <mutex>

/**
 * @brief resample for audio
 * remember to set public atr
 */
class XResample
{
public:
	XResample();
	~XResample();

    virtual bool Open(AVCodecParameters *para);
	virtual void Close();

	virtual int Resample(AVFrame *indata, unsigned char *data);


    int outChannels = 2;
    int outRate; // drop tmply
	//AV_SAMPLE_FMT_S16
	int outFormat = 1;
protected:
	std::mutex mux;
	SwrContext *actx = 0;
};

#endif // XRESAMPLE_H
