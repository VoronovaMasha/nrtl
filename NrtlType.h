#ifndef NRTLTYPE_H
#define NRTLTYPE_H
#include <stdint.h>
#include <QColor>
#include <vector>

enum {NONE=0};

typedef uint32_t DataId;
typedef std::vector<DataId> ResourceList;

struct IGroupId
{
    IGroupId(int id = NONE, const QString& name="", QColor color = {0, 0, 0}) :
        _id(id), _color(color), _name(name)
    {

    }
    int _id;
    QColor _color;
    QString _name;

    inline bool operator==(const IGroupId& other) const
    {
        return (_id == other._id);
    }
    inline bool operator!=(const IGroupId& other) const
    {
        return (_id != other._id);
    }

    inline bool operator==(const int val) const
    {
        return (_id == val);
    }
    inline bool operator!=(const int val) const
    {
        return (_id != val);
    }


    inline IGroupId operator=(const int val)
    {
        _id = val;
        return *this;
    }

    void None()
    {
        _id = NONE;
    }
    bool isNone()
    {
        return (_id == NONE);
    }
};

/*bool operator==(const int val, const IGroupId& gr)
{
    return (gr._id == val);
}
bool operator!=(const int val, const IGroupId& gr)
{
    return (gr._id != val);
}

/*bool operator==(const IGroupId& gr, const int val)
{
    return (gr._id == val);
}
bool operator!=(const IGroupId& gr, const int val)
{
    return (gr._id != val);
}*/
#endif // NRTLTYPE_H
