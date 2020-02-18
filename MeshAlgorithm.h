#ifndef MESHALGORITHM_H
#define MESHALGORITHM_H
#include "meshmodel.h"
#include <QMatrix4x4>
class MeshModelLoader
{
public:
    class OBJ
    {
    public:
        static MeshModel* loadMesh(const QString& path, bool centerize = true);
    };

    static QString errorString()
    {
        return _error_string;
    }

private:
    static QString _error_string;
};

class AlignMesh
{
public:
   static QMatrix4x4 getAlignMatrix(QVector<QVector3D> dst, QVector<QVector3D> src);
};



#endif // MESHALGORITHM_H
