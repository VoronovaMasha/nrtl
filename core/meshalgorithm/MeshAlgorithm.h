#ifndef MESHALGORITHM_H
#define MESHALGORITHM_H
#include "meshmodel.h"
#include <QMatrix4x4>
#include <string>
#include <QMutex>

struct MeshTopology
{
    static QString errorString()
    {
        return _error_string;
    }
    static PolygonMatrix makePolygonMatrix(MeshModel* mesh);
    static MeshBorder makeBorder(MeshModel* mesh);
    static MeshModel* makeSurface(MeshModel* a , MeshModel* b);
private:
    static QString _error_string;
};


class MeshCutter
{
public:
    static MeshModel* cutFromMesh(DataId mesh_id);
    static QString errorString()
    {
        return _error_string;
    }
    class CutVertexes
    {
    public:
        static QVector<QVector3D>& get()
        {
            return cutVertexes;
        }
        static bool set(QVector<QVector3D> new_cutVertexes,QVector<QVector3D> new_tr_cutVertexes)
        {
            if(new_tr_cutVertexes.size()>0)
                cutVertexes=new_cutVertexes+new_tr_cutVertexes;
            else
                cutVertexes=new_cutVertexes;
            if(cutVertexes.size()<3)
                _error_string=QString("You must choose at least 3 vertexes.");
            else
                _error_string=QString("No error");
            return true;
        }
    };
    class ViewMatrix
    {
    public:
        static QMatrix4x4 get()
        {
            return ViewMatrix2;
        }
        static bool set(QMatrix4x4 ViewMatrix)
        {
            ViewMatrix2=ViewMatrix;
            return true;
        }
    };
private:
    static QString _error_string;
    static QVector<QVector3D> cutVertexes;
    static QMatrix4x4 ViewMatrix2;
};

struct SimpleMesh
{
    SimpleMesh(QVector<VertexData> dat, QVector<GLuint> ind, QImage img) :
        vertData(dat), indexes(ind), texture(img)
    {}
    QVector<VertexData> vertData;
    QVector<GLuint> indexes;
    QImage texture;
    QVector<QVector<unsigned int>> polygons;
    QVector<QVector<unsigned int>> texpolygons;
    QVector<QVector3D> coords;
    QVector<QVector2D> texcoords;

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

    static int getProgress()
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
            meshModel->texcoords=mesh->texcoords;
            meshModel->texpolygons=mesh->texpolygons;

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
    static volatile int  _progress;  //from 0 to 100
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
