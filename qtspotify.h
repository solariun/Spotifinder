#ifndef QTSPOTIFY_H
#define QTSPOTIFY_H

#include <QWidget>

namespace Ui {
class QtSpotify;
}

class QtSpotify : public QWidget
{
    Q_OBJECT

public:
    explicit QtSpotify(QWidget *parent = 0);
    ~QtSpotify();

private:
    Ui::QtSpotify *ui;
};

#endif // QTSPOTIFY_H
