#include "qtspotify.h"
#include "ui_qtspotify.h"

QtSpotify::QtSpotify(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QtSpotify)
{
    ui->setupUi(this);
}

QtSpotify::~QtSpotify()
{
    delete ui;
}
