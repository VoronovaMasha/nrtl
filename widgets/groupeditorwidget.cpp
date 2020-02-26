#include "groupeditorwidget.h"
#include <QDebug>
#include "setcolor.h"
#include <QMap>
#include <QPair>
#include <QPainter>
#include <QMenu>
#include <QLabel>
#include <QLineEdit>
#include <QColorDialog>
#include <QMessageBox>

#include "nrtlmanager.h"

GroupEditorWidget::GroupEditorWidget(QWidget* parent, bool edit) :
    QWidget(parent), _edit(edit)
{
    boxV = new QVBoxLayout(this);
    mainList = new QListWidget();
    mainList->setContextMenuPolicy(Qt::CustomContextMenu);
    mainList->setIconSize(QSize(50,50));
    addGroupBtn = new QPushButton("Add group");
    boxV->addWidget(mainList);
    if(edit)
        boxV->addWidget(addGroupBtn);
    connect(addGroupBtn, SIGNAL(clicked()), this, SLOT(select_color()));
    connect(mainList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested(QPoint)));
}

//перезапись
void GroupEditorWidget::rewrite()
{
    mainList->clear();
    for(auto group : ROutlinerData::GroupList::get())
    {
        QIcon icon = makeIcon(group._color);
        GroupItem *itm = new GroupItem(icon, group._name);
        itm->_group = group;
        mainList->addItem(itm);
    }
    emit update_group_tree();
}

void GroupEditorWidget::slotCustomMenuRequested(QPoint pos)
{
    showContextMenu(pos);
}

void GroupEditorWidget::showContextMenu(const QPoint &globalPos)
{

    QMenu *menu = new QMenu(this);
    QAction *addDevice = new QAction("Rename", this);
    QAction *addDevice2 = new QAction("Change color", this);
    QAction *addDevice3 = new QAction("Delete", this);
    menu->addAction(addDevice);
    menu->addAction(addDevice2);
    menu->addAction(addDevice3);
    menu->popup(mainList->viewport()->mapToGlobal(globalPos));
    connect(addDevice, SIGNAL(triggered()), this, SLOT(window_rename()));
    connect(addDevice2, SIGNAL(triggered()), this, SLOT(change_color()));
    connect(addDevice3, SIGNAL(triggered()), this, SLOT(delete_color()));
}

void GroupEditorWidget::window_rename()
{
    wid = new QWidget();
    QHBoxLayout *box = new QHBoxLayout(wid);
    QPushButton *but = new QPushButton("OK");
    //wid->show();
    QLabel *lab = new QLabel();
    lab->setText("Rename");
    line = new QLineEdit();
    box->addWidget(lab);
    box->addWidget(line);
    box->addWidget(but);
    wid->show();
    connect(but, SIGNAL(clicked()),this, SLOT(rename()));
}

void GroupEditorWidget::rename()
{
    if (line->text() != ""){
    wid->close();
    IGroupId id = dynamic_cast<GroupItem*>(mainList->currentItem())->_group;
    GroupId::Name::set(id, line->text());
    rewrite();
    }
    else{
        QMessageBox::information(wid,"Warning","You dont enter a name");
    }
}

void GroupEditorWidget::change_color()
{
    QColor clr = QColorDialog::getColor();
    IGroupId id = dynamic_cast<GroupItem*>(mainList->currentItem())->_group;
    GroupId::Color::set(id, clr);
    rewrite();
}

void GroupEditorWidget::delete_color()
{
    IGroupId id = dynamic_cast<GroupItem*>(mainList->currentItem())->_group;
    ROutlinerData::GroupList::remove(id);
    GroupId::remove(id);
    rewrite();
}



void GroupEditorWidget::select_color()
{
    SetColor *st = new SetColor();
    st->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    connect(st, SIGNAL(info_about_all(QColor, QString)), this, SLOT(add_group(QColor, QString)));
    st->show();
}

void GroupEditorWidget::add_group(QColor clr, QString n)
{
    //QListWidgetItem *itm_new = new QListWidgetItem(icon,n);
    //добавили новую группу
    IGroupId id = GroupId::create(n, clr);
    ROutlinerData::GroupList::add(id);
    rewrite();
}

QIcon GroupEditorWidget::makeIcon(QColor clr)
{
    QImage image(512, 512, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&image);
    painter.fillRect(0, 0, 512, 512, clr);
    QRgb rgb = clr.rgb();
    if(rgb == QColor(Qt::white).rgb()){
        QPen pen(Qt::black);
        pen.setWidth(20);
        painter.setPen(pen);
        painter.drawRect(0,0,512,512);
    }
    QPixmap pixmap;
    pixmap.convertFromImage(image);
    QIcon icon(pixmap);
    return icon;
}
