#include "nrtlmanager.h"
#include <QFile>
#include <QFileDialog>

bool ProjectOptions::Saver::saveProject(QString filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly))
        return false;
    QDataStream stream(&file);
    std::vector<DataId> modelIds=MeshData::getMeshIdList();
    stream<<modelIds.size();
    for(unsigned int i=0;i<modelIds.size();i++)
    {
        stream<<modelIds[i];
        stream<<RMeshModel::Visibility::get(modelIds[i]);
        stream<<RMeshModel::Name::get(modelIds[i]);
        stream<<MeshData::get().getElement(modelIds[i])->r;
        stream<<MeshData::get().getElement(modelIds[i])->g;
        stream<<MeshData::get().getElement(modelIds[i])->b;
        stream<<RMeshModel::Transperancy::get(modelIds[i]);
        stream<<RMeshModel::Step::get(modelIds[i]);
        stream<<RMeshModel::ModelMatrix::get(modelIds[i]);
        stream<<MeshData::get().getElement(modelIds[i])->polygons;
        stream<<MeshData::get().getElement(modelIds[i])->texpolygons;
        stream<<MeshData::get().getElement(modelIds[i])->coords;
        stream<<MeshData::get().getElement(modelIds[i])->texcoords;
        stream<<MeshData::get().getElement(modelIds[i])->image;
    }
    std::vector<IGroupId> groupLst;
    groupLst=ROutlinerData::GroupList::get();
    stream<<groupLst.size();
    for(unsigned int i=0;i<groupLst.size();i++)
    {
        stream<<groupLst[i]._id;
        stream<<GroupId::Name::get(groupLst[i]);
        stream<<GroupId::Color::get(groupLst[i]);
    }
    RStepList stepLst=ROutlinerData::StepList::get();
    stream<<stepLst.size();
    for(unsigned int i=0;i<stepLst.size();i++)
    {
        stream<<stepLst[i];
        stream<<RStep::Name::get(stepLst[i]);
        stream<<RStep::MeshCut::get(stepLst[i]);
        stream<<RStep::SectionList::get(stepLst[i]).size();
        for(unsigned int j=0;j<RStep::SectionList::get(stepLst[i]).size();j++)
            stream<<RStep::SectionList::get(stepLst[i])[j];
        std::unordered_map<DataId, IGroupId, DataHash> map1=NrtlManager::getStep(stepLst[i])->section_group_map;
        stream<<map1.size();
        for(auto it=map1.begin();it!=map1.end();it++)
        {
            stream<<it->first;
            stream<<(it->second)._id;
            stream<<(it->second)._name;
            stream<<(it->second)._color;
        }
        std::unordered_map<IGroupId, DataId, GroupHash> map2=NrtlManager::getStep(stepLst[i])->group_section_map;
        stream<<map2.size();
        for(auto it=map2.begin();it!=map2.end();it++)
        {
            stream<<(it->first)._id;
            stream<<(it->first)._name;
            stream<<(it->first)._color;
            stream<<it->second;
        }
    }
    stream<<ROutlinerData::MainMesh::get();
    stream<<ROutlinerData::WorkingStep::get();
    std::vector<ITract*> tractLst=RTractM::getTractLst();
    stream<<tractLst.size();
    stream<<RTractM::getIdCounter();
    for(unsigned int i=0;i<tractLst.size();i++)
    {
        stream<<tractLst[i]->_tr_id;
        stream<<tractLst[i]->_name;
        stream<<tractLst[i]->vis;
        stream<<tractLst[i]->groupId._id;
        stream<<tractLst[i]->groupId._color;
        stream<<tractLst[i]->groupId._name;
        stream<<tractLst[i]->secLst.size();
        for(unsigned int j=0;j<tractLst[i]->secLst.size();j++)
            stream<<tractLst[i]->secLst[j];
        stream<<tractLst[i]->latSurfLst.size();
        for(unsigned int j=0;j<tractLst[i]->latSurfLst.size();j++)
            stream<<tractLst[i]->latSurfLst[j];
    }
    file.close();
    return true;
}

bool ProjectOptions::Loader::loadProject(QString filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        return false;
    NrtlManager::restart();
    QDataStream stream(&file);
    unsigned int size;
    stream>>size;
    for(unsigned int i=0;i<size;i++)
    {
        DataId id;
        bool visibility;
        QString name;
        float r,g,b;
        uint8_t transparency;
        DataId step_id;
        QMatrix4x4 modelMatrix;
        QVector<QVector<unsigned int>> polygons;
        QVector<QVector<unsigned int>> texpolygons;
        QVector<QVector3D> coords;
        QVector<QVector2D> texcoords;
        QImage image;
        stream>>id;
        stream>>visibility;
        stream>>name;
        stream>>r;
        stream>>g;
        stream>>b;
        stream>>transparency;
        stream>>step_id;
        stream>>modelMatrix;
        stream>>polygons;
        stream>>texpolygons;
        stream>>coords;
        stream>>texcoords;
        stream>>image;
        QVector<VertexData> vertexes;
        QVector<GLuint> indexes;
        for(int j=0;j<polygons.size();j++)
        {
            if(texpolygons.size()!=0 && texcoords.size()!=0)
            {
                vertexes.append(VertexData(coords[polygons[j][0]-1],texcoords[texpolygons[j][0]-1],QVector3D(1.0,0.0,0.0)));
                vertexes.append(VertexData(coords[polygons[j][1]-1],texcoords[texpolygons[j][1]-1],QVector3D(1.0,0.0,0.0)));
                vertexes.append(VertexData(coords[polygons[j][2]-1],texcoords[texpolygons[j][2]-1],QVector3D(1.0,0.0,0.0)));
            }
            else
            {
                vertexes.append(VertexData(coords[polygons[j][0]-1],QVector2D(0.0,0.0),QVector3D(1.0,0.0,0.0)));
                vertexes.append(VertexData(coords[polygons[j][1]-1],QVector2D(0.0,1.0),QVector3D(1.0,0.0,0.0)));
                vertexes.append(VertexData(coords[polygons[j][2]-1],QVector2D(1.0,0.0),QVector3D(1.0,0.0,0.0)));
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
            if(texpolygons.size()!=0 && texcoords.size()!=0)
            {
                vertexes.append(VertexData(coords[polygons[j][2]-1],texcoords[texpolygons[j][2]-1],QVector3D(1.0,0.0,0.0)));
                vertexes.append(VertexData(coords[polygons[j][1]-1],texcoords[texpolygons[j][1]-1],QVector3D(1.0,0.0,0.0)));
                vertexes.append(VertexData(coords[polygons[j][0]-1],texcoords[texpolygons[j][0]-1],QVector3D(1.0,0.0,0.0)));
            }
            else
            {
                vertexes.append(VertexData(coords[polygons[j][2]-1],QVector2D(0.0,0.0),QVector3D(1.0,0.0,0.0)));
                vertexes.append(VertexData(coords[polygons[j][1]-1],QVector2D(0.0,1.0),QVector3D(1.0,0.0,0.0)));
                vertexes.append(VertexData(coords[polygons[j][0]-1],QVector2D(1.0,0.0),QVector3D(1.0,0.0,0.0)));
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
        }
        MeshModel* obj=new MeshModel(vertexes,indexes,image);
        obj->polygons=polygons;
        obj->texpolygons=texpolygons;
        obj->coords=coords;
        obj->texcoords=texcoords;
        NrtlManager::createTransaction(NrtlManager::SYNC);
        DataId mesh_id=RMeshModel::create(obj);
        NrtlManager::commitTransaction();
        MeshData::get().changeId(mesh_id,id);
        MeshData::get().changeIdCounter(id+1);
        RMeshModel::Name::set(id,name);
        RMeshModel::Transperancy::set(id,transparency);
        RMeshModel::Visibility::set(id,visibility);
        RMeshModel::Step::set(id,step_id);
        MeshData::get().getElement(id)->setModelMatrix(modelMatrix);
        MeshData::get().getElement(id)->r=r;
        MeshData::get().getElement(id)->g=g;
        MeshData::get().getElement(id)->b=b;
    }
    stream>>size;
    std::vector<IGroupId> groupLst;
    for(unsigned int i=0;i<size;i++)
    {
        int id;
        QString name;
        QColor color;
        stream>>id;
        stream>>name;
        stream>>color;
        groupLst.push_back(IGroupId(id,name,color));
        ROutlinerData::GroupList::changeIdCounter(id+1);
    }
    ROutlinerData::GroupList::setGroupLstInManagers(groupLst);
    ROutlinerData::GroupList::setGroupLstInOutliner(groupLst);
    stream>>size;
    for(unsigned int i=0;i<size;i++)
    {
        DataId id;
        QString name;
        DataId meshCutId;
        unsigned int secSize;
        stream>>id;
        stream>>name;
        stream>>meshCutId;
        stream>>secSize;
        RSectionList meshSectionIds;
        for(unsigned int j=0;j<secSize;j++)
        {
            DataId secId;
            stream>>secId;
            meshSectionIds.push_back(secId);
        }
        std::unordered_map<DataId, IGroupId, DataHash> map1;
        unsigned int mapSize;
        stream>>mapSize;
        for(unsigned int j=0;j<mapSize;j++)
        {
            DataId mapSecId;
            int mapGroupId;
            QString mapGroupName;
            QColor mapGroupColor;
            stream>>mapSecId;
            stream>>mapGroupId;
            stream>>mapGroupName;
            stream>>mapGroupColor;
            map1[mapSecId]=IGroupId(mapGroupId,mapGroupName,mapGroupColor);
        }
        std::unordered_map<IGroupId, DataId, GroupHash> map2;
        stream>>mapSize;
        for(unsigned int j=0;j<mapSize;j++)
        {
            DataId mapSecId;
            int mapGroupId;
            QString mapGroupName;
            QColor mapGroupColor;
            stream>>mapGroupId;
            stream>>mapGroupName;
            stream>>mapGroupColor;
            stream>>mapSecId;
            map2[IGroupId(mapGroupId,mapGroupName,mapGroupColor)]=mapSecId;
        }
        DataId stepId=RStep::create(name);
        ROutlinerData::StepList::add(stepId);
        RStep::changeId(stepId,id);
        NrtlManager::getStep(id)->section_group_map=map1;
        NrtlManager::getStep(id)->group_section_map=map2;
        RStep::MeshCut::set(id,meshCutId);
        RStep::SectionList::set(id,meshSectionIds);
        RStep::changeStepIdCounter(id+1);
    }
    DataId mainMeshId;
    stream>>mainMeshId;
    ROutlinerData::MainMesh::set(mainMeshId);
    DataId workingStep;
    stream>>workingStep;
    ROutlinerData::WorkingStep::set(workingStep);
    stream>>size;
    DataId tractIdCouter;
    stream>>tractIdCouter;
    RTractM::setIdCounter(tractIdCouter);
    std::vector<ITract*> tractLst;
    for(unsigned int i=0;i<size;i++)
    {
        ITract* tr=new ITract();
        stream>>tr->_tr_id;
        stream>>tr->_name;
        stream>>tr->vis;
        IGroupId group;
        stream>>group._id;
        stream>>group._color;
        stream>>group._name;
        tr->groupId=group;
        unsigned int length;
        stream>>length;
        for(unsigned int j=0;j<length;j++)
        {
            DataId secId;
            stream>>secId;
            tr->secLst.push_back(secId);
        }
        stream>>length;
        for(unsigned int j=0;j<length;j++)
        {
            DataId surfId;
            stream>>surfId;
            tr->latSurfLst.push_back(surfId);
        }
        tractLst.push_back(tr);
    }
    RTractM::setTractLst(tractLst);
    file.close();
    return true;
}
