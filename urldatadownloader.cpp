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


#include "urldatadownloader.h"

#include <QThread>
#include <unistd.h>
#include <QSemaphore>



UrlDataDownloader::UrlDataDownloader(QUrl dataUrl, QObject *parent) : QObject(parent)
{
    noErros();

    qDebug() << __FUNCTION__ << " Downloading: " << dataUrl.toString() << endl;

    QNetworkRequest request(dataUrl);


    QNetworkReply* pReply = m_WebCtrl.get(request);

    if (pReply->error() != QNetworkReply::NoError)
    {
        setErrors(ERR_UrlD_NET_REQUEST, pReply->errorString());

        return;
    }

    if (pReply->isRunning() == true)
           QThread::msleep(50);

     connect(pReply, &QNetworkReply::finished, [=]() {

        if (pReply->error() != QNetworkReply::NoError)
        {

            qWarning() << __FUNCTION__ <<" ERR: " << pReply->errorString() << "Request: " << pReply->url().toString() << endl;

            setErrors(ERR_UrlD_NET_REQUEST, pReply->errorString());

            pReply->close();

            emit downloaded();

            return;
        }

        m_DownloadedData.clear();

        m_DownloadedData = pReply->readAll();

        //emit a signal
        emit downloaded();
    });

}


UrlDataDownloader::~UrlDataDownloader()
{
    //qDebug() << "Deletando " << __FUNCTION__ << "..." << endl;
}



QByteArray UrlDataDownloader::getDownloadedData() const
{
    return m_DownloadedData;
}

