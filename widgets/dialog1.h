#ifndef DIALOG1_H
#define DIALOG1_H

#include <QDialog>

namespace Ui {
class Dialog1;
}

class Dialog1 : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog1(QString s);
    ~Dialog1();
signals:
    void send_name(QString);
private slots:
    void on_pushButton_clicked();

private:
    Ui::Dialog1 *ui;
};

#endif // DIALOG1_H
