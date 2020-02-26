#ifndef TRACTVIEW_H
#define TRACTVIEW_H
#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

#include "nrtlmanager.h"

class GroupItem : public QListWidgetItem
{
public:
    GroupItem(const QIcon &icon,
                       const QString &text,
                       QListWidget *parent = nullptr,
                       int type = Type) :
        QListWidgetItem(icon, text, parent, type)
    {

    }
    IGroupId _group;
};

class GroupEditorWidget : public QWidget
{
Q_OBJECT
public:
    GroupEditorWidget(QWidget* parent, bool edit=true);
    void rewrite();
    IGroupId getCurrentId()
    {
        IGroupId _none;
        if(mainList->currentItem() == nullptr)
        {
            qDebug() << "id=" << _none._id;
            return _none;
        }
        else
        {
            qDebug() << "id!=0";
            return dynamic_cast<GroupItem*>(mainList->currentItem())->_group;
        }
    }

signals:
    void update_group_tree();
private:
    QWidget *wid;
    QListWidget *mainList;
    QVBoxLayout *boxV;
    QPushButton *addGroupBtn;
//    QMap< QListWidgetItem*, QPair<QString,QIcon>> mapLink;
//    QMap<QString,QIcon> textIcon;
    QLineEdit *line;
    bool _edit;

public slots:
   void select_color();
   void add_group(QColor clr, QString name);
   static QIcon makeIcon(QColor clr);
   void slotCustomMenuRequested(QPoint);
   void showContextMenu(const QPoint& globalPos);
   void window_rename();
   void rename();
   void change_color();
   void delete_color();

signals:
};

#endif // TRACTVIEW_H
