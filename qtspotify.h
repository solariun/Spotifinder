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


#ifndef QTSPOTIFY_H
#define QTSPOTIFY_H

#include "spotifyuserinfo.h"
#include "databasecontroler.h"
#include "modelspotifysearch.h"

#include <QMainWindow>

#include <QWidget>
#include <QUrl>
#include <QOAuth2AuthorizationCodeFlow>
#include <QList>
#include <QListWidgetItem>
#include <QMap>
#include <QMediaPlayer>
#include <QBuffer>
#include <QMediaPlaylist>

namespace Ui {
class QtSpotify;
}

class QtSpotify : public QMainWindow
{
    Q_OBJECT

public:
    explicit QtSpotify(QWidget *parent = nullptr);

    ~QtSpotify();

    bool setUserInfoData (const SpotifyUserInfo& spotifyUserInfo);

private slots:

    void granted ();
    void authStatusChanged(QAbstractOAuth::Status status);

    /* DEBUG */
    void on_pushButton_2_clicked();

    void on_SearchModel_returnPressed();

    void on_SearchResultsView_itemClicked(QListWidgetItem *item);

    void on_toolButton_2_clicked();

    void on_volumeSlider_sliderMoved(int position);

    void on_SearchResultsView_itemDoubleClicked(QListWidgetItem *item);

    void on_pushButton_clicked();

    void on_QtSpotify_destroyed();

    void on_pushButton_4_clicked();

    void on_playPlaylist_clicked();

    void on_buttonForward_clicked();

    void on_buttonBackward_clicked();

    void on_PlayListView_itemDoubleClicked(QListWidgetItem *item);

    void on_SearchButton_clicked();

private:
    void loadUserPictureFrom (QUrl objQUrl);
    void configAuthenticationProcess ();

    void requestUserInformaiton ();

    bool viewPopulateSearchResults(const QList<MusicDataItem>& listSearchResults);

    void populatePlaylist ();

    void addMusicItemToPlaylistView (const MusicDataItem &musicDataItem);

    void safePlayerStop ();

    DatabaseControler dbControler;

    Ui::QtSpotify *ui;

    SpotifyUserInfo spotifyUserInfo;

    QOAuth2AuthorizationCodeFlow spotify;

    bool isGranted;

    QMediaPlayer* objPlayer;
    QMediaPlaylist * objPlaylist;

    QList<MusicDataItem> listActivePlaylistMusic;

    QColor qcolorDeFaultItemBGColor;

    bool boolPlayingPlaylist;
};

#endif // QTSPOTIFY_H
