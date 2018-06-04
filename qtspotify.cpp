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


#include "qtspotify.h"
#include "ui_qtspotify.h"
#include "urldatadownloader.h"
#include "utils.h"
#include "modelspotifysearch.h"
#include "databasecontroler.h"
#include "labelitem.h"

#include <QBuffer>
#include <QMediaPlayer>
#include <QImage>
#include <QPixmap>
#include <QUrl>
#include <QString>
#include <QBrush>
#include <QPainter>
#include <QRect>
#include <QSize>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QSize>

#include <QDebug>
#include <QtNetworkAuth>
#include <QJsonDocument>

#include <QDesktopServices>
#include <QApplication>
#include <QCoreApplication>
#include <QMessageBox>
#include <QTextStream>
#include <QMetaMethod>

QtSpotify::QtSpotify(QWidget *parent) : QMainWindow(parent), ui(new Ui::QtSpotify)
{
   ui->setupUi(this);

    configAuthenticationProcess();

}

void QtSpotify::configAuthenticationProcess()
{
    objPlayer = new QMediaPlayer (this);
    objPlaylist = new QMediaPlaylist (objPlayer);

    objPlaylist->setPlaybackMode (QMediaPlaylist::Sequential);

    /* Default item backgroud color */
    qcolorDeFaultItemBGColor.setRgb(50,50,50);

    objPlayer->setPlaylist(objPlaylist);

    this->setAutoFillBackground(true);
    this->setWindowOpacity(0.9);

    connect (ui->chkDEbugView, &QCheckBox::toggled,[=](bool checked) {

        qDebug() << "Defining qtspotify window to " << (checked ? "Debug view" : "Normal") << endl;

        uint nHeight = checked ? 800 : 584;

        this->resize(QSize (this->width(), nHeight));
        this->setMinimumHeight(nHeight);
        this->setMaximumHeight(nHeight);

        this->setMinimumWidth(this->width());
        this->setMaximumWidth(this->width());

    } );


    //Force window to adapt to the rules.
    ui->chkDEbugView->setChecked(false);
    auto replyHandler = new QOAuthHttpServerReplyHandler(QHostAddress("127.0.0.1"), 8888, this);

    if (replyHandler->isListening() == false)
    {
        QMessageBox::StandardButton reply;

        reply = QMessageBox::critical(this, tr("QMessageBox::critical()"),
                                      tr("Error, application could not listern port ") + QString::number(replyHandler->port()),
                                      QMessageBox::Abort /*| QMessageBox::Retry | QMessageBox::Ignore*/);

        if (reply == QMessageBox::Abort)
            QCoreApplication::quit();
    }

    Q_ASSERT(replyHandler->isListening());

    replyHandler->setProperty("redirect_uri", "http://localhost:8888/");


    spotify.setReplyHandler(replyHandler);
    spotify.setAuthorizationUrl(QUrl("https://accounts.spotify.com/authorize"));
    spotify.setAccessTokenUrl(QUrl("https://accounts.spotify.com/api/token"));

    spotify.setClientIdentifier("a76554e9d65445508d25f67ca73a429c");
    spotify.setClientIdentifierSharedKey("b93e03fa930e4ba68112131241c5a00d");

    //spotify.setClientIdentifier("client_id");
    //spotify.setClientIdentifierSharedKey("client_secret");

    spotify.setScope("user-read-private user-top-read user-read-birthdate playlist-read-private playlist-modify-public playlist-modify-private user-read-recently-played");

    //oauth2.setScope("identity read");

    QString strToken;


    connect(&spotify, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
             &QDesktopServices::openUrl);

    connect(&spotify, &QOAuth2AuthorizationCodeFlow::statusChanged,
            this, &QtSpotify::authStatusChanged);

    connect(&spotify, &QOAuth2AuthorizationCodeFlow::granted,
            this, &QtSpotify::granted);


    if (dbControler.getConfigValue("Token", strToken) == false)
    {
        qWarning() << "Warning, no data returned: " << dbControler.getLastError() << endl;

        spotify.grant();
    }
    else
    {
        qDebug() << "Database: " << strToken << endl;

        ui->labelToken->setText(strToken);

        spotify.setToken(strToken);

        requestUserInformaiton ();
    }

    ui->teOutput->appendPlainText(replyHandler->callbackText());

    populatePlaylist();
}






void QtSpotify::requestUserInformaiton()
{
    ui->teOutput->appendPlainText("Loading User Informations");

    QUrl u ("https://api.spotify.com/v1/me");

    auto reply = spotify.get(u);

    connect(reply, &QNetworkReply::finished, [=]() {

        if (reply->error() != QNetworkReply::NoError)
        {
            ui->teOutput->appendPlainText(reply->errorString());

            spotify.grant();

            reply->close();

            return;
        }

        const auto data = reply->readAll();
        ui->teOutput->appendPlainText(data);

        const QJsonDocument document = QJsonDocument::fromJson(data);

        Utils::Assert (spotifyUserInfo.set (document) == true, "User Data", "No user data loaded.<H2>Attention</H2>");

        setUserInfoData(spotifyUserInfo);
    });
}



void QtSpotify::granted ()
{
    ui->teOutput->appendPlainText("Signal granted received");

    QString token = spotify.token();
    ui->teOutput->appendPlainText("Token: " + token);

    dbControler.setConfigValue("Token", token);

    isGranted = true;

    requestUserInformaiton();
}


void QtSpotify::authStatusChanged(QAbstractOAuth::Status status)
{
    QString s;
    if (status == QAbstractOAuth::Status::Granted)
        s = "granted";

    if (status == QAbstractOAuth::Status::TemporaryCredentialsReceived) {
        s = "temp credentials";
        //oauth2.refreshAccessToken();
    }

    ui->teOutput->appendPlainText("Auth Status changed: " + s +  "\n");
}



QtSpotify::~QtSpotify()
{

    delete ui;
}



bool QtSpotify::setUserInfoData(const SpotifyUserInfo &spotifyUserInfo)
{
  if (this->spotifyUserInfo.set(spotifyUserInfo) == false)
      return false;

  loadUserPictureFrom(QUrl(this->spotifyUserInfo.getPictureUrl()));

  ui->DisplayAccount->setText("<font color=gray>" + tr("Account:") + " </font><font color=white><b>" + this->spotifyUserInfo.getProduct() + "</b></font><font color=gray>  " + tr("Type:")+ " </font><font color=white><b>" + this->spotifyUserInfo.getType());
  ui->DisplayName->setText("<font color=white size=5><b>" \
                           + (this->spotifyUserInfo.getDisplayName().length() == 0 ? this->spotifyUserInfo.getUserName() : this->spotifyUserInfo.getDisplayName()) \
                           + "</h1>");

  return true;
}

void QtSpotify::loadUserPictureFrom (QUrl objQUrl)
{
    UrlDataDownloader* objImageCtrl = new UrlDataDownloader(objQUrl, this);

    connect (objImageCtrl, &UrlDataDownloader::downloaded, [=](){
        QPixmap pixImage;

        Utils::Assert (pixImage.loadFromData(objImageCtrl->getDownloadedData()) == true, "Load User Picture", "Error loading User Picture");

        pixImage = Utils::getCircleImageFromQPixmap(pixImage);

        //ui->UserPicture->resize(QSize(uiImageFactor, uiImageFactor));
        ui->UserPicture->setPixmap(pixImage);
    });
}


void QtSpotify::on_pushButton_2_clicked()
{
    spotify.grant();
}



void QtSpotify::on_SearchModel_returnPressed()
{
    qDebug() << "------------------------" << endl;
    qDebug() << "STARTING SEARCHING..." << endl;

    safePlayerStop();

    ModelSpotifySearch* spotifySearch = new ModelSpotifySearch (&spotify, ui->teOutput);

    connect (spotifySearch, &ModelSpotifySearch::processed, [=](QList<MusicDataItem>& listResultItems){

        if (spotifySearch->getLastErrorCode() != 0)
        {
            qWarning() << "Error, no data returned: " << spotifySearch->getLastError() << endl;
            return;
        }

        viewPopulateSearchResults(listResultItems);

        return;
    });

    ui->loadingBar->setValue(5);
    ui->loadingBar->setMaximum(50);

    spotifySearch->search(spotifyUserInfo.getCountry(), 1, ui->loadingBar->maximum(), ui->SearchModel->text());

    if (spotifySearch->getLastErrorCode() != 0)
    {
        qWarning() << "Error, no data returned: " << spotifySearch->getLastError() << endl;
        return;
    }
}


bool QtSpotify::viewPopulateSearchResults(const QList<MusicDataItem>& listSearchResults)
{
    ui->SearchResultsView->clear();

    ui->loadingBar->setMaximum(listSearchResults.size());
    ui->loadingBar->setValue(0);

    QIcon qIcon;
    qIcon.addFile(QStringLiteral (":/Images/play-32.ico"), QSize(), QIcon::Normal, QIcon::On);

    int nCount=0;

    foreach (const MusicDataItem& searchResultItem, listSearchResults)
    {
        qDebug() << "   -> Adding ID:" << searchResultItem.strID <<"; Music:" << searchResultItem.strMusic << "Image: " << searchResultItem.strArtImagem <<endl;

        LabelItem* pLabelItem = new LabelItem();

        pLabelItem->pObject = (void*) &searchResultItem;

        //Safe initialization and self distroction on site
        {

            qDebug() << __FUNCTION__ << " Downading: [" << searchResultItem.strArtImagem << "]" << endl;

            UrlDataDownloader* download = new UrlDataDownloader (QUrl(searchResultItem.strArtImagem));

            connect (download, &UrlDataDownloader::downloaded, [=](){

                ((MusicDataItem*) pLabelItem->pObject)->byteaPicture = download->getDownloadedData();

                ui->loadingBar->setValue(ui->loadingBar->value()+1);
                QPixmap pixSmallAlbum;
                pixSmallAlbum.loadFromData(((MusicDataItem*) pLabelItem->pObject)->byteaPicture);
                pLabelItem->setMainIcon(pixSmallAlbum);

                qDebug() << __FUNCTION__ <<" AlbumArt dowloaded: [" << ((MusicDataItem*) pLabelItem->pObject)->byteaPicture.size() << "]" << endl;
             });
        }


        pLabelItem->setMainLabel("<font size=2><b>" + QString::number(++nCount) + "<b> - Duration: <b>" + searchResultItem.strSongDuration + "</b> Artist: <b>" + searchResultItem.strArtist+ "</b><br></font><font size=2><b>" + searchResultItem.strMusic + "</b><br></font><font size=1>  Album: <b>" + searchResultItem.strAlbum + "/" + searchResultItem.strReleaseDate + "</b></font>");


        pLabelItem->setKey(searchResultItem.strID);


        pLabelItem->setStatusLabel(qIcon, QSize(10,10));

        //pLabelItem->setStatusLabel("" + QString::number(++nCount));

        pLabelItem->setBackgroundColor(QColor(50,50,50));
        pLabelItem->setDefaultFontColor(QColor(255,255,255));

        QListWidgetItem* Item = new QListWidgetItem();
        Item->setSizeHint(QSize(ui->SearchResultsView->width()-20, pLabelItem->height() + 1));

        ui->SearchResultsView->addItem(Item);
        ui->SearchResultsView->setItemWidget(Item, pLabelItem);

    }

    return true;
}



void QtSpotify::populatePlaylist()
{
    this->listActivePlaylistMusic.clear();

    QList<MusicDataItem> listActivePlaylistMusic;

    ui->PlayListView->clear();

    if (dbControler.getPlaylist(listActivePlaylistMusic))
    {
        foreach (const MusicDataItem& musicDataItem , listActivePlaylistMusic.toVector())
        {
            qDebug() << __FUNCTION__ << ": Adding: " << musicDataItem.strID <<" : " << musicDataItem.strMusic << ": Albumart: " << musicDataItem.byteaPicture.size() << endl;

            addMusicItemToPlaylistView (musicDataItem);
        }
    }

    ui->PlayListView->update();
    qApp->processEvents();
}



void QtSpotify::addMusicItemToPlaylistView(const MusicDataItem &musicDataItem)
{
    LabelItem* pLabelItem = new LabelItem();

    MusicDataItem* pMusicDataItem = new MusicDataItem (musicDataItem);

    listActivePlaylistMusic.append(*pMusicDataItem);

    pLabelItem->pObject = (void*) pMusicDataItem;

    qDebug() << __FUNCTION__ << "Safe assignment test: " << ((const MusicDataItem*)pLabelItem->pObject)->strMusic << endl;

    //Safe initialization and self distruction on site
    {
        QPixmap pixSmallAlbum;
        pixSmallAlbum.loadFromData(musicDataItem.byteaPicture);
        pLabelItem->setMainIcon(pixSmallAlbum);

        qDebug() << __FUNCTION__ <<" AlbumArt added: [" << ((MusicDataItem*) pLabelItem->pObject)->byteaPicture.size() << "]" << endl;
    }

    pLabelItem->setMainLabel("<font size=2> Duration: <b>" + musicDataItem.strSongDuration + "</b> Artist: <b>" + musicDataItem.strArtist+ "</b><br></font><font size=2><b>" + musicDataItem.strMusic + "</b><br></font><font size=1>  Album: <b>" + musicDataItem.strAlbum + "/" + musicDataItem.strReleaseDate + "</b></font>");

    pLabelItem->setKey(musicDataItem.strID);

    pLabelItem->setStatusLabel(" ");

    pLabelItem->setBackgroundColor(qcolorDeFaultItemBGColor);
    pLabelItem->setDefaultFontColor(QColor(255,255,255));

    QListWidgetItem* Item = new QListWidgetItem();
    Item->setSizeHint(QSize(ui->SearchResultsView->width()-20, pLabelItem->height() + 1));

    ui->PlayListView->addItem(Item);
    ui->PlayListView->setItemWidget(Item, pLabelItem);
}

void QtSpotify::safePlayerStop()
{
    objPlayer->stop();
    objPlaylist->clear();

    if (boolPlayingPlaylist == true)
    {
        objPlaylist->disconnect(objPlaylist, SLOT(QMediaPlaylist::currentIndexChanged(int)));

        populatePlaylist();

        boolPlayingPlaylist = false;
    }
}


void QtSpotify::on_SearchResultsView_itemClicked(QListWidgetItem *item)
{

    LabelItem* plabelIten =  (LabelItem*) item->listWidget()->itemWidget(item);

    const MusicDataItem& searchResultItem = *((MusicDataItem*)plabelIten->pObject);

    qDebug() << "Search Item Cliecked.... " << searchResultItem.strMusic << endl;

    {
        qDebug() << __FUNCTION__ << " playing: [" << searchResultItem.strArtImagem << "]" << endl;

        plabelIten->setStatusLabel("wait");

        safePlayerStop ();

        objPlaylist->addMedia(QUrl(searchResultItem.strSampleMp3));

        objPlayer->setVolume(ui->volumeSlider->value());
        objPlayer->play();

        plabelIten->setStatusLabel(":/Images/play-32.ico", QSize(10,10));

        qDebug() << "done...";
    }
}



void QtSpotify::on_toolButton_2_clicked()
{
    safePlayerStop();
}



void QtSpotify::on_volumeSlider_sliderMoved(int position)
{
    objPlayer->setVolume(position);
}



void QtSpotify::on_SearchResultsView_itemDoubleClicked(QListWidgetItem *item)
{
    safePlayerStop();

    LabelItem* pLabelItem =  (LabelItem*) item->listWidget()->itemWidget(item);

    const MusicDataItem& searchResultItem = *((MusicDataItem*) pLabelItem->pObject);

    qDebug() << "Search Item Cliecked.... " << searchResultItem.strMusic << endl;

    if (dbControler.addToPlaylist(searchResultItem) == false)
    {
        qDebug() << "Erro edding: " << dbControler.getLastError() << endl;

        QColor colorOriginal (pLabelItem->getBackgroundColor());

        pLabelItem->setBackgroundColor(QColor(78,8,12));
        pLabelItem->setStatusLabel(":/Images/x-mark-3-32.ico", QSize(20,20));
        pLabelItem->repaint();
        qApp->processEvents();

        QThread::msleep(400);

        pLabelItem->setBackgroundColor(colorOriginal);
        pLabelItem->setStatusLabel(":/Images/play-32.ico", QSize(10,10));
        pLabelItem->repaint();
    }
    else
    {
        qDebug() << "OK edding: " << endl;

        QColor colorOriginal (pLabelItem->getBackgroundColor());
        QIcon icon;

        pLabelItem->setBackgroundColor(QColor(6, 122, 11));
        pLabelItem->setStatusLabel(":/Images/approval-32.ico", QSize(20,20));
        pLabelItem->repaint();
        qApp->processEvents();

        QThread::msleep(400);

        pLabelItem->setBackgroundColor(colorOriginal);
        pLabelItem->setStatusLabel(":/Images/play-32.ico", QSize(10,10));
        pLabelItem->repaint();

        addMusicItemToPlaylistView (searchResultItem);
    }

}


void QtSpotify::on_pushButton_clicked()
{
    QApplication::quit();
}

void QtSpotify::on_QtSpotify_destroyed()
{
    qInfo() << "Closing APPLICATION" << endl;
}

void QtSpotify::on_pushButton_4_clicked()
{
    safePlayerStop();
    populatePlaylist();
}

void QtSpotify::on_playPlaylist_clicked()
{
    safePlayerStop();

    if (ui->PlayListView->count() == 0)
        return;

    static int nCurrentIdex;

    foreach (const MusicDataItem& musicDataItem, listActivePlaylistMusic)
    {
        qDebug() << __FUNCTION__ << " Adding to play in playlist: " << musicDataItem.strMusic << endl;

        objPlaylist->addMedia(QUrl(musicDataItem.strSampleMp3));
    }

    objPlayer->setVolume(ui->volumeSlider->value());
    objPlayer->play();

    boolPlayingPlaylist = true;

    nCurrentIdex = 0;

    {
        QListWidgetItem* pListWidgetItem;

        pListWidgetItem = ui->PlayListView->item(nCurrentIdex);
        LabelItem* pLabelItemNew = (LabelItem*) pListWidgetItem->listWidget()->itemWidget(pListWidgetItem);
        pLabelItemNew->setBackgroundColor(QColor(62, 92, 180));
        pLabelItemNew->setStatusLabel(":/Images/play-32.ico", QSize(10,10));

        pLabelItemNew->repaint();
        qApp->processEvents();
    }



    connect (objPlaylist, &QMediaPlaylist::currentIndexChanged, [=](int index) {

        qDebug() << __FUNCTION__ << " Datas: index: [" << index << "] nCurrent: [" << nCurrentIdex << "] Player stopped: "<< (objPlayer->state() == QMediaPlayer::StoppedState ? "STOPED" : "PLAYING") << endl;

        if (index == nCurrentIdex) return;

        LabelItem* pLabelItemOld = nullptr;
        LabelItem* pLabelItemNew = nullptr;
        QListWidgetItem* pListWidgetItem = nullptr;

        pListWidgetItem = ui->PlayListView->item(nCurrentIdex);
        pLabelItemOld = (LabelItem*) pListWidgetItem->listWidget()->itemWidget(pListWidgetItem);

        pLabelItemOld->setBackgroundColor(qcolorDeFaultItemBGColor);
        pLabelItemOld->setStatusLabel(" ");

        pLabelItemOld->repaint();

        if (index >= 0)
        {
            pListWidgetItem = ui->PlayListView->item(index);

            pLabelItemNew = (LabelItem*) pListWidgetItem->listWidget()->itemWidget(pListWidgetItem);
            pLabelItemNew->setStatusLabel(":/Images/play-32.ico", QSize(10,10));

            pLabelItemNew->setBackgroundColor(QColor(62, 92, 180));

            pListWidgetItem->listWidget()->setItemSelected(pListWidgetItem, true);

            pLabelItemNew->repaint();

            nCurrentIdex = index;
        }
        else
        {
            boolPlayingPlaylist = false;
        }


        qApp->processEvents();

    });

}

void QtSpotify::on_buttonForward_clicked()
{
     objPlaylist->next();
}

void QtSpotify::on_buttonBackward_clicked()
{
    objPlaylist->previous();
}

void QtSpotify::on_PlayListView_itemDoubleClicked(QListWidgetItem *item)
{
    if (boolPlayingPlaylist == true)
    {
        safePlayerStop();
        return;
    }

    LabelItem* pLabelItem = nullptr;

    pLabelItem = (LabelItem*) item->listWidget()->itemWidget(item);

    QColor colorHighLight(78, 8, 12);

    if (dbControler.deleteMusicDataItemFromPlaylist(pLabelItem->getKey()) == true)
    {
        int nCount=0;
        foreach (const MusicDataItem& musicDataItem, listActivePlaylistMusic)
        {
            if(musicDataItem.strID.compare(pLabelItem->getKey()) == 0)
            {
                qDebug() << __FUNCTION__ << " Deleting: " << listActivePlaylistMusic[nCount].strID << " | " << listActivePlaylistMusic[nCount].strMusic << endl;
                qDebug() << __FUNCTION__ << "    listActivePlaylistMusic size: " << listActivePlaylistMusic.size() << endl;

                listActivePlaylistMusic.removeAt(nCount);
            }

            nCount++;
        }


        for(int nCount=0; nCount < 2; nCount++)
        {
            pLabelItem->setBackgroundColor(colorHighLight);
            pLabelItem->setStatusLabel(":/Images/trash-32.ico", QSize(20,20));
            pLabelItem->repaint();
            qApp->processEvents();

            QThread::msleep(100);

            pLabelItem->setBackgroundColor(qcolorDeFaultItemBGColor);
            pLabelItem->setStatusLabel(" ");
            pLabelItem->repaint();
            qApp->processEvents();

            QThread::msleep(100);
        }

        item->listWidget()->takeItem(item->listWidget()->row(item));

        ui->PlayListView->repaint();
        qApp->processEvents();

    }
    else
    {
        QColor colorOriginal (pLabelItem->getBackgroundColor());

        pLabelItem->setBackgroundColor(colorHighLight);
        pLabelItem->setStatusLabel(":/Images/x-mark-3-32.ico", QSize(25,25));

        qApp->processEvents();

        QThread::msleep(500);

        pLabelItem->setBackgroundColor(colorOriginal);
        pLabelItem->setStatusLabel(":/Images/play-32.ico", QSize(10,10));

        qDebug() << __FUNCTION__ << " Error deleting data: " << dbControler.getLastError() << endl;
    }

    ui->PlayListView->update();
    ui->PlayListView->repaint();
    qApp->processEvents();

}

void QtSpotify::on_SearchButton_clicked()
{
    on_SearchModel_returnPressed();
}
