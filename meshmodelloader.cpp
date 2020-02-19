#include "MeshAlgorithm.h"
#include <QFile>
#include <QFileInfo>


QString MeshModelLoader::_error_string = QString("Error");
volatile MeshModelLoader::Status MeshModelLoader::_status = WAIT;
volatile float  MeshModelLoader::_progress = 0;
QString MeshModelLoader::_path = "";
bool MeshModelLoader::centerize = true;
SimpleMesh* MeshModelLoader::mesh = nullptr;
MeshModel* MeshModelLoader::meshModel = nullptr;

void MeshModelLoader::OBJ::loadMesh()
{
    static QMutex mtx;
    mtx.lock();
    _status = RUN;
    _progress = 0;
    mtx.unlock();

    try
    {
        QFile objFile(_path);
        QVector<QVector3D> coords;
        QVector<QVector2D> texcoords;
        QVector<VertexData> vertexes;
        QVector<GLuint> indexes;
        QVector<QVector<unsigned int>> polygons;
        if(!objFile.exists())
        {
            _error_string = "Did not found \".obj\" file.";
            mesh = nullptr;
            return;
        }
        objFile.open(QIODevice::ReadOnly);
        QTextStream input(&objFile);
        QString picture;
        bool is_picture=false;
        mtx.lock();
        _progress = 0.1;
        mtx.unlock();

        while(!input.atEnd())
        {
            QString str=input.readLine();
            QStringList list=str.split(" ");
            if(list[0]=="mtllib")
            {
                if(list.size()!=2)
                {
                    _error_string = "Bad \".obj\" file (problems in string \"mtllib ...\").";
                    mesh = nullptr;
                    return;
                }
                QFileInfo info(_path);
                if(list[1].startsWith("./"))
                    list[1].remove(0,2);
                QFile mtlFile(info.absolutePath()+"/"+list[1]);
                if(!mtlFile.exists())
                {
                    _error_string = "Did not found \".mtl\" file.";
                    mesh = nullptr;
                    return;
                }
                mtlFile.open(QIODevice::ReadOnly);
                QTextStream input2(&mtlFile);
                while(!input2.atEnd())
                {
                    QString str2=input2.readLine();
                    QStringList list2=str2.split(" ");
                    if(list2[0]=="map_Kd")
                    {
                        if(list2.size()!=2)
                        {
                            _error_string = "Bad \".mtl\" file (problems in string \"map_Kd ...\").";
                            mesh = nullptr;
                            return;
                        }
                        if(list2[1].startsWith("./"))
                            list2[1].remove(0,2);
                        picture=info.absolutePath()+"/"+list2[1];
                        is_picture=true;
                        mtlFile.close();
                        break;
                    }
                    mtx.lock();
                    _progress = 0.3;
                    mtx.unlock();
                }
                continue;
            }
            if(list[0]=="v")
            {
                if(list.size()!=4 && list.size()!=7)
                {
                    _error_string = "Bad \".obj\" file (problems in string \"v ...\").";
                    mesh = nullptr;
                    return;
                }
                coords.append(QVector3D((list[1].toFloat()/50.f),(list[2].toFloat()/50.f),(list[3].toFloat()/50.f)));
                continue;
            }
            if(list[0]=="vt")
            {
                if(list.size()!=3)
                {
                    _error_string = "Bad \".obj\" file (problems in string \"vt ...\").";
                    mesh = nullptr;
                    return;
                }
                texcoords.append(QVector2D(list[1].toFloat(),list[2].toFloat()));
                continue;
            }
            if(list[0]=="f")
            {
                if(list.size()!=4)
                {
                    _error_string = "Bad \".obj\" file (problems in string \"f ...\").";
                    mesh = nullptr;
                    return;
                }
                QStringList vert1=list[1].split("/");
                QStringList vert2=list[2].split("/");
                QStringList vert3=list[3].split("/");
                QVector<unsigned int> tmp,ttmp;
                tmp.push_back(vert1[0].toUInt());
                tmp.push_back(vert2[0].toUInt());
                tmp.push_back(vert3[0].toUInt());
                polygons.push_back(tmp);
                ttmp.push_back(vert1[1].toUInt());
                ttmp.push_back(vert2[1].toUInt());
                ttmp.push_back(vert3[1].toUInt());
                if(texcoords.size()==0 || vert1.size()==1 || vert2.size()==1 || vert3.size()==1)
                {
                    is_picture=false;
                    vertexes.append(VertexData(coords[tmp[0]-1],QVector2D(0.0,1.0),QVector3D(1.0,0.0,0.0)));
                    vertexes.append(VertexData(coords[tmp[1]-1],QVector2D(0.0,0.0),QVector3D(1.0,0.0,0.0)));
                    vertexes.append(VertexData(coords[tmp[2]-1],QVector2D(1.0,1.0),QVector3D(1.0,0.0,0.0)));
                }
                if(texcoords.size()!=0 && vert1.size()>1 && vert2.size()>1 && vert3.size()>1)
                {
                    vertexes.append(VertexData(coords[tmp[0]-1],texcoords[ttmp[0]-1],QVector3D(1.0,0.0,0.0)));
                    vertexes.append(VertexData(coords[tmp[1]-1],texcoords[ttmp[1]-1],QVector3D(1.0,0.0,0.0)));
                    vertexes.append(VertexData(coords[tmp[2]-1],texcoords[ttmp[2]-1],QVector3D(1.0,0.0,0.0)));
                }
                QVector3D a=vertexes[vertexes.size()-3].position;
                QVector3D b=vertexes[vertexes.size()-2].position;
                QVector3D c=vertexes[vertexes.size()-1].position;
                QVector3D n1=QVector3D(c.x()-a.x(),c.y()-a.y(),c.z()-a.z());
                QVector3D n2=QVector3D(b.x()-a.x(),b.y()-a.y(),b.z()-a.z());
                QVector3D n=QVector3D(n1.y()*n2.z()-n1.z()*n2.y(),n1.z()*n2.x()-n1.x()*n2.z(),n1.x()*n2.y()-n1.y()*n2.x());
                QVector3D norm=QVector3D(-n.x()/n.length(),-n.y()/n.length(),-n.z()/n.length());
                vertexes[vertexes.size()-3].normal=norm;
                vertexes[vertexes.size()-2].normal=norm;
                vertexes[vertexes.size()-1].normal=norm;
                indexes.append(indexes.size());
                indexes.append(indexes.size());
                indexes.append(indexes.size());
                if(texcoords.size()==0 || vert1.size()==1 || vert2.size()==1 || vert3.size()==1)
                {
                    is_picture=false;
                    vertexes.append(VertexData(coords[tmp[2]-1],QVector2D(0.0,1.0),QVector3D(1.0,0.0,0.0)));
                    vertexes.append(VertexData(coords[tmp[1]-1],QVector2D(0.0,0.0),QVector3D(1.0,0.0,0.0)));
                    vertexes.append(VertexData(coords[tmp[0]-1],QVector2D(1.0,1.0),QVector3D(1.0,0.0,0.0)));
                }
                if(texcoords.size()!=0 && vert1.size()>1 && vert2.size()>1 && vert3.size()>1)
                {
                    vertexes.append(VertexData(coords[tmp[2]-1],texcoords[ttmp[2]-1],QVector3D(1.0,0.0,0.0)));
                    vertexes.append(VertexData(coords[tmp[1]-1],texcoords[ttmp[1]-1],QVector3D(1.0,0.0,0.0)));
                    vertexes.append(VertexData(coords[tmp[0]-1],texcoords[ttmp[0]-1],QVector3D(1.0,0.0,0.0)));
                }
                a=vertexes[vertexes.size()-3].position;
                b=vertexes[vertexes.size()-2].position;
                c=vertexes[vertexes.size()-1].position;
                n1=QVector3D(c.x()-a.x(),c.y()-a.y(),c.z()-a.z());
                n2=QVector3D(b.x()-a.x(),b.y()-a.y(),b.z()-a.z());
                n=QVector3D(n1.y()*n2.z()-n1.z()*n2.y(),n1.z()*n2.x()-n1.x()*n2.z(),n1.x()*n2.y()-n1.y()*n2.x());
                norm=QVector3D(-n.x()/n.length(),-n.y()/n.length(),-n.z()/n.length());
                vertexes[vertexes.size()-3].normal=norm;
                vertexes[vertexes.size()-2].normal=norm;
                vertexes[vertexes.size()-1].normal=norm;
                indexes.append(indexes.size());
                indexes.append(indexes.size());
                indexes.append(indexes.size());
                continue;
            }
        }
        objFile.close();
        mtx.lock();
        _progress += 0.4;
        mtx.unlock();

        if(is_picture)
            mesh=new SimpleMesh(vertexes,indexes,QImage(picture));
        else
            mesh=new SimpleMesh(vertexes,indexes,QImage(":/cube.png"));
        mesh->polygons=polygons;
        mesh->coords=coords;
        mtx.lock();
        _status = WAIT;
        _progress = 0.99;
        mtx.unlock();
    }
    catch(...)
    {
        _error_string = "There was an error during loading the model. Maybe the reason is the limits of your RAM.";

        mtx.lock();
        _status = WAIT;
        _progress = 0;
        mtx.unlock();
        mesh = nullptr;
        return;
    }
}
