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


#ifndef URLDATADOWNLOADER_H
#define URLDATADOWNLOADER_H

#include "utils.h"

#define ERR_NET_REQUEST 100

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>


enum _ERR_UrlDownloder_Enum
{
    ERR_UrlD_NET_REQUEST = 200
};


class UrlDataDownloader : public QObject, public Utils::Error
{
 Q_OBJECT
 public:
    explicit UrlDataDownloader(QUrl imageUrl, QObject *parent = 0);
    virtual ~UrlDataDownloader();

    QByteArray getDownloadedData() const;

 signals:

  void downloaded();

 private slots:


 private:
    QNetworkAccessManager m_WebCtrl;
    QByteArray m_DownloadedData;
};

#endif // URLDATADOWNLOADER_H
