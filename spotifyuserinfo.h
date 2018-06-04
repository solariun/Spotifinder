#ifndef SPOTIFYUSERINFO_H
#define SPOTIFYUSERINFO_H

#include <QObject>

class SpotifyUserInfo : public QObject
{
    Q_OBJECT
public:
    explicit SpotifyUserInfo(QObject *parent = nullptr);

signals:

public slots:
};

#endif // SPOTIFYUSERINFO_H