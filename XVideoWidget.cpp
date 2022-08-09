#include "XVideoWidget.h"
#include "XUnpackage.h"

// 0-openGl,1-paintEvent
#define PAINT_TYPE 1
#define VIDEO_FORMAT QImage::Format_ARGB32
extern "C" {
//    #include "libavutil/frame.h"
    #include "libavutil/imgutils.h"

#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
//    #include "libavutil/avutil.h"
}



#if PAINT_TYPE == 0

#define GET_STR(x) #x
#define A_VER 3
#define T_VER 4
//����shader
const char *vString = GET_STR(
    attribute vec4 vertexIn;
attribute vec2 textureIn;
varying vec2 textureOut;
void main(void)
{
    gl_Position = vertexIn;
    textureOut = textureIn;
}
);


//ƬԪshader
const char *tString = GET_STR(
    varying vec2 textureOut;
uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;
void main(void)
{
    vec3 yuv;
    vec3 rgb;
    yuv.x = texture2D(tex_y, textureOut).r;
    yuv.y = texture2D(tex_u, textureOut).r - 0.5;
    yuv.z = texture2D(tex_v, textureOut).r - 0.5;
    rgb = mat3(1.0, 1.0, 1.0,
        0.0, -0.39465, 2.03211,
        1.13983, -0.58060, 0.0) * yuv;
    gl_FragColor = vec4(rgb, 1.0);
}

);

#endif
#if PAINT_TYPE == 1
static QImage* PaintImage;

//void filterColor(QImage *img){
//    QImageHandler handler(img);
//    for (int i = 0; i < colorTasks.size(); i++) {
//        switch (colorTasks[i].type)
//        {
//        case COLRTASK_GRAY_TO_RGBA://灰度图转RGB图
//            handler.gray2RGB();
//            break;
//        case COLRTASK_RGBA_TO_GRAY://RGB图转灰度图
//            handler.rgb2Gray();
//            break;
//        default:
//            break;
//        }
//    }
//    return handler.getHandleImage();
//}
#endif




XVideoWidget::XVideoWidget(QWidget *p): QOpenGLWidget(p) {
//    XVideoWidget::instance = this;
    XVideoWidget::getInstance(this);
}
XVideoWidget::~XVideoWidget() {}

XVideoWidget* XVideoWidget::getInstance(XVideoWidget *obj){
    static XVideoWidget* instance;
    if(obj!=NULL){

        instance = obj;

    }
    return instance;
}

void XVideoWidget::Init(int width, int height){

    mux.lock();
    this->width = width;
    this->height = height;
#if PAINT_TYPE == 0
    delete datas[0];
    delete datas[1];
    delete datas[2];
    ///���������ڴ��ռ�
    datas[0] = new unsigned char[width*height];		//Y
    datas[1] = new unsigned char[width*height / 4];	//U
    datas[2] = new unsigned char[width*height / 4];	//V


    if (texs[0])
    {
        glDeleteTextures(3, texs);
    }
    //��������
    glGenTextures(3, texs);

    //Y
    glBindTexture(GL_TEXTURE_2D, texs[0]);
    //�Ŵ����ˣ����Բ�ֵ   GL_NEAREST(Ч�ʸߣ�������������)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //���������Կ��ռ�
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

    //U
    glBindTexture(GL_TEXTURE_2D, texs[1]);
    //�Ŵ����ˣ����Բ�ֵ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //���������Կ��ռ�
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

    //V
    glBindTexture(GL_TEXTURE_2D, texs[2]);
    //�Ŵ����ˣ����Բ�ֵ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //���������Կ��ռ�
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);



#endif
    mux.unlock();

//#if PAINT_TYPE == 1
//    if(PaintImage)
//        delete PaintImage;
////        PaintImage = new QImage("/home/king/workspace/opencv/resource/1.png");


//    uchar *buf = new uchar[width*height * 4];
//    PaintImage = new QImage(buf, width, height, QImage::Format_ARGB32);
//#endif
}

void XVideoWidget::Repaint(AVFrame *frame){
    if (!frame)return;
#if PAINT_TYPE == 0
    mux.lock();
    if (!datas[0] || width*height == 0 || frame->width != this->width || frame->height != this->height)
    {
        qDebug() <<  frame->width << ";" <<frame->height;
        av_frame_free(&frame);
        mux.unlock();
        return;
    }

    memcpy(datas[0], frame->data[0], width*height);
    memcpy(datas[1], frame->data[1], width*height/4);
    memcpy(datas[2], frame->data[2], width*height/4);
    //�ж�������
    mux.unlock();
    av_frame_free(&frame);
    //ˢ����ʾ
    update();
    qDebug() << "hisdf";

#endif

#if PAINT_TYPE == 1
    if(PaintImage)
        delete PaintImage;
    PaintImage = new QImage(frame->width, frame->height, QImage::Format_RGB888);
//    QImage img (pFrame->width, pFrame->height, QImage::Format_RGB888);

    uint8_t* dst[] = { PaintImage->bits() };
    int dstStride[4];
    // AV_PIX_FMT_RGB24 对应于 QImage::Format_RGB888
    av_image_fill_linesizes(dstStride, AV_PIX_FMT_RGB24, frame->width);

    SwsContext *vctx = NULL;
    vctx = sws_getCachedContext(
                            vctx,
                            frame->width, // 源图像的宽度
                            frame->height, //  源图像的高度
                            (AVPixelFormat)frame->format,
                            width,
                            height,
                            AV_PIX_FMT_RGB24,
                            SWS_FAST_BILINEAR,
                            0, 0, 0
                            );
    // 转换
    sws_scale(vctx, frame->data, (const int*)frame->linesize,
        0, frame->height, dst, dstStride);
    qDebug() << "repaint";
//    repaint();
    update();
#endif
}


void XVideoWidget::initializeGL()
{
#if PAINT_TYPE == 0
    qDebug() << "initializeGL";
    mux.lock();
    //��ʼ��opengl ��QOpenGLFunctions�̳У�����
    initializeOpenGLFunctions();

    //program����shader��������ƬԪ���ű�
    //ƬԪ�����أ�
    qDebug() << program.addShaderFromSourceCode(QGLShader::Fragment, tString);
    //����shader
    qDebug() << program.addShaderFromSourceCode(QGLShader::Vertex, vString);

    //���ö��������ı���
    program.bindAttributeLocation("vertexIn", A_VER);

    //���ò�������
    program.bindAttributeLocation("textureIn", T_VER);

    //����shader
    qDebug() << "program.link() = " << program.link();

    qDebug() << "program.bind() = " << program.bind();

    //���ݶ����Ͳ�������
    //����
    static const GLfloat ver[] = {
        -1.0f,-1.0f,
        1.0f,-1.0f,
        -1.0f, 1.0f,
        1.0f,1.0f
    };

    //����
    static const GLfloat tex[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f
    };

    //����
    glVertexAttribPointer(A_VER, 2, GL_FLOAT, 0, 0, ver);
    glEnableVertexAttribArray(A_VER);

    //����
    glVertexAttribPointer(T_VER, 2, GL_FLOAT, 0, 0, tex);
    glEnableVertexAttribArray(T_VER);


    //��shader��ȡ����
    unis[0] = program.uniformLocation("tex_y");
    unis[1] = program.uniformLocation("tex_u");
    unis[2] = program.uniformLocation("tex_v");

    mux.unlock();
#endif
}

void XVideoWidget::paintGL()
{
#if PAINT_TYPE == 0
    mux.lock();
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, texs[0]); //0���󶨵�Y����
//                                           //�޸Ĳ�������(�����ڴ�����)
//    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, datas[0]);
//    //��shader uni��������
//    glUniform1i(unis[0], 0);


//    glActiveTexture(GL_TEXTURE0 + 1);
//    glBindTexture(GL_TEXTURE_2D, texs[1]); //1���󶨵�U����
//                                           //�޸Ĳ�������(�����ڴ�����)
//    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_RED, GL_UNSIGNED_BYTE, datas[1]);
//    //��shader uni��������
//    glUniform1i(unis[1], 1);


//    glActiveTexture(GL_TEXTURE0 + 2);
//    glBindTexture(GL_TEXTURE_2D, texs[2]); //2���󶨵�V����
//                                           //�޸Ĳ�������(�����ڴ�����)
//    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_RED, GL_UNSIGNED_BYTE, datas[2]);
//    //��shader uni��������
//    glUniform1i(unis[2], 2);

//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


//    qDebug() << "paintGL";

//    QImage img("/home/king/workspace/opencv/resource/1.png");
//    img.scaled(320, 240);

//    QPainter painter;
//    painter.begin(this);
//    painter.drawImage(QPoint(0, 0), img);
//    painter.end();
    mux.unlock();

#endif
}

void XVideoWidget::resizeGL(int width, int height)
{
}

void XVideoWidget::paintEvent(QPaintEvent *e){
    qDebug() << "paintGL123";

    if(PaintImage){
        QPainter painter;
        painter.begin(this);
        painter.drawImage(rect(), *PaintImage);
        painter.end();
    }
}
//-----------------------------------------------------------------

XVideoDevice::XVideoDevice()
{

}

XVideoDevice::~XVideoDevice(){

}

XVideoDevice* XVideoDevice::getInstance(){
    return XVideoWidget::getInstance();
}

