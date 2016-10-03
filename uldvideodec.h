#ifndef ULDVIDEODEC_H
#define ULDVIDEODEC_H
#include <QVideoFrame>
#include <QVideoSurfaceFormat>
typedef void(*yuvdecoderfunc)(const uchar *, uchar *, int, int, int);
void qt_convert_NV21_to_ARGB32(const uchar * pinframe, uchar * poutframe, int width, int height, int flags);
void qt_convert_BGR32_to_ARGB32(const uchar * pinframe, uchar * poutframe, int width, int height, int flags);    
void debugVideoFrameInfo(QVideoFrame  * fp, const QVideoSurfaceFormat &sf );


#define ULD_VIDEODEC_NONE 0x00000000
#define ULD_VIDEODEC_NV12 0x00000001
#define ULD_VIDEODEC_NV21 0x00000000


class uldvideodec
{
    typedef struct {
        yuvdecoderfunc func;
        int flag;
    }decdescriptor; 
    
private:
    
    QMap<int,decdescriptor>m_yuvdecodedriver;
    
public:
    uldvideodec();
    QImage toARGB32(QVideoFrame *fp);
};

#endif // ULDVIDEODEC_H
