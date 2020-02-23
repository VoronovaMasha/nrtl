#include "nrtlmanager.h"

DataId RSectionModel::create(MeshModel* obj)
{
    if (state() == TransactionState::WAIT)
        exit(1);
    model->meshData.create() = obj;
    return model->meshData.getId(obj);
/*		switch (type())
    {
    case TransactionType::SYNC:
        model->meshData.create() = obj;
        return model->meshData.getId(obj);
    case TransactionType::ASYNC:
        return model->meshData.curId();
    }

    return IVContainer<MeshModel*>::NONE;*/
}

bool RSectionModel::Name::set(DataId mesh_id, const QString& name)
{
    model->meshData.getElement(mesh_id)->setName(name);
    return true;
}
QString RSectionModel::Name::get(DataId mesh_id)
{
    return model->meshData.getElement(mesh_id)->name();
}


bool RSectionModel::Transperancy::set(DataId mesh_id, uint8_t val)
{
    model->meshData.getElement(mesh_id)->setTransperancy(val);
    return true;
}
uint8_t RSectionModel::Transperancy::get(DataId mesh_id)
{
    return model->meshData.getElement(mesh_id)->transperancy();
}


bool RSectionModel::Visibility::set(DataId mesh_id, bool vis)
{
    model->meshData.setVisFlag(model->meshData.getElement(mesh_id), vis);
    return true;
}

bool RSectionModel::Visibility::get(DataId mesh_id)
{
    return model->meshData.isVisible(model->meshData.getElement(mesh_id));
}

void RSectionModel::Border::set(DataId mesh_id, MeshBorder border)
{
    model->meshData.getElement(mesh_id)->border=border;
}

MeshBorder RSectionModel::Border::get(DataId mesh_id)
{
    return model->meshData.getElement(mesh_id)->border;
}

void RSectionModel::MatrixOfPolygons::set(DataId mesh_id, PolygonMatrix polygonMatrix)
{
    model->meshData.getElement(mesh_id)->polygonMatrix=polygonMatrix;
}

PolygonMatrix RSectionModel::MatrixOfPolygons::get(DataId mesh_id)
{
    return model->meshData.getElement(mesh_id)->polygonMatrix;
}

bool RSectionModel::ModelMatrix::set(DataId mesh_id, QMatrix4x4 mt)
{
    model->meshData.getElement(mesh_id)->setModelMatrix(mt);
    return true;
}

QMatrix4x4 RSectionModel::ModelMatrix::get(DataId mesh_id)
{
    return model->meshData.getElement(mesh_id)->getModelMatrix();
}


bool RSectionModel::GroupId::set(DataId section_id, IGroupId group_id)
{
    IStep* stp = getStep(model->meshData.getElement(section_id)->step());
    IGroupId _prev_group;
    DataId _prev_section;

    /* 1. Разорвать старые связи */
    _prev_group = stp->section_group_map.at(section_id);
    if(group_id._id != NONE)
    {
        _prev_section = stp->group_section_map.at(group_id);
        stp->section_group_map[_prev_section] = NONE;
    }
    if(_prev_group._id != NONE)
    {
        stp->group_section_map[_prev_group] = NONE;
    }
    /*2. Построить новые */
    stp->section_group_map[section_id] = group_id;
    stp->group_section_map[group_id] = section_id;

    /** назначить цвет модели **/

    model->meshData.getElement(section_id)->setColor(group_id._color);


    return true;
}

IGroupId RSectionModel::GroupId::get(DataId section_id)
{
    IStep* stp = getStep(model->meshData.getElement(section_id)->step());
    return stp->section_group_map.at(section_id);
}

DataId RSectionModel::Step::get(DataId section_id)
{
    return model->meshData.getElement(section_id)->step();
}

QString RSectionModel::Converge::_error_string=QString("");

MeshModel* RSectionModel::Converge::convergesections(DataId a,DataId b)
{
    if(MeshData::get().getElement(a)->coords!=MeshData::get().getElement(a)->coords)
    {
        _error_string=QString("You can make one section from tho only for sections of the same step.");
        return nullptr;
    }
    QVector<QVector3D> coords=MeshData::get().getElement(a)->coords;
    QVector<VertexData> vertexes;
    QVector<GLuint> indexes;
    QVector<QVector<unsigned int>> polygons=MeshData::get().getElement(a)->polygons+MeshData::get().getElement(b)->polygons;
    int i=0, j=0;
    while(1)
    {
        if(j==MeshData::get().getElement(a)->polygons.size())
            break;
        if(polygons.count(polygons[i])!=1)
        {
            polygons.remove(i);
            i--;
        }
        i++;
        j++;
    }
    for(j=0;j<polygons.size();j++)
    {
        vertexes.append(VertexData(coords[polygons[j][0]-1],QVector2D(0.0,1.0),QVector3D(1.0,0.0,0.0)));
        vertexes.append(VertexData(coords[polygons[j][1]-1],QVector2D(0.0,0.0),QVector3D(1.0,0.0,0.0)));
        vertexes.append(VertexData(coords[polygons[j][2]-1],QVector2D(1.0,1.0),QVector3D(1.0,0.0,0.0)));
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
        vertexes.append(VertexData(coords[polygons[j][2]-1],QVector2D(1.0,1.0),QVector3D(1.0,0.0,0.0)));
        vertexes.append(VertexData(coords[polygons[j][1]-1],QVector2D(0.0,0.0),QVector3D(1.0,0.0,0.0)));
        vertexes.append(VertexData(coords[polygons[j][0]-1],QVector2D(0.0,1.0),QVector3D(1.0,0.0,0.0)));
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
    }
    MeshModel* obj=new MeshModel(vertexes,indexes,QImage(":/img/textures/cube2.png"));
    obj->polygons=polygons;
    obj->coords=coords;
    return obj;
}


