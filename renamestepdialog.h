#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QDebug>

namespace Ui {
class RenameStepDialog;
}

class RenameStepDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RenameStepDialog(QString s);
    ~RenameStepDialog();

signals:
    void send_name(QString);
private slots:
    void on_pushButton_clicked();

private:
    Ui::RenameStepDialog *ui;
};


#endif // DIALOG_H
