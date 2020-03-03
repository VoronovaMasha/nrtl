#include "nrtlmanager.h"
#include "MeshAlgorithm.h"

QString RTractM::LtSurface::_error_string = "Error";
DataId RTractM::create(IGroupId gid)
{
    ITract* tr = new ITract();
    tr->_name = gid._name;
    tr->groupId = gid;
    for(auto stp : ROutlinerData::StepList::get())
    {
        IStep* pstep = getStep(stp);
        if(pstep->group_section_map[gid] != NONE)
        {
            tr->secLst.push_back(pstep->group_section_map[gid]);
        }
    }
    model->tractLst.push_back(tr);
    tr->_tr_id = model->tractIdCounter++;

    return tr->_tr_id;
}
bool RTractM::remove(DataId tr_id)
{
    for(size_t i = 0; i < model->tractLst.size(); ++i)
    {
        if(model->tractLst[i]->_tr_id == tr_id)
        {
            delete model->tractLst[i];
            model->tractLst.erase(model->tractLst.begin() + i);
            return true;
        }
    }
    return false;
}
std::vector<ITract*> RTractM::getTractLst()
{
    return model->tractLst;
}
bool RTractM::setTractLst(std::vector<ITract*> newTractLst)
{
    model->tractLst=newTractLst;
    return true;
}
DataId RTractM::getIdCounter()
{
    return model->tractIdCounter;
}
bool RTractM::setIdCounter(DataId newIdCounter)
{
    model->tractIdCounter=newIdCounter;
    return true;
}
bool RTractM::Name::set(DataId tr_id, const QString& name)
{
    for(size_t i = 0; i < model->tractLst.size(); ++i)
    {
        if(model->tractLst[i]->_tr_id == tr_id)
        {
            model->tractLst[i]->_name = name;
            return true;
        }
    }
    return false;
}
QString RTractM::Name::get(DataId tr_id)
{
    for(size_t i = 0; i < model->tractLst.size(); ++i)
    {
        if(model->tractLst[i]->_tr_id == tr_id)
        {
            return model->tractLst[i]->_name;
        }
    }
    return QString("No such tract");
}
bool RTractM::Visibility::set(DataId tr_id, bool vis)
{
    ITract* tract = nullptr;

    for(size_t i = 0; i < model->tractLst.size(); ++i)
    {
        if(model->tractLst[i]->_tr_id == tr_id)
        {
            tract = model->tractLst[i];
        }
    }
    if(tract != nullptr)
    {
        for(auto sec : tract->secLst)
        {
            RSectionModel::Visibility::set(sec, vis);
        }
        for(auto surf : tract->latSurfLst)
        {
            RMeshModel::Visibility::set(surf, vis);
        }
        tract->vis = vis;
        return true;
    }
    return false;
}
bool RTractM::Visibility::get(DataId tr_id)
{
    ITract* tract = nullptr;

    for(size_t i = 0; i < model->tractLst.size(); ++i)
    {
        if(model->tractLst[i]->_tr_id == tr_id)
        {
            tract = model->tractLst[i];
        }
    }

    if(tract != nullptr)
        return tract->vis;
    return false;
}


bool RTractM::LtSurface::create(DataId tr_id)
{
    ITract* tract = nullptr;
    for(auto tr : model->tractLst)
    {
        if(tr->_tr_id == tr_id)
        {
            tract = tr;
            break;
        }
    }

    if(tract == nullptr)
    {
        _error_string = "There is no such tract";
        return false;
    }

    if(tract->secLst.size() < 2)
    {
        _error_string = "Add more sections";
        return false;
    }

    for(DataId id : tract->latSurfLst)
    {
        RMeshModel::deleteMesh(id);
    }

    for(int i = 0; i < tract->secLst.size()-1; i++)
    {
        DataId sec_id = tract->secLst[i];
        DataId sec_id_next = tract->secLst[i + 1];
        if(RSectionModel::Border::get(sec_id).size()==0)
        {
            PolygonMatrix m = MeshTopology::makePolygonMatrix(model->meshData.getElement(sec_id));
            RSectionModel::MatrixOfPolygons::set(sec_id, m);
            MeshBorder b = MeshTopology::makeBorder(model->meshData.getElement(sec_id));
            if(b.size()==0)
            {
                _error_string = MeshTopology::errorString();
                return false;
            }
            else
                RSectionModel::Border::set(sec_id ,b);
        }
        if(RSectionModel::Border::get(sec_id_next).size()==0)
        {
            PolygonMatrix m = MeshTopology::makePolygonMatrix(model->meshData.getElement(sec_id_next));
            RSectionModel::MatrixOfPolygons::set(sec_id_next, m);
            MeshBorder b=MeshTopology::makeBorder(model->meshData.getElement(sec_id_next));
            if(b.size()==0)
            {
                _error_string = MeshTopology::errorString();
                return false;
            }
            else
                RSectionModel::Border::set(sec_id_next, b);
        }
        MeshModel* mesh=MeshTopology::makeSurface(MeshData::get().getElement(sec_id),
                                                  MeshData::get().getElement(sec_id_next));

        model->meshData.create(false) = mesh;
        DataId surf_id = model->meshData.getId(mesh);

        tract->latSurfLst.push_back(surf_id);

    }

    return true;
}

bool RTractM::LtSurface::Vis::set(DataId tr_id, bool vis)
{
    ITract* tract = nullptr;
    for(auto tr : model->tractLst)
    {
        if(tr->_tr_id == tr_id)
        {
            tract = tr;
            break;
        }
    }

    if(tract != nullptr)
    {
        for(DataId surf_id : tract->latSurfLst)
        {
            RMeshModel::Visibility::set(surf_id, vis);
        }
    }

    return (tract != nullptr);
}

