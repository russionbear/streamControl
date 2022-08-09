#ifndef XPARMNG_H
#define XPARMNG_H


struct AudioStruct{
    int format;
    int sampleSize;
    int sampleRate;
    int channels;
};

struct VideoStruct{
    int width;
    int height;
};

class XParMng{
public:
    AudioStruct audio;
    VideoStruct video;

    static XParMng* getInstance(){
        static XParMng instance;
        return &instance;
    }
};

#endif // XPARMNG_H
