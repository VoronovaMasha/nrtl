#ifndef TRACTWIDGET_SIMPLE_H
#define TRACTWIDGET_SIMPLE_H
#include <QWidget>
#include <QListWidget>
#include <QPushButton>

class TractWidget_simple : public QWidget
{
    Q_OBJECT
public:
    TractWidget_simple(QWidget* parent=nullptr);
public slots:
    void make_tracts();

private:
    QListWidget* list;
    QPushButton* btn_acc;
};

#endif // TRACTWIDGET_SIMPLE_H
