#include "uldvideodec.h"
#include "uldvideodec_data.h"
#include <QDebug>
#define MAKE_RGB(r, g, b) 0xff000000 | clampedR[r] | clampedG[g] | clampedB[b]

void qt_convert_BGR32_to_ARGB32(const uchar *pinframe, uchar *poutframe, int width, int height, int flags){
    Q_UNUSED(flags);
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j ++, pinframe +=4, poutframe +=4){
            poutframe[0] = pinframe[3];
            poutframe[1] = pinframe[2];
            poutframe[2] = pinframe[1];
            poutframe[3] = pinframe[0]; 
        }
    }
}

void qt_convert_NV21_to_ARGB32(const uchar * pinframe, uchar * poutframe, int width, int height, int flags){
    
    const uchar *y0 = pinframe;
    const uchar *y1 = pinframe + width;
    const uchar *vu = pinframe + width * height;

    quint32 *rgb0 = reinterpret_cast<quint32*>(poutframe);
    quint32 *rgb1 = rgb0 + width;

    for (int i = 0; i < height; i += 2) {
        for (int j = 0; j < width; j += 2) {
            
            
            int v = flags & ULD_VIDEODEC_NV21 ? vu[0] : vu[1];
            int u = flags & ULD_VIDEODEC_NV21 ? vu[1] : vu[0];
            vu++;vu++;
            
            int ruv = coefficientsRV[v] >> 15;
            int guv = (coefficientsGU[u] + coefficientsGV[v]) >> 15;
            int buv = coefficientsBU[u] >> 15;

            int y = coefficientsY[*y0++] >> 15;
            int r = y + ruv;
            int g = y + guv;
            int b = y + buv;
            *rgb0++ = MAKE_RGB(r, g, b);

            y = coefficientsY[*y0++] >> 15;
            r = y + ruv;
            g = y + guv;
            b = y + buv;
            *rgb0++ = MAKE_RGB(r, g, b);

            y = coefficientsY[*y1++] >> 15;
            r = y + ruv;
            g = y + guv;
            b = y + buv;
            *rgb1++ = MAKE_RGB(r, g, b);

            y = coefficientsY[*y1++] >> 15;
            r = y + ruv;
            g = y + guv;
            b = y + buv;
            *rgb1++ = MAKE_RGB(r, g, b);
        }

        rgb0 += width;
        rgb1 += width;
        y0 += width;
        y1 += width;
    }    
}

uldvideodec::uldvideodec(){
    
    qDebug()<<"Setting up decoding drivers";
    
    m_yuvdecodedriver[QVideoFrame::Format_BGR32] = {qt_convert_BGR32_to_ARGB32, ULD_VIDEODEC_NONE};
    m_yuvdecodedriver[QVideoFrame::Format_NV12] =  {qt_convert_NV21_to_ARGB32, ULD_VIDEODEC_NV12};
    m_yuvdecodedriver[QVideoFrame::Format_NV21] =  {qt_convert_NV21_to_ARGB32, ULD_VIDEODEC_NV21};
    
}

QImage uldvideodec::toARGB32(QVideoFrame * fp){
    qDebug()<<"AMO A PAOLA";
    if(!fp->isValid()){
        qDebug()<<"Invalid QVideoFrame";
        return QImage();
    }
    
    int width = fp->width();
    int height = fp->height();
    
    /* Calculate size for 4 bytes per pixel */
    QByteArray rgbBuffer(width*height*4,0);
    
    /* Get YUV frame */
    const uchar * pyuv = fp->bits();
    
    /* rgbBufferPointer */
    uchar * prgb = reinterpret_cast<uchar*>(rgbBuffer.data()); 
    
    
    /* Decode */
    qDebug()<<"Video Decoding:"<<fp->pixelFormat();
    m_yuvdecodedriver[fp->pixelFormat()].func(pyuv, prgb, width, height, m_yuvdecodedriver[fp->pixelFormat()].flag);        
    
    
    
    /* Create QImage */
    /* Encode in Qimage */
    QImage img = QImage(prgb,width,height,QImage::Format_ARGB32);
    
    return img;
    
}
void debugVideoFrameInfo(QVideoFrame  * fp, const QVideoSurfaceFormat &sf ){
    
    if (fp->isMapped()==false) {
        qDebug()<<"Frame is not mapped.... silently dropped";
        return;
    }
    qDebug()<<"SURFACE FORMAT";
    qDebug()<<"--------------";
    qDebug()<<"Frame Width:"<<sf.frameWidth();
    qDebug()<<"Frame Height:"<<sf.frameHeight();
    qDebug()<<"Video Handle Type:"<<sf.handleType();
    qDebug()<<"Pixel Aspect Ratio:"<<sf.pixelAspectRatio();
    qDebug()<<"Pixel Format:"<<sf.pixelFormat();
    qDebug()<<"Color Space:"<<sf.yCbCrColorSpace();
    qDebug()<<"Properties:"<<sf.propertyNames();
    qDebug()<<"Scan Line Direction:"<<sf.scanLineDirection();
    qDebug()<<"Viewport Size:"<<sf.sizeHint();
    qDebug()<<"Viewport Rect:"<<sf.viewport();
    
    qDebug()<<"VIDEO FRAME INFO";
    qDebug()<<"----------------";
    qDebug()<<"MetaData:"<<fp->availableMetaData();
    qDebug()<<"Mapped Bytes:"<<fp->mappedBytes();
    qDebug()<<"Planes:"<<fp->planeCount();
    qDebug()<<"Size:"<<fp->size();
    qDebug()<<"StartTime:"<<fp->startTime();
    qDebug()<<"Bytes Per Line Plane 0 :"<<fp->bytesPerLine(0);
    qDebug()<<"Bytes Per Line Plane 1 :"<<fp->bytesPerLine(1);
    qDebug()<<"Address 0:"<<reinterpret_cast<qint64>(fp->bits(0));
    qDebug()<<"Address 1:"<<reinterpret_cast<qint64>(fp->bits(1));
    
    
    
    
}
