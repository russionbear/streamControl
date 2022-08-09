#include "mainwindow.h"
#include "XUnpackage.h"
#include "XDecode.h"
#include "XResample.h"
#include "XPlayDevice.h"
#include "XParMng.h"

#include <QApplication>
#include <QtDebug>
#include <QThread>


void testUnpackage(){
    XUnpackage unpackage;
    unpackage.Open("/home/king/workspace/opencv/resource/1.map4");

    XDecode vdecode;
    XDecode adecode;
    XAudioDevice* audioDevice = XAudioDevice::GetInstance();
    XVideoDevice* videoDevice = XVideoDevice::getInstance();

    vdecode.Open(unpackage.VideoPar);
    adecode.Open(unpackage.AudioPar);

    XResample aresample;
    aresample.Open(unpackage.AudioPar);

    qDebug() << "audio device open:" << audioDevice->Open();

    int64_t count = 0;
//    unpackage.SeekByDouble(0.9);
    while (true) {
        AVPacket *pkt = unpackage.Read();
        AVFrame *frame;
        unsigned char *pcm = new unsigned char[1024 * 1024 * 10];

        if(!pkt)
            break;
        if(unpackage.IsAudio(pkt)){
            qDebug() << "a" <<  adecode.Send(pkt);
            frame = adecode.Recv();
//            qDebug() << "a rev" << frame;

            if (!frame) continue;
            //�ز���
            int size = aresample.Resample(frame, pcm);
            //������Ƶ
            qDebug() << "sizes9d0f: "<<size<<"," <<audioDevice->GetFree();

            while (true)
            {
                if (size <= 0)break;
                if (audioDevice->GetFree() < size)
                {
                    qDebug() << "sdfsdf";
                    QThread::msleep(1);
                    continue;
                }
                qDebug() << "write result" <<audioDevice->Write(pcm, size);
                break;
            }
            delete pcm;

        }else{
            qDebug() << "v" << vdecode.Send(pkt);
            vdecode.Recv();
//            while(true){
//                frame = vdecode.Recv();
//                QThread::msleep(1);
//                if(frame){
//                    break;
//                }
//            }
        }
        count ++;
    }

//    qDebug() << count;
//    unpackage.Open("/home/king/workspace/opencv/resource/1.map4");
//    unpackage.SeekByDouble(0.9);
//    while (true) {
//        AVPacket *pkt = unpackage.Read();
//        if(!pkt)
//            break;
//        count ++;
//    }
//    qDebug() << count;

}

int main(int argc, char *argv[])
{


    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    testUnpackage();
//    qDebug() << XVideoDevice::getInstance();
    return a.exec();
}
