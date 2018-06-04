#include "labelitem.h"
#include "ui_labelitem.h"

LabelITem::LabelITem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LabelITem)
{
    ui->setupUi(this);
}

LabelITem::~LabelITem()
{
    delete ui;
}
