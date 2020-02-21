#include "nrtlmanager.h"

bool ROutlinerData::StepList::add(DataId step_id)
{
    model->outliner.addStep(step_id);
    return true;
}
bool ROutlinerData::StepList::remove(DataId step_id)
{
    model->outliner.deleteStep(step_id);
    return true;
}
RStepList& ROutlinerData::StepList::get()
{
    return model->outliner.stepVec;
}

/*! \todo: доделать */
bool ROutlinerData::StepList::relocate(DataId step_id, int pos)
{
    Q_UNUSED(step_id)
    Q_UNUSED(pos)

    return true;
}

bool ROutlinerData::GroupList::add(IGroupId group)
{
    model->outliner.groups.push_back(group);
    for(auto stp : model->outliner.stepVec)
    {
        getStep(stp)->group_section_map[group] = NONE;
    }

    return true;
}

bool ROutlinerData::GroupList::remove(IGroupId group_id)
{
    model->outliner.groups.erase(std::find(model->outliner.groups.begin(),
                                           model->outliner.groups.end(),
                                           group_id));
    for(auto stp : model->outliner.stepVec)
    {
        getStep(stp)->group_section_map.erase(group_id);
    }

    return true;
}
RGroupList& ROutlinerData::GroupList::get()
{
    return model->outliner.groups;
}


bool ROutlinerData::MainMesh::set(DataId mesh_id)
{
    model->mainMeshId = mesh_id;
    return true;
}
DataId ROutlinerData::MainMesh::get()
{
    return model->mainMeshId;
}


bool ROutlinerData::WorkingStep::set(DataId step_id)
{
    model->outliner.workingStep = step_id;
    return true;
}

DataId ROutlinerData::WorkingStep::get()
{
    return model->outliner.workingStep;
}
