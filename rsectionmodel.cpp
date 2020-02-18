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
    _prev_section = stp->group_section_map.at(group_id);
    stp->section_group_map[_prev_section] = NONE;
    stp->group_section_map[_prev_group] = NONE;

    /*2. Построить новые */
    stp->section_group_map[section_id] = group_id;
    stp->group_section_map[group_id] = section_id;

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




