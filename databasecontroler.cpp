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


#include "databasecontroler.h"
#include "utils.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QSqlQuery>



void DatabaseControler::prepareMusicDataItem(MusicDataItem &musicDataItem)
{
    musicDataItem.strID.clear();
    musicDataItem.strMusic.clear();
    musicDataItem.strAlbum .clear();
    musicDataItem.strArtist.clear();
    musicDataItem.strReleaseDate.clear();
    musicDataItem.strSongDuration.clear();

    musicDataItem.strSampleMp3 .clear();
    musicDataItem.strArtImagem.clear();
}



DatabaseControler::DatabaseControler()
{
    noErros();

    if (sqlDatabase.isOpen())
        return;

    QString strPath = QCoreApplication::applicationDirPath() + "/spotifycli.sqlite";

    qDebug() << "Instatiating DatabaseControler" << endl;
    qDebug() << "Dabasefile: [" << strPath << "]" << endl;


    sqlDatabase = QSqlDatabase::addDatabase("QSQLITE");

    sqlDatabase.setDatabaseName(strPath);

    Utils::Assert (sqlDatabase.open() == true, "Database", "Error, opening database: " + sqlDatabase.lastError().text());

    QStringList strlTables = sqlDatabase.tables();


    qDebug () << "TAbles: " << strlTables << endl;

    if (strlTables.indexOf("config") == -1)
    {
        QSqlQuery sqlQuery (sqlDatabase);

        qDebug() << "ATTENTION: Creating config table" << endl;

        //Creating config database
        Utils::Assert(sqlQuery.exec ("CREATE TABLE config (key TEXT UNIQUE PRIMARY KEY, value TEXT)"),
                      "DataBase Creating", "Error Creating Table: " + sqlQuery.lastError().text());

        //Creating Index
        Utils::Assert(sqlQuery.exec ("CREATE INDEX config_idx on config (key)"),
                      "DataBase Creating", "Error Creating Table Index: " + sqlQuery.lastError().text());
    }

    if (strlTables.indexOf("playlist") == -1)
    {
        qDebug() << "ATTENTION: Creating playlist     table" << endl;

        QSqlQuery sqlQuery (sqlDatabase);

        //Creating config database
        Utils::Assert(sqlQuery.exec ("CREATE TABLE playlist (ID UNIQUE PRIMARY KEY, music TEXT, artist TEXT, album TEXT, albumart, samplemp3 TEXT, releasedate TEXT, songduration TEXT)"),
                      "DataBase Creating", "Error Creating Table: " + sqlQuery.lastError().text());

        //Creating Index
        Utils::Assert(sqlQuery.exec ("CREATE INDEX playlist_idx ON playlist (ID)"),
                      "DataBase Creating", "Error Creating Table Index: " + sqlQuery.lastError().text());
    }

}


DatabaseControler::~DatabaseControler()
{
    if (sqlDatabase.isOpen() == true)
    {
        qDebug() << "Closing database..." << endl;
        sqlDatabase.close();
    }
}



bool DatabaseControler::getConfigValue(const QString& strKey, QString& strReturn)
{
    noErros();

    VERIFY(sqlDatabase.isOpen() == true, ERR_DB_DB_IS_CLOSED, tr("Database is closed."), false);

    QSqlQuery sqlQuery (sqlDatabase);

    VERIFY (sqlQuery.prepare("SELECT value FROM config WHERE key = :value") == true,
            ERR_DB_PREP_SELECT, tr("Error, selecting database config: ") + sqlQuery.lastError().text(), false);

    sqlQuery.bindValue (":value", strKey);

    VERIFY (sqlQuery.exec(), ERR_DB_EXEC_SELECT, tr("Error, executing: ") + sqlQuery.lastError().text(), false);

    VERIFY (sqlQuery.next() == true, ERR_DB_NO_SELECT_DATA, tr("Error, no data available") + sqlQuery.lastError().text(), false);

    strReturn = sqlQuery.value(0).toString();

    return true;
}




bool DatabaseControler::setConfigValue(const QString &strKey, QString &strValue)
{
    noErros();

    VERIFY(sqlDatabase.isOpen() == true, ERR_DB_DB_IS_CLOSED, tr("Database is closed."), false);

    QString strReturn;

    QSqlQuery sqlQuery (sqlDatabase);

    if (getConfigValue (strKey, strReturn) == true)
    {
        VERIFY (sqlQuery.prepare("UPDATE config SET Key= :key, Value= :value") == true,
                ERR_DB_PREP_UPDATE, tr("Error, selecting database config: ") + sqlQuery.lastError().text(), false);

        qDebug() << __FUNCTION__ <<": data set to be UPDATED" << endl;
    }
    else
    {
        VERIFY (sqlQuery.prepare("INSERT INTO config (key, value) VALUES (:key, :value)") == true,
                ERR_DB_PREP_INSERT, tr("Error, selecting database config: ") + sqlQuery.lastError().text(), false);

        qDebug() << __FUNCTION__ <<": data set to be INSERTED" << endl;
    }

    sqlQuery.bindValue (":key", strKey);
    sqlQuery.bindValue (":value", strValue);

    VERIFY (sqlQuery.exec(), ERR_DB_EXEC_UPDATE, tr("Error, executing: ") + sqlQuery.lastError().text(), false);

    qDebug() << "-> Affected data: " << sqlQuery.numRowsAffected() << endl;

    return true;
}



bool DatabaseControler::addToPlaylist(const MusicDataItem &searchResultItem)
{
    noErros();

    VERIFY(sqlDatabase.isOpen() == true, ERR_DB_DB_IS_CLOSED, tr("Database is closed."), false);

    QString strReturn;

    QSqlQuery sqlQuery (sqlDatabase);

    {
        MusicDataItem tmpMusicDataItem;

        VERIFY (getMusicDataItemFromPlaylist (searchResultItem.strID, tmpMusicDataItem) == false,
                ERR_DB_DT_ALREADY_EXIST, tr("Error, data already existe: ") + getLastError(), false);
    }

    VERIFY (sqlQuery.prepare("INSERT INTO playlist (ID, music, artist, album, albumart, samplemp3, releasedate, songduration) "
                             "VALUES (:id, :music, :artist, :album, :albumart, :samplemp3, :releasedate, :songduration)") == true,
            ERR_DB_PREP_INSERT, tr("Error, selecting database config: ") + sqlQuery.lastError().text(), false);

    qDebug() << __FUNCTION__ <<": data set to be INSERTED" << endl;

    sqlQuery.bindValue (":id", searchResultItem.strID);
    sqlQuery.bindValue (":music", searchResultItem.strMusic);
    sqlQuery.bindValue (":artist", searchResultItem.strArtist);
    sqlQuery.bindValue (":album", searchResultItem.strAlbum);
    sqlQuery.bindValue (":albumart", searchResultItem.byteaPicture);
    sqlQuery.bindValue (":samplemp3", searchResultItem.strSampleMp3);
    sqlQuery.bindValue (":songduration", searchResultItem.strSongDuration);

    VERIFY (sqlQuery.exec(), ERR_DB_EXEC_UPDATE, tr("Error, executing: ") + sqlQuery.lastError().text(), false);

    qDebug() << "-> Affected data: " << sqlQuery.numRowsAffected() << endl;

    return true;
}



bool DatabaseControler::deleteMusicDataItemFromPlaylist(QString strID)
{
    noErros();

    VERIFY(sqlDatabase.isOpen() == true, ERR_DB_DB_IS_CLOSED, tr("Database is closed."), false);

    QSqlQuery sqlQuery (sqlDatabase);

    VERIFY (sqlQuery.prepare("DELETE FROM playlist WHERE ID = :id") == true,
            ERR_DB_PREP_SELECT, tr("Error, selecting database config: ") + sqlQuery.lastError().text(), false);

    sqlQuery.bindValue (":id", strID);

    VERIFY (sqlQuery.exec(), ERR_DB_EXEC_SELECT, tr("Error, executing: ") + sqlQuery.lastError().text(), false);

    VERIFY (sqlQuery.numRowsAffected() > 0, ERR_DB_NO_DATA_DELETED, tr("Error, no data deleted") + sqlQuery.lastError().text(), false);

    return true;
}



bool DatabaseControler::getMusicDataItemFromPlaylist(QString strID, MusicDataItem& musicDataItem)
{
    noErros();

    prepareMusicDataItem(musicDataItem);

    VERIFY(sqlDatabase.isOpen() == true, ERR_DB_DB_IS_CLOSED, tr("Database is closed."), false);

    QSqlQuery sqlQuery (sqlDatabase);

    VERIFY (sqlQuery.prepare("SELECT * FROM playlist WHERE ID = :id") == true,
            ERR_DB_PREP_SELECT, tr("Error, selecting database config: ") + sqlQuery.lastError().text(), false);

    sqlQuery.bindValue (":id", strID);

    VERIFY (sqlQuery.exec(), ERR_DB_EXEC_SELECT, tr("Error, executing: ") + sqlQuery.lastError().text(), false);

    VERIFY (sqlQuery.next() == true, ERR_DB_NO_SELECT_DATA, tr("Error, no data available") + sqlQuery.lastError().text(), false);


    musicDataItem.strID = sqlQuery.value("ID").toString();
    musicDataItem.strMusic = sqlQuery.value("music").toString();
    musicDataItem.strAlbum = sqlQuery.value("album").toString();
    musicDataItem.strArtist = sqlQuery.value("artist").toString();
    musicDataItem.strReleaseDate = sqlQuery.value("releasedate").toString();
    musicDataItem.strSongDuration = sqlQuery.value("songduration").toString();
    musicDataItem.strSampleMp3 = sqlQuery.value("samplemp3").toString();
    musicDataItem.byteaPicture = sqlQuery.value("albumart").toByteArray();

    return true;
}

bool DatabaseControler::getPlaylist(QList<MusicDataItem> &listMusics)
{
    noErros();

    VERIFY(sqlDatabase.isOpen() == true, ERR_DB_DB_IS_CLOSED, tr("Database is closed."), false);

    QSqlQuery sqlQuery (sqlDatabase);

    VERIFY (sqlQuery.prepare("SELECT * FROM playlist ORDER BY music") == true,
            ERR_DB_PREP_SELECT, tr("Error, selecting database config: ") + sqlQuery.lastError().text(), false);

    VERIFY (sqlQuery.exec(), ERR_DB_EXEC_SELECT, tr("Error, executing: ") + sqlQuery.lastError().text(), false);

    listMusics.clear();

    while (sqlQuery.next() == true)
    {
        MusicDataItem musicDataItem;

        musicDataItem.strID = sqlQuery.value("ID").toString();
        musicDataItem.strMusic = sqlQuery.value("music").toString();
        musicDataItem.strAlbum = sqlQuery.value("album").toString();
        musicDataItem.strArtist = sqlQuery.value("artist").toString();
        musicDataItem.strReleaseDate = sqlQuery.value("releasedate").toString();
        musicDataItem.strSongDuration = sqlQuery.value("songduration").toString();
        musicDataItem.strSampleMp3 = sqlQuery.value("samplemp3").toString();
        musicDataItem.byteaPicture = sqlQuery.value("albumart").toByteArray();

        listMusics.append(musicDataItem);
    }

    return true;
}






