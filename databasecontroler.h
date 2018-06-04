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


#ifndef DATABASECONTROLER_H
#define DATABASECONTROLER_H

#include "utils.h"
#include "modelspotifysearch.h"

#include <QObject>
#include <QString>
#include <QSqlDatabase>


enum _db_errorcodes
{
    ERR_DB_DB_IS_CLOSED = 10,
    ERR_DB_PREP_SELECT,
    ERR_DB_EXEC_SELECT,
    ERR_DB_NO_SELECT_DATA,

    ERR_DB_PREP_UPDATE = 20,
    ERR_DB_EXEC_UPDATE,

    ERR_DB_PREP_INSERT = 30,
    ERR_DB_EXEC_INSERT,

    ERR_DB_DT_ALREADY_EXIST = 40,
    ERR_DB_NO_DATA_DELETED
};


class DatabaseControler;

class  DatabaseControler : public QObject, public Utils::Error
{

Q_OBJECT

private:

    QSqlDatabase sqlDatabase;

    void prepareMusicDataItem (MusicDataItem& musicDataItem);

public:

    DatabaseControler();

    ~DatabaseControler();

    bool getConfigValue(const QString& strValue, QString& strReturn);

    bool setConfigValue(const QString& strKey, QString& strValue);

    bool addToPlaylist (const MusicDataItem& searchResultItem);

    bool getMusicDataItemFromPlaylist(QString strID, MusicDataItem& musicDataItemRet);

    bool getPlaylist(QList<MusicDataItem>& listMusics);

    bool deleteMusicDataItemFromPlaylist(QString strID);

};


#endif // DATABASECONTROLER_H
