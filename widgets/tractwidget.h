#ifndef TRACTWIDGET_SIMPLE_H
#define TRACTWIDGET_SIMPLE_H
#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVector>
#include <QDialog>
#include "outlinerwidget.h"
#include "groupeditorwidget.h"

class GroupViewer
{
public:
    GroupViewer(QWidget* parent = nullptr);

    static IGroupId getGroup(QWidget* parent=nullptr)
    {
        bool isItemClckd = false;
        QDialog *wnd = new QDialog(parent);
        QListWidget *mainList = new QListWidget();
        for(auto group : ROutlinerData::GroupList::get())
        {
            QIcon icon = GroupEditorWidget::makeIcon(group._color);
            GroupItem *itm = new GroupItem(icon, group._name);
            itm->_group = group;
            mainList->addItem(itm);
        }
        QVBoxLayout* lout = new QVBoxLayout();

        QObject::connect(mainList, &QListWidget::doubleClicked,
                         wnd, &QDialog::close);
        QObject::connect(mainList, &QListWidget::doubleClicked,
                         [&isItemClckd](){ isItemClckd=true; });
        lout->addWidget(mainList);
        wnd->setLayout(lout);
        wnd->exec();
        QObject::disconnect(mainList, &QListWidget::doubleClicked,
                            wnd, &QDialog::close);
        if(isItemClckd && mainList->selectedItems().size() == 1)
        {
            return dynamic_cast<GroupItem*>(mainList->currentItem())->_group;
        }
        return IGroupId();
    }
};


class TractWidget : public QWidget
{
    Q_OBJECT
public:
    TractWidget(QWidget* parent=nullptr);

signals:
    void update_tract_tree();
public slots:
    void make_tracts();
    void update();
private slots:
    void add_tract();
    void slotCustomMenuRequested(QPoint);
    void showContextMenu(QTreeWidgetItem* item, const QPoint& globalPos);
   // void add_new_section();
    void rename();
    void remove();
    void change(QString);
    void ShowHide();
    void transperancy();

private:
    QListWidget* list;
    QPushButton* btn_acc;
    QTreeWidget *tree;
    QVBoxLayout *lout;
    QPushButton *addTrBtn;
    QVector<TractItem*> v_tracts;
    int how_many_step = 1;
    QTreeWidgetItem *currentIt;

    //QTreeWidgetItem *mainMesh;
};

#endif // TRACTWIDGET_SIMPLE_H
