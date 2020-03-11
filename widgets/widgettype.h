#ifndef WIDGETTYPE_H
#define WIDGETTYPE_H
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include "NrtlType.h"

class NrtlItem : public QTreeWidgetItem
{
public:
    DataId id;
    bool show;
};

class MainMeshItem : public NrtlItem
{};

class CutItem : public NrtlItem
{};

class SectionItem : public NrtlItem
{};

class StepItem : public NrtlItem
{};

class NrtlLabel : public QTreeWidgetItem
{};

class LtSurfItem : public NrtlItem
{};

class TractItem : public NrtlItem{
private:
    IGroupId _gid;
public:

     TractItem(IGroupId gid, DataId tr_id)
     {
         id = tr_id;
         _gid = gid;
     }
};


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

#endif // WIDGETTYPE_H
