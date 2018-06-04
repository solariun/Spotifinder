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

#include "labelitem.h"
#include "ui_labelitem.h"
#include <QPalette>
#include <QBrush>
#include <QDebug>

void LabelItem::resizeEvent(QResizeEvent *event)
{
    noErros();

    QWidget::resizeEvent(event);

    int nDynWidth = this->width() - ui->IconLabel->width() - ui->StatusLabel->width();

    if (ui->MainLabel->width() != nDynWidth)
    {
        ui->MainLabel->setMaximumWidth(nDynWidth);

        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);

        ui->MainLabel->setSizePolicy(sizePolicy);

        //qDebug() << "Resize event on LabelItem risen...." << endl;

        QWidget::resizeEvent(event);
    }

    this->repaint();
}

LabelItem::LabelItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LabelITem)
{
    noErros();
    ui->setupUi(this);
}

LabelItem::~LabelItem()
{
    noErros();
    delete ui;

   // qDebug() << "Deletando " << __FUNCTION__ << endl;
}


void LabelItem::setMainIcon(const QPixmap &pixMap)
{
    noErros();
    ui->IconLabel->setPixmap(pixMap);
    ui->IconLabel->setScaledContents(true);
}


const QPixmap* LabelItem::getMainIcon()
{
    return ui->IconLabel->pixmap();
}


void LabelItem::setMainLabel(const QString &strValue)
{
    noErros();
    ui->MainLabel->setText(strValue);
}

QString LabelItem::getMailLabelText()
{
    noErros();

    return ui->MainLabel->text();
}


void LabelItem::setStatusLabel(const QString &strValue)
{
    noErros();
    ui->StatusLabel->setText(strValue);
}


void LabelItem::setStatusLabel(const QPixmap &pixMap)
{
    noErros();
    ui->StatusLabel->setPixmap(pixMap);
    ui->StatusLabel->setScaledContents(true);
    this->repaint();
}

void LabelItem::setStatusLabel(QString strIconPath, QSize size)
{
    noErros();

    QIcon qIcon;

    qIcon.addFile(strIconPath, QSize(), QIcon::Normal, QIcon::On);

    ui->StatusLabel->setPixmap(qIcon.pixmap(size));

    //this->repaint();
}



void LabelItem::setStatusLabel(QIcon qIcon, QSize size)
{
    noErros();

    ui->StatusLabel->setPixmap(qIcon.pixmap(size));

    //this->repaint();
}

void LabelItem::setKey(const QString strKey)
{
    noErros();
    this->strKey = strKey;

    //this->repaint();
}

void LabelItem::setBackgroundColor(QColor colorBG)
{
    noErros();
    QBrush brush(colorBG);
    brush.setStyle(Qt::SolidPattern);
    QPalette palette (this->palette());
    palette.setBrush(QPalette::Active, QPalette::Base, brush);
    this->setPalette(palette);

    //this->repaint();
}

void LabelItem::setDefaultFontColor(QColor colorFont)
{
    noErros();
    QBrush brush(colorFont);
    brush.setStyle(Qt::SolidPattern);
    QPalette palette (this->palette());
    palette.setBrush(QPalette::Active, QPalette::Text, brush);
    this->setPalette(palette);

    //this->repaint();
}

QColor LabelItem::getBackgroundColor()
{
    noErros();
    return this->palette().brush(QPalette::Active, QPalette::Base).color();
}


QString LabelItem::getKey()
{
    noErros();
    return strKey;
}



