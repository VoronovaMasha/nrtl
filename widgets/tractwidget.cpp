#include "tractwidget.h"
#include "nrtlmanager.h"
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QMenu>
#include "dialog1.h"
#include "dialog2.h"

TractWidget::TractWidget(QWidget* parent) :
    QWidget(parent)
{
    currentIt = nullptr;
    lout = new QVBoxLayout();
    tree = new QTreeWidget();

    tree->header()->hide();
    tree->clear();

    addTrBtn = new QPushButton("Add tract");
    lout->addWidget(tree);
    lout->addWidget(addTrBtn);
    tree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(addTrBtn, &QPushButton::clicked,
            this, &TractWidget::add_tract);
    connect(tree, &QTreeWidget::customContextMenuRequested,
            this, &TractWidget::slotCustomMenuRequested);
    this->setLayout(lout);

/*
    QVBoxLayout* lout = new QVBoxLayout();
    list = new QListWidget();
    btn_acc = new QPushButton("Create");
    connect(btn_acc, &QPushButton::clicked, this, &TractWidget::make_tracts);
    lout->addWidget(list);
    lout->addWidget(btn_acc);
    this->setLayout(lout);*/
}

void TractWidget::add_tract()
{
    //Здесь должно открываться окно group и возвращать имя и иконку, далее эту информацию помещать в консруктор Tract,
    //только в констурктор нужно еще будет добавить QIcon icon

    IGroupId gid = GroupViewer::getGroup();
    TractItem *st;
    if(gid._id != NONE)
    {
        DataId tr_id = RTractM::create(gid);

        st = new TractItem(gid, tr_id);
        v_tracts.push_back(st);
        RTractM::Visibility::set(tr_id, true);
        tree->addTopLevelItem(st);
    }


}

void TractWidget::slotCustomMenuRequested(QPoint pos)
{
    QTreeWidgetItem *itm = tree->currentItem();
    currentIt=itm;
    showContextMenu(currentIt,pos);
}

// создание контекстных меню для определнных секций
void TractWidget::showContextMenu(QTreeWidgetItem* item, const QPoint& globalPos)
{
    QMenu * menu = new QMenu(this);
    currentIt = item;

    for(auto i =0; i < v_tracts.size(); ++i)
    {
        TractItem *tract = v_tracts[i];
        if(item == tract)
        {
            QAction * addDevice1 = new QAction("Rename", this);
            QAction * addDevice2 = new QAction("Remove", this);
            menu->addAction(addDevice1);
            menu->addAction(addDevice2);
            menu->popup(tree->viewport()->mapToGlobal(globalPos));

            connect(addDevice1, SIGNAL(triggered()), this, SLOT(rename()));
            connect(addDevice2, SIGNAL(triggered()), this, SLOT(remove()));
            break;
        }
        else
        {
            for(auto j = 0; j < tract->v_section_in_tract.size(); ++j)
            {
                if(item == tract->v_section_in_tract[j]){
                    QAction * addDevice1 = new QAction("Transperancy", this);
                    QAction * addDevice2;
                    menu->addAction(addDevice1);
                    if(tract->v_section_in_tract[j]->show)
                    {
                        addDevice2 = new QAction("Hide", this);
                        menu->addAction(addDevice2);
                    }
                    else
                    {
                        addDevice2 = new QAction("Show", this);
                        menu->addAction(addDevice2);
                    }
                    menu->popup(tree->viewport()->mapToGlobal(globalPos));
                    connect(addDevice1, SIGNAL(triggered()), this, SLOT(transperancy()));
                    connect(addDevice2, SIGNAL(triggered()), this, SLOT(ShowHide()));
                    break;
                }
            }
            for(auto j = 0; j < tract->v_bok_in_tract.size(); ++j)
            {
                if(item == tract->v_bok_in_tract[j]){
                    QAction * addDevice1 = new QAction("Transperancy", this);
                    QAction * addDevice2;
                    menu->addAction(addDevice1);
                    if(tract->v_bok_in_tract[j]->show)
                    {
                        addDevice2 = new QAction("Hide", this);
                        menu->addAction(addDevice2);
                    }
                    else
                    {
                        addDevice2 = new QAction("Show", this);
                        menu->addAction(addDevice2);
                    }

                    menu->popup(tree->viewport()->mapToGlobal(globalPos));
                    connect(addDevice1, SIGNAL(triggered()), this, SLOT(transperancy()));
                    connect(addDevice2, SIGNAL(triggered()), this, SLOT(ShowHide()));
                    break;
                }
            }
        }
    }
}

void TractWidget::rename(){
    Dialog1 *d1 = new Dialog1(currentIt->text(0));
    connect (d1, SIGNAL (send_name(QString)),
             this, SLOT(change(QString)));
    d1->exec();
}


void TractWidget::remove()
{
    qDebug() << "здесь будет удаление трактов";
}


void TractWidget :: change(QString s){
    QTreeWidgetItem *item = tree->currentItem();
    for(auto i =0; i < v_tracts.size(); ++i)
    {
        TractItem *tract = v_tracts[i];
        if(item == tract)
        {
            item->setText(0,s);
            break;
        }
    }
}

void TractWidget::ShowHide()
{
    QTreeWidgetItem *item = tree->currentItem();
    for(auto i =0; i < v_tracts.size(); ++i)
    {
        TractItem *tract = v_tracts[i];

            for(auto j = 0; j < tract->v_section_in_tract.size(); ++j)
            {
                if(item == tract->v_section_in_tract[j]){
                    if(tract->v_section_in_tract[j]->show){
                        tract->v_section_in_tract[j]->show = false;
                    }
                    else{
                        tract->v_section_in_tract[j]->show = true;
                    }
                    break;
                }
            }
            for(auto j = 0; j < tract->v_bok_in_tract.size(); ++j)
            {
                if(item == tract->v_bok_in_tract[j])
                {
                    if(tract->v_bok_in_tract[j]->show){
                        tract->v_bok_in_tract[j]->show = false;
                    }
                    else{
                        tract->v_bok_in_tract[j]->show = true;
                    }
                    break;
                }
            }
        }
}

void TractWidget::transperancy()
{
    qDebug() << "Здесь будет устанавливаться прозрачность" <<endl;
    Dialog2 *d2 = new Dialog2();
    //connect (d2,?,?,?);
    d2->exec();
}

void TractWidget::make_tracts()
{
    list->clear();
    for(auto gid : ROutlinerData::GroupList::get())
    {
        DataId tr_id = RTractM::create(gid);
        list->addItem(new QListWidgetItem({RTractM::Name::get(tr_id)}));
        RTractM::Visibility::set(tr_id, true);
    }

}
