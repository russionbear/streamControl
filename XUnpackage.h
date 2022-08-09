#ifndef XUNPACKAGE_H
#define XUNPACKAGE_H

#include <mutex>
struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;


/**
 * method "Open" makes the program more flexible
 * 
 */


/**
 * @brief 
 * 
 */
class XUnpackage
{
public:
    XUnpackage();
    ~XUnpackage();

    static XUnpackage* getInstance(XUnpackage* upk=NULL);
    virtual bool Open(const char *url);
    virtual void Close();

    virtual AVPacket *Read();

    virtual bool SeekByDouble(double pos);
    virtual bool SeekByInt(long long pos);



    virtual bool IsAudio(AVPacket *pkt);
    AVCodecParameters *VideoPar = NULL;
    AVCodecParameters *AudioPar = NULL;

    /**
     * @brief an atr for input stream
     * 
     */
	int totalMs = 0;
//	int width = 0;
//	int height = 0;
//	int sampleRate = 0;
//	int channels = 0;
protected:
    std::mutex mux;

    AVFormatContext *afCtx = NULL;

    int videoStreamIndex = -1;
    int audioStreamIndex = -1;
};



#endif // XUNPACKAGE_H
