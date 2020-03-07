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

void TractWidget::update()
{
    tree->clear();
    tree->setColumnCount(3);
    for(auto tract : RTractM::get())
    {
        TractItem* tr_itm = new TractItem(RTractM::GroupId::get(tract), tract);
        tree->addTopLevelItem(tr_itm);
    }
    emit update_tract_tree();
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

    TractItem *tract = dynamic_cast<TractItem*>(item);
    SectionItem *section = dynamic_cast<SectionItem*>(item);
    LtSurfItem *surface = dynamic_cast<LtSurfItem*>(item);

    if(tract != nullptr)
    {
        QAction* addDevice1 = new QAction("Rename", this);
        QAction* addDevice2 = new QAction(this);
        bool vis = RTractM::Visibility::get(tract->_tr_id);
        if(vis)
        {
            addDevice2 = new QAction("Hide", this);
            menu->addAction(addDevice2);
        }
        else
        {
            addDevice2 = new QAction("Show", this);
            menu->addAction(addDevice2);
        }
        QAction* addDevice3 = new QAction("Remove", this);

        QAction* act_createLtSurf = new QAction("Create Lat. Surface", this);
        menu->addAction(addDevice1);
        menu->addAction(addDevice2);
        menu->addAction(addDevice3);
        menu->addAction(act_createLtSurf);
        menu->popup(tree->viewport()->mapToGlobal(globalPos));

        connect(addDevice1, SIGNAL(triggered()), this, SLOT(rename()));
        connect(addDevice2, &QAction::triggered,
                [tract, vis, this](){ RTractM::Visibility::set(tract->_tr_id, !vis ); update(); });
        connect(addDevice3, SIGNAL(triggered()), this, SLOT(remove()));
        connect(act_createLtSurf, &QAction::triggered, [this]()
        {
            RTractM::LtSurface::create(dynamic_cast<TractItem*>(tree->currentItem())->_tr_id);
        });
        return;
    }

    if(section != nullptr)
    {
        QAction * addDevice1 = new QAction("Transperancy", this);
        QAction * addDevice2;
        menu->addAction(addDevice1);
        if(RSectionModel::Visibility::get(section->id))
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
        connect(addDevice2, &QAction::triggered, [](){});
        return;
    }

    if(surface != nullptr)
    {
        QAction * addDevice1 = new QAction("Transperancy", this);
        QAction * addDevice2;
        menu->addAction(addDevice1);
        if(RTractM::Visibility::get(surface->id_bok))
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
        return;
    }
}

void TractWidget::rename(){
    Dialog1 *d1 = new Dialog1(currentIt->text(0));
    connect (d1, SIGNAL (send_name(QString)),
             this, SLOT(change(QString)));
    d1->exec();
    update();
}


void TractWidget::remove()
{
    TractItem* itm =
            dynamic_cast<TractItem*>(list->currentItem());
    if(itm != nullptr)
    {
        RTractM::remove(itm->_tr_id);
    }
    update();
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
    update();
}

void TractWidget::ShowHide()
{
/*    QTreeWidgetItem *item = tree->currentItem();
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
        }*/
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
