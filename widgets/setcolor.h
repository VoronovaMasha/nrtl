#ifndef SETCOLOR_H
#define SETCOLOR_H

#include <QDialog>
#include <QColor>
namespace Ui {
class SetColor;
}

class SetColor : public QDialog
{
    Q_OBJECT

public:
    SetColor(QWidget *parent = nullptr);
    ~SetColor();

private:
    Ui::SetColor *ui;
    QColor clr;
private slots:
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
signals:
    void info_about_all(QColor,QString);
};

#endif // SETCOLOR_H
