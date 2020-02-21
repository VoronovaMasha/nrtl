#include "nrtlmodel.h"
NrtlModel::NrtlModel()
{
    mainMeshId=NONE;
    tractIdCounter = 1;
}

void NrtlModel::setMainMeshId(DataId id)
{
    mainMeshId = id;
}

void NrtlModel::resetMainMesh()
{
    mainMeshId = NONE;
}

DataId NrtlModel::getMainMeshId()
{
    return mainMeshId;
}
