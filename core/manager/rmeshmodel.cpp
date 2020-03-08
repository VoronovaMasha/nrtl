#include "nrtlmanager.h"
#include "MeshAlgorithm.h"
DataId RMeshModel::create(MeshModel* obj)
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

bool RMeshModel::deleteMesh(DataId id)
{
    delete model->meshData.getElement(id);
    auto er_sec =  std::find(sectionsToHide.begin(),
                   sectionsToHide.end(),
                   id);
    if(er_sec != sectionsToHide.end()) sectionsToHide.erase(er_sec);
    return model->meshData.removeById(id);
}

bool RMeshModel::align(QVector<QVector3D> mesh_points, QVector<QVector3D> dst_points,DataId mesh_id, DataId dst_id)
{
    QMatrix4x4 align_mat = AlignMesh::getAlignMatrix(mesh_points, dst_points);
    align_mat = RMeshModel::ModelMatrix::get(mesh_id)*align_mat;
    RMeshModel::ModelMatrix::set(dst_id, align_mat);

    return true;
}

bool RMeshModel::Name::set(DataId mesh_id, const QString& name)
{
    model->meshData.getElement(mesh_id)->setName(name);
    return true;
}
QString RMeshModel::Name::get(DataId mesh_id)
{
    return model->meshData.getElement(mesh_id)->name();
}


bool RMeshModel::Transperancy::set(DataId mesh_id, uint8_t val)
{
    if(val==100)
    {
        model->meshData.makeLast(mesh_id);
    }
    else
    {
        model->meshData.makeFirst(mesh_id);
    }
    model->meshData.getElement(mesh_id)->setTransperancy(val);
    return true;
}
uint8_t RMeshModel::Transperancy::get(DataId mesh_id)
{
    return model->meshData.getElement(mesh_id)->transperancy();
}


void RMeshModel::Visibility::makeVisibleOnlyOne(DataId mesh_id)
{
    model->meshData.makeVisibleOnlyOne(mesh_id);
}

void RMeshModel::Visibility::makeAllUnvisible()
{
    model->meshData.makeAllUnvisible();
}

bool RMeshModel::Visibility::set(DataId mesh_id, bool vis)
{
    model->meshData.setVisFlag(model->meshData.getElement(mesh_id), vis);
    return true;
}

bool RMeshModel::Visibility::get(DataId mesh_id)
{
    return model->meshData.isVisible(model->meshData.getElement(mesh_id));
}

bool RMeshModel::ModelMatrix::set(DataId mesh_id, QMatrix4x4 mt)
{
    model->meshData.getElement(mesh_id)->setModelMatrix(mt);
    return true;
}

QMatrix4x4 RMeshModel::ModelMatrix::get(DataId mesh_id)
{
    return model->meshData.getElement(mesh_id)->getModelMatrix();
}

DataId RMeshModel::Step::get(DataId mesh_id)
{
    return model->meshData.getElement(mesh_id)->step();
}

bool RMeshModel::Step::set(DataId mesh,DataId step_id)
{
    model->meshData.getElement(mesh)->setStep(step_id);
    return true;
}
