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


#ifndef LABELITEM_H
#define LABELITEM_H

#include "utils.h"

#include <QWidget>
#include <QPixmap>
#include <QColor>
#include <QIcon>

namespace Ui {
class LabelITem;
}

class LabelItem : public QWidget, public Utils::Error
{
    Q_OBJECT
protected:
    void resizeEvent(QResizeEvent *event);

public:
    explicit LabelItem(QWidget *parent = 0);
    ~LabelItem();

    void setMainIcon (const QPixmap& pixMap);
    const QPixmap *getMainIcon();

    void setMainLabel (const QString& strValue);
    QString getMailLabelText ();

    void setStatusLabel (const QString& strValue);
    void setStatusLabel (const QPixmap& pixMap);

    /*
     * Routines to add icons to status
     */
    void setStatusLabel (QString strIconPath, QSize size);
    void setStatusLabel (QIcon, QSize size);

    void setKey (const QString strKey);

    void setBackgroundColor (QColor colorBG);
    void setDefaultFontColor (QColor colorFont);

    QColor getBackgroundColor ();

    QString getKey ();

    void* pObject;

private:

    Ui::LabelITem *ui;
    QString strKey;
};

#endif // LABELITEM_H
