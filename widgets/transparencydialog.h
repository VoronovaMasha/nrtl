#ifndef TRANSPARENCYDIALOG_H
#define TRANSPARENCYDIALOG_H

#include <QDialog>
#include "NrtlType.h"

namespace Ui {
class TransparencyDialog;
}

class TransparencyDialog : public QDialog
{
    Q_OBJECT

public:
    TransparencyDialog(int value,DataId id,QWidget *parent = nullptr);
    ~TransparencyDialog();
signals:
    void new_value(int,DataId);
private slots:
    void on_horizontalSlider_valueChanged(int value);

    void on_pushButton_clicked();

private:
    Ui::TransparencyDialog *ui;
    DataId id;
};

#endif // TRANSPARENCYDIALOG_H
