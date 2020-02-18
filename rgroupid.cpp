#include "nrtlmanager.h"


IGroupId GroupId::create(const QString& name, QColor clr)
{
    IGroupId group(groupIdCounter, name, clr);
    ++groupIdCounter;
    groupLst.push_back(group);

    return group;
}
bool GroupId::remove(IGroupId id)
{
    groupLst.erase(std::find(groupLst.begin(), groupLst.end(), id));
    return true;
}

bool GroupId::Name::set(IGroupId group_id, const QString& name)
{
    auto it = groupLst.begin();

    for(; it != groupLst.end(); ++it)
        if((*it) == group_id)
            break;

    (*it)._name = name;
    return true;
}

QString GroupId::Name::get(IGroupId group_id)
{
    return group_id._name;
}


bool GroupId::Color::set(IGroupId group_id, QColor clr)
{
    auto it = groupLst.begin();

    for(; it != groupLst.end(); ++it)
        if((*it) == group_id)
            break;

    (*it)._color = clr;
    return true;
}

QColor GroupId::Color::get(IGroupId group_id)
{
    return group_id._color;
}
