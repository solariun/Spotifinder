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


#include "spotifyuserinfo.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

SpotifyUserInfo::SpotifyUserInfo(QObject *parent) : QObject(parent)
{}


bool SpotifyUserInfo::set (const QJsonDocument& document)
{
    if (document.isNull() || document.isEmpty())
    {
        return false;
    }
    const QJsonObject root = document.object();

    strUserName     = root.value("id").toString();
    strCountry      = root.value("country").toString();
    strDisplayName  = root.value("display_name").toString();
    strType         = root.value("type").toString();
    strProduct      = root.value("product").toString();
    strPictureUrl   = root.value("images").toArray()[0].toObject().value("url").toString();
    strSpotifyUri   = root.value("uri").toString();

   return true;
}


bool SpotifyUserInfo::set (const SpotifyUserInfo& spotifyUserInfo)
{
    strUserName     = spotifyUserInfo.strUserName;
    strDisplayName  = spotifyUserInfo.strDisplayName;
    strType         = spotifyUserInfo.strType;
    strProduct      = spotifyUserInfo.strProduct;
    strPictureUrl   = spotifyUserInfo.strPictureUrl;
    strSpotifyUri   = spotifyUserInfo.strSpotifyUri;

    return true;
}

const QString SpotifyUserInfo::getUserName()
{
    return strUserName;
}

const QString SpotifyUserInfo::getDisplayName()
{
    return strDisplayName;
}

const QString SpotifyUserInfo::getType()
{
    return strType;
}

const QString SpotifyUserInfo::getProduct()
{
    return strProduct;
}

const QString SpotifyUserInfo::getPictureUrl()
{
    return strPictureUrl;
}

const QString SpotifyUserInfo::getSpotifyUri()
{
    return strSpotifyUri;
}

const QString SpotifyUserInfo::getCountry()
{
    return strCountry;
}
