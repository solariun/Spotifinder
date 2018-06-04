#ifndef LABELITEM_H
#define LABELITEM_H

#include <QWidget>

namespace Ui {
class LabelITem;
}

class LabelITem : public QWidget
{
    Q_OBJECT

public:
    explicit LabelITem(QWidget *parent = 0);
    ~LabelITem();

private:
    Ui::LabelITem *ui;
};

#endif // LABELITEM_H
