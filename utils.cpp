/*
 * MIT License

Copyright (c) 2018 Luiz Gustavo de Campos

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include "utils.h"

#include <QObject>
#include <QImage>
#include <QPixmap>
#include <QUrl>
#include <QString>
#include <QBrush>
#include <QPainter>
#include <QRect>
#include <QSize>
#include <QMessageBox>
#include <QApplication>
#include <QByteArray>

#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>

QPixmap Utils::getCircleImageFromQPixmap (const QPixmap& image)
{
    QImage qImage (image.toImage());

    qImage = qImage.convertToFormat(QImage::Format_ARGB32);

    //Lets cut it squeare
    uint uiImageFactor = std::min (qImage.width(), qImage.height());

    QRect qRect ((qImage.width() - uiImageFactor)/2, (qImage.height() - uiImageFactor)/2, uiImageFactor, uiImageFactor);

    qImage = qImage.copy (qRect);

    QImage qOutImage (uiImageFactor, uiImageFactor, QImage::Format_ARGB32);
    qOutImage.fill(Qt::transparent);

    QBrush qBrush (qImage);

    QPainter qPainter;

    qPainter.begin(&qOutImage);
    qPainter.setBrush (qBrush);
    qPainter.setPen (Qt::NoPen);  // do not draw an outline
    qPainter.setRenderHint(QPainter::Antialiasing, true);
    qPainter.drawEllipse(0, 0, uiImageFactor, uiImageFactor);
    qPainter.end();

    return QPixmap::fromImage(qOutImage);
}



void Utils::Assert(bool boolSentence, QString strTitle, QString strText)
{
    if (!boolSentence)
    {
        QMessageBox::critical(0, strTitle, strText);
        QApplication::quit();
    }
}

const QString Utils::Error::getLastError()
{
    return _strErrorMessage;
}


int Utils::safe_usleep(useconds_t useconds)
{
    struct timespec ts;

    ts.tv_sec = (long int)useconds / 1000000;
    ts.tv_nsec = ((long int)useconds % 1000000) * 1000;
    if (!nanosleep(&ts,&ts)) return 0;
    if (errno == EINTR) return ts.tv_sec;
    return -1;
}


uint Utils::Error::getLastErrorCode()
{
    return _intErrorCode;
}

void Utils::Error::noErros()
{
    _strErrorMessage.clear();
    _intErrorCode=0;
}

void Utils::Error::setErrors(int nError, const QString &strError)
{
    _intErrorCode = nError;
    _strErrorMessage = strError;
}

