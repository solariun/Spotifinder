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


#include "modelspotifysearch.h"
#include "urldatadownloader.h"

#include <QUrl>
#include <QList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QNetworkReply>
#include <QByteArray>


static uint nActiveDownloads = 0;

static uint nDataCount=0;

ModelSpotifySearch::ModelSpotifySearch(QObject *parent) : QObject(parent)
{
    noErros();

    qDebug() << "Deletando " << __FUNCTION__ << endl;
}


ModelSpotifySearch::~ModelSpotifySearch()
{
    qDebug() << __FUNCTION__ <<" Releasing object." << endl;
}

void ModelSpotifySearch::getSearchResultsList(const QJsonDocument &jsonDocument, QList<MusicDataItem>& listSearchResults)
{
    noErros();

    QJsonObject root = jsonDocument.object().value("tracks").toObject();

    listSearchResults.clear();

    uint nWidth;

    foreach (QJsonValue valueItem, root.value("items").toArray())
    {
        qDebug() << "Starting creating Search Item:"  << valueItem.toObject().value("name").toString() <<  endl;

        if (valueItem.toObject().value("preview_url").toString().length() == 0)
        {
            qDebug() << "No sample for id: " << valueItem.toObject().value("id").toString() << endl;
            continue;
        }
        MusicDataItem pSearchResultItem;

        qDebug() << __FUNCTION__ << " Created id: " << pSearchResultItem.nid << endl;

        pSearchResultItem.strMusic = valueItem.toObject().value("name").toString();
        qDebug() << "   Music         : " << pSearchResultItem.strMusic << endl;

        pSearchResultItem.strArtist = valueItem.toObject().value("artists")[0].toObject().value("name").toString();
        qDebug() << "   Artist        : " << pSearchResultItem.strArtist << endl;

        pSearchResultItem.strAlbum = valueItem.toObject().value("album").toObject().value("name").toString();
        qDebug() << "   Album         : " << pSearchResultItem.strAlbum << endl;

        pSearchResultItem.strReleaseDate = valueItem.toObject().value("album").toObject().value("release_date").toString();
        qDebug() << "   Release Date  : " << pSearchResultItem.strReleaseDate << endl;

        pSearchResultItem.strSongDuration = QDateTime::fromTime_t(valueItem.toObject().value("duration_ms").toInt()).toUTC().toString("hh:mm:ss");;
        qDebug() << "   Song Duration : " << pSearchResultItem.strSongDuration << endl;

        pSearchResultItem.strSampleMp3 =  valueItem.toObject().value("preview_url").toString();
        qDebug() << "   Sample MP3    : " << pSearchResultItem.strSampleMp3 << endl;

        pSearchResultItem.strID = valueItem.toObject().value("id").toString();
        qDebug() << "   ID            : " << pSearchResultItem.strID << endl;

        foreach(QJsonValue jsonArtItem, valueItem.toObject().value("album").toObject().value("images").toArray())
        {
            qDebug() << "   dynamic search: found: " << jsonArtItem.toObject().value("width").toInt() << endl;

            nWidth = jsonArtItem.toObject().value("width").toInt();

            if (nWidth < 100)
            {
                pSearchResultItem.strArtImagem = jsonArtItem.toObject().value("url").toString();
                qDebug() << "   Art Pic 64px : " << pSearchResultItem.strArtImagem << endl;
            }
        }

        listSearchResults.push_back(pSearchResultItem);
    }
}


ModelSpotifySearch::ModelSpotifySearch(QOAuth2AuthorizationCodeFlow* spotify, QPlainTextEdit* teOutput) :
    spotify(spotify), teOutput(teOutput)
{ }


bool ModelSpotifySearch::search(QString strCountry, uint nOffset, uint nMaxElements, QString strQuery)
{
   noErros();

   teOutput->appendPlainText("Start searchin sequence...");

   QUrl urlSearch("https://api.spotify.com/v1/search");

   urlSearch.setQuery("q="+ strQuery +"&type=track&market="+ strCountry +"&limit="+ QString::number(nMaxElements) + "&offset=" + QString::number(nOffset));


   auto reply = spotify->get(urlSearch);


   if (reply->error() != QNetworkReply::NoError)
   {
       teOutput->appendPlainText(reply->errorString());

       setErrors(ERR_MSPY_NET_REQUEST, reply->errorString());

       return false;
   }

   nActiveDownloads++;

   connect (reply, &QNetworkReply::finished, [=](){

       listResultItems.clear();

       if (reply->error() != QNetworkReply::NoError)
       {
           teOutput->appendPlainText(reply->errorString());

           setErrors(ERR_MSPY_NET_REQUEST, reply->errorString());

           reply->close();

           emit processed (listResultItems);
           return;
       }

        QByteArray dataReturned = reply->readAll();

        teOutput->appendPlainText("Search results: ");
        teOutput->appendPlainText(dataReturned);

        const QJsonDocument jsonDocument = QJsonDocument::fromJson(dataReturned);

        getSearchResultsList(jsonDocument, listResultItems);

        qDebug() << __FUNCTION__ << " Returned: " << listResultItems.size() << "Elements." << endl;

        nActiveDownloads--;

        reply->close();

        emit processed (listResultItems);

   });

   qDebug() << "Assync search streated..." << endl;

   return true;
}

uint ModelSpotifySearch::getActiveDownloads()
{
    return nActiveDownloads;
}



MusicDataItem::~MusicDataItem()
{
   // qDebug() << "***** Deleting " << __FUNCTION__ << "ID: " << nid << endl;
}

MusicDataItem::MusicDataItem()
{
    this->nid=++nDataCount;
}

MusicDataItem::MusicDataItem(const MusicDataItem &musicDataItem) :
    strMusic (musicDataItem.strMusic),
    strArtist (musicDataItem.strArtist),
    strAlbum (musicDataItem.strAlbum),
    strReleaseDate (musicDataItem.strReleaseDate),
    strSongDuration (musicDataItem.strSongDuration),
    strSampleMp3 (musicDataItem.strSampleMp3),
    strArtImagem (musicDataItem.strArtImagem),
    byteaPicture (musicDataItem.byteaPicture),
    strID (musicDataItem.strID)
{
    this->nid=++nDataCount;
}
