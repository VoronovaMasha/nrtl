

#ifndef NRTLTYPE_H //условная компиляция при неопределенном NRTLTYPE_H
#define NRTLTYPE_H //идентифекатор на замену символов
#include <stdint.h> //подключене заголовочных файлов
#include <QColor>//подключене заголовочных файлов
#include <vector>//подключене заголовочных файлов
#include <QVector3D>//подключене заголовочных файлов

template<typename DocType=void*> //задание шаблонных классов
class IVContainer;
class MeshModel;

enum {NONE=0};

typedef uint64_t DataId;
typedef std::vector<DataId> ResourceList;
typedef QVector<QVector<int>> PolygonMatrix;
typedef QVector<QVector3D> MeshBorder;


namespace NrtlSettings {
    static QString PREV_DIR_VAR = "Previous Dir";
}
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
typedef std::vector<DataId> RStepList;
typedef std::vector<IGroupId> RGroupList;
typedef IVContainer<MeshModel*> RMeshData;
#endif // NRTLTYPE_H
