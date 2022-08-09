#ifndef XVIDEODEVICE_H
#define XVIDEODEVICE_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <mutex>

#include "XPlayDevice.h"


struct AVFrame;
class XVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions,public XVideoDevice
{
    Q_OBJECT
public:
    static XVideoWidget* getInstance(XVideoWidget* obj=NULL);
    XVideoWidget(QWidget *p);
    ~XVideoWidget();

    void Init(int width, int height);
    void Repaint(AVFrame *frame);

    void paintEvent(QPaintEvent *e) override;

protected:
    std::mutex mux;
    QGLShaderProgram program;
    GLuint unis[3] = { 0 };
    GLuint texs[3] = { 0 };
    unsigned char *datas[3] = { 0 };
    int width = 320;
    int height = 240;

    void initializeGL();

    void paintGL();
    void resizeGL(int width, int height);

};

#endif // XVIDEODEVICE_H
