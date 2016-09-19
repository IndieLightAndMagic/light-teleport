#include "uldhelper.h"
#include <QDebug>
#include <QDateTime>
#include <QBuffer>
UldHelper::UldHelper()
{
    
}

QByteArray UldHelper::swapEndianness(int i){
    
    unsigned char * pi = reinterpret_cast<unsigned char *>(&i);
    unsigned char swapper = pi[3];
    pi[3] = pi[0];
    pi[0] = swapper;
    
    swapper = pi [2];
    pi[2] = pi[1];
    pi[1] = swapper;
    
    return QByteArray(reinterpret_cast<const char*>(pi),4);
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

void UldHelper::serializeQImageAndSend(QAbstractSocket * as,
                                       QImage &img){
    
    img = img.scaled(204,204);
    
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
    int pngChunkSize = pngChunk.size();
    
    
    
    qInfo()<<"Buffered PNG Size:"<<pngChunk.size();
    pngChunk = swapEndianness(pngChunkSize)+ pngChunk;
    
    qint64 bWritten = as->write(pngChunk);
    //as->flush();
    
    /* This did not work */
    qInfo()<<"Bytes Written:"<<bWritten;
    qInfo()<<as->waitForBytesWritten(pngChunk.size());
    
    
}
