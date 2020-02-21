#include "tractwidget_simple.h"
#include "nrtlmanager.h"
#include <QListWidgetItem>
#include <QVBoxLayout>
TractWidget_simple::TractWidget_simple(QWidget* parent) :
    QWidget(parent)
{
    QVBoxLayout* lout = new QVBoxLayout();
    list = new QListWidget();
    btn_acc = new QPushButton("Create");
    connect(btn_acc, &QPushButton::clicked, this, &TractWidget_simple::make_tracts);
    lout->addWidget(list);
    lout->addWidget(btn_acc);
    this->setLayout(lout);
}

void TractWidget_simple::make_tracts()
{
    list->clear();
    for(auto gid : ROutlinerData::GroupList::get())
    {
        DataId tr_id = RTractM::create(gid);
        list->addItem(new QListWidgetItem({RTractM::Name::get(tr_id)}));
        RTractM::Visibility::set(tr_id, true);
    }

}
