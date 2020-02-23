#include "nrtlmanager.h"
#include "MeshAlgorithm.h"
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
        return true;
    }
    return false;
}
bool RTractM::Visibility::get(DataId tr_id)
{
    Q_UNUSED(tr_id)
    return true;
}

void RTractM::LtSurface::create()
{
    /*if(RSectionModel::Border::get(2).size()==0)
    {
        PolygonMatrix m=MeshTopology::makePolygonMatrix(MeshData::get().getElement(2));
        RSectionModel::MatrixOfPolygons::set(2,m);
        MeshBorder b=MeshTopology::makeBorder(MeshData::get().getElement(2));
        if(b.size()==0)
        {
            QMessageBox::warning(this, "Warning", MeshTopology::errorString());
        }
        else
            RSectionModel::Border::set(2,b);
    }
    if(RSectionModel::Border::get(3).size()==0)
    {
        PolygonMatrix m=MeshTopology::makePolygonMatrix(MeshData::get().getElement(3));
        RSectionModel::MatrixOfPolygons::set(3,m);
        MeshBorder b=MeshTopology::makeBorder(MeshData::get().getElement(3));
        if(b.size()==0)
        {
            QMessageBox::warning(this, "Warning", MeshTopology::errorString());
        }
        else
            RSectionModel::Border::set(3,b);
    }
    MeshModel* mesh=MeshTopology::makeSurface(MeshData::get().getElement(2),MeshData::get().getElement(3));
    if(mesh!=nullptr)
    {
        NrtlManager::createTransaction(NrtlManager::SYNC);
        outlinerWgt->addMainModel(mesh,"fsdc");
        NrtlManager::commitTransaction();
    }*/
}
