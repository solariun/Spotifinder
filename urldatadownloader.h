#ifndef URLDATADOWNLOADER_H
#define URLDATADOWNLOADER_H

#include <QObject>

class UrlDataDownloader : public QObject
{
    Q_OBJECT
public:
    explicit UrlDataDownloader(QObject *parent = nullptr);

signals:

public slots:
};

#endif // URLDATADOWNLOADER_H