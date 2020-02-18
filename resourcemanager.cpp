#include "resourcemanager.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QWindow>
ResourceManager::ResourceManager(QWidget *parent) : QWidget(parent)
{

    setWindowFlags(this->windowFlags() |
                   Qt::Window);
    setWindowFlags(this->windowFlags() & ~Qt::WindowMaximizeButtonHint);

    QVBoxLayout* lout = new QVBoxLayout(this);
    treeWgt = new QTreeWidget(this);
    QPushButton* updateBtn = new QPushButton(this);

    updateBtn->setText("Update");
    this->setWindowTitle("Resource Manager");

    connect(updateBtn, &QPushButton::clicked, this, &ResourceManager::update);

    lout->addWidget(treeWgt);
    lout->addWidget(updateBtn);
}


void ResourceManager::update()
{
    treeWgt->clear();
    for(auto dat : MeshData::getMeshIdList())
    {
        QTreeWidgetItem* itm = new QTreeWidgetItem();
        itm->setText(0, RMeshModel::Name::get(dat));
        treeWgt->addTopLevelItem(itm);
    }
}
