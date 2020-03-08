#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QWidget>
#include <QTreeWidget>
#include <QDialog>
#include <QListWidget>
#include <QVBoxLayout>
#include "nrtlmanager.h"
#include "widgettype.h"


class ResourceManager : public QWidget
{
    Q_OBJECT
public:
    explicit ResourceManager(QWidget *parent = nullptr);
    static DataId getMesh(QWidget* parent=nullptr)
    {
        QDialog *wnd = new QDialog(parent);
        QTreeWidget *mainList = new QTreeWidget();
        DataId id = NONE;
        for(auto dat : MeshData::getMeshIdList())
        {
            NrtlItem* itm = new NrtlItem();
            itm->setText(0, RMeshModel::Name::get(dat));
            itm->id = dat;
            mainList->addTopLevelItem(itm);
        }

        QVBoxLayout* lout = new QVBoxLayout();

        QObject::connect(mainList, &QTreeWidget::doubleClicked,
                         wnd, &QDialog::close);
        QObject::connect(mainList, &QTreeWidget::doubleClicked,
                         [&id, mainList](){ id=dynamic_cast<NrtlItem*>(mainList->selectedItems()[0])->id; });
        lout->addWidget(mainList);
        wnd->setLayout(lout);
        wnd->exec();
        QObject::disconnect(mainList, &QTreeWidget::doubleClicked,
                            wnd, &QDialog::close);
        return id;
    }

signals:

public slots:

    void update();

private:
    QTreeWidget* treeWgt;
};

#endif // RESOURCEMANAGER_H
