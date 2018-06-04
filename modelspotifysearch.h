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


#ifndef MODELSPOTIFYSEARCH_H
#define MODELSPOTIFYSEARCH_H

#include "utils.h"

#include <QOAuth2AuthorizationCodeFlow>
#include <QObject>
#include <QPlainTextEdit>

#include <QString>
#include <QList>
#include <QJsonDocument>


enum _ModelSpotifyErrors
{
   ERR_MSPY_NET_REQUEST  = 10
};



class MusicDataItem
{
public:
    uint nid;

    ~MusicDataItem();
    MusicDataItem();
    MusicDataItem(const MusicDataItem& musicDataItem);


    QString strMusic;
    QString strArtist;
    QString strAlbum;
    QString strReleaseDate;

    QString strSongDuration;
    QString strSampleMp3;

    QString strArtImagem;

    QByteArray byteaPicture;

    QString strID;
};



class ModelSpotifySearch : public QObject, public Utils::Error
{
    Q_OBJECT
public:
    ModelSpotifySearch(QObject *parent = nullptr);

    ModelSpotifySearch(QOAuth2AuthorizationCodeFlow* spotify, QPlainTextEdit* teOutput);

    ~ModelSpotifySearch();

signals:

   void processed (QList<MusicDataItem>& );

public slots:


private:
    QList<MusicDataItem> listResultItems;

    QOAuth2AuthorizationCodeFlow* spotify;
    QPlainTextEdit* teOutput;

    void getSearchResultsList (const QJsonDocument& jsonDocument, QList<MusicDataItem>& listSearchResults);


public:
     bool search (QString strCountry, uint nOffset, uint nMaxElements, QString strQuery);

     uint getActiveDownloads();

};

#endif // MODELSPOTIFYSEARCH_H
