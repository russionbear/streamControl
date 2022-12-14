#include "XPlayDevice.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <mutex>
#include "XParMng.h"

#include "QDebug"

class MAudioPlayer:public XAudioDevice{
    QAudioOutput *output = NULL;
	QIODevice *io = NULL;
    std::mutex mux;

    virtual bool Open()
	{
        Close();
		QAudioFormat fmt;
        fmt.setSampleRate(sampleRate);
        fmt.setSampleSize(sampleSize);
        fmt.setChannelCount(channels);
		fmt.setCodec("audio/pcm");
		fmt.setByteOrder(QAudioFormat::LittleEndian);
        fmt.setSampleType(QAudioFormat::SignedInt);//
		mux.lock();
		output = new QAudioOutput(fmt);
//        qDebug() << "output" << output;
		io = output->start();
		mux.unlock();
		if(io)
			return true;
		return false;
	}
    virtual void Close()
    {
        mux.lock();
        if (io)
        {
            io->close ();
            io = NULL;
        }
        if (output)
        {
            output->stop();
            delete output;
            output = 0;
        }
        mux.unlock();
    }

    virtual bool Write(const unsigned char *data, int datasize)
	{
		if (!data || datasize <= 0)return false;
		mux.lock();
		if (!output || !io)
		{
            qDebug() << "no io";
			mux.unlock();
			return false;
		}
		int size = io->write((char *)data, datasize);
        qDebug() << "write size:" <<size;
		mux.unlock();
		if (datasize != size)
			return false;
		return true;
	}

	virtual int GetFree()
	{
		mux.lock();
		if (!output)
        {
			mux.unlock();
			return 0;
		}
		int free = output->bytesFree();
		mux.unlock();
		return free;
	}
};

XAudioDevice *XAudioDevice::GetInstance()
{
	static MAudioPlayer play;
	return &play;
}

XAudioDevice::XAudioDevice()
{
}

XAudioDevice::~XAudioDevice()
{
}
