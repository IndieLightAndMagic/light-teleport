#include "uldhelper.h"
#include <QDebug>
#include <QDateTime>
#include <QBuffer>
UldHelper::UldHelper()
{
    
}

QByteArray UldHelper::formatMessage(char * buff){
    
    
    
    QString qsbuff = QString(buff)+QDateTime::currentDateTime().toString();
    int bsz =  qsbuff.size();
    int * pbsz = &bsz;
    quint16 zero = 0;
    char * prebuff  = reinterpret_cast<char*>(pbsz);
    
    char swap = prebuff[1];
    
    prebuff[1] = prebuff[0];
    prebuff[0] = swap;
    
    
    
    QByteArray ba(reinterpret_cast<const char*>(&zero),2);
    ba+=QByteArray(prebuff,2);
    ba += qsbuff;
    qDebug()<<ba<<ba.size();
    return ba;
    
}
void UldHelper::serializeQImageAndSend(QAbstractSocket * as,QImage &img){
    
    /* Put some metainformation in the PNG */
    QDateTime dt = QDateTime::currentDateTimeUtc();
    QString pngFileName("_"+dt.toString("yyyyMMdd_HHmmss_zzz_t")+".png");
    qDebug()<<"FileName is:"<<pngFileName;
    img.setText("Filename",pngFileName);
    
    /* Save a PNG file on RAM and upload it */
    QByteArray pngChunk;
    QBuffer pngChunkBuffer(&pngChunk);
    pngChunkBuffer.open(QIODevice::WriteOnly);
    img.save(&pngChunkBuffer,"PNG");
    qint64 bWritten = as->write(pngChunk);
    qInfo()<<"Buffered PNG Size:"<<pngChunk.size();
    img.save(pngFileName,"PNG");
    
    
    
    /* This did not work */
    qInfo()<<"Bytes Written:"<<bWritten;
    qInfo()<<as->waitForBytesWritten(pngChunk.size());
    
    
}
