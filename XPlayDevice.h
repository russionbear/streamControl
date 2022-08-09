#ifndef XPLAYDEVICE_H
#define XPLAYDEVICE_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <mutex>
struct AVFrame;

class XVideoDevice
{
public:
    XVideoDevice();
    ~XVideoDevice();
    static XVideoDevice* getInstance();

    virtual void Init(int width, int height)=0;
    /**
     * @brief Repaint
     * @param frame, promise free frame no matter suc or failed
     */
    virtual void Repaint(AVFrame *frame)=0;

};

class XAudioDevice
{
public:
    XAudioDevice();
    ~XAudioDevice();
    static XAudioDevice *GetInstance();

    int sampleRate = 44100;
    int sampleSize = 16;
    int channels = 2;

    virtual bool Open() = 0;
    virtual void Close() = 0;

    virtual bool Write(const unsigned char *data, int datasize) = 0;
    virtual int GetFree() = 0;

};


#endif // XPLAYDEVICE_H
