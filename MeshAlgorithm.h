#ifndef MESHALGORITHM_H
#define MESHALGORITHM_H
#include "meshmodel.h"
#include <QMatrix4x4>
#include <string>
#include <QMutex>

struct SimpleMesh
{
    SimpleMesh(QVector<VertexData> dat, QVector<GLuint> ind, QImage img) :
        vertData(dat), indexes(ind), texture(img)
    {}
    QVector<VertexData> vertData;
    QVector<GLuint> indexes;
    QImage texture;
    QVector<QVector<unsigned int>> polygons;
    QVector<QVector3D> coords;

};

class MeshModelLoader
{
public:
    class OBJ
    {
    public:
        static void loadMesh();
    };

    enum Status { RUN, WAIT };

    static QString errorString()
    {
        return _error_string;
    }

    static Status getStatus()
    {
        return _status;
    }

    static float getProgress()
    {
        return _progress;
    }

    static void setPath(QString path)
    {
        _path = path;
    }

    static QString getPath()
    {
        return _path;
    }

    static MeshModel* getMesh()
    {
        if(mesh != nullptr)
        {
            meshModel = new MeshModel(mesh->vertData, mesh->indexes, mesh->texture);
            meshModel->polygons = mesh->polygons;
            meshModel->coords = mesh->coords;

            double x=0.f,y=0.f,z=0.f;
            for(int i=0;i<mesh->coords.size();i++)
            {
                x+=mesh->coords[i].x()/mesh->coords.size();
                y+=mesh->coords[i].y()/mesh->coords.size();
                z+=mesh->coords[i].z()/mesh->coords.size();
            }
            meshModel->translate(QVector3D(-x,-y,-z));
            delete mesh;
            mesh = nullptr;
            return meshModel;
        }
        else return nullptr;
    }


private:
    static QString         _error_string;
    static volatile Status _status;
    static volatile float  _progress;  //from 0 to 1
    static QString _path;
    static bool centerize;

    static SimpleMesh* mesh;
    static MeshModel* meshModel;
};

class AlignMesh
{
public:
   static QMatrix4x4 getAlignMatrix(QVector<QVector3D> dst, QVector<QVector3D> src);
};



#endif // MESHALGORITHM_H
