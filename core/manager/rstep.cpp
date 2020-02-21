#include "nrtlmanager.h"
#include <vector>
DataId RStep::create(const QString& name)
{
    IStep* stp = new IStep(name);
    stp->stepId = stepIdCounter;
    stepLst.push_back(stp);
    ++stepIdCounter;
    return stp->stepId;
}

bool RStep::remove(DataId step_id)
{
    model->outliner.deleteStep(step_id);
    auto it = stepLst.begin();
    for (; it != stepLst.end(); ++it)
    {
        if ((*it)->stepId == step_id)
            break;
    }
    delete (*it);
    stepLst.erase(it);
    return true;
}

bool RStep::Name::set(DataId step_id, const QString& name)
{
    getStep(step_id)->stepName = name;
    return true;
}

QString RStep::Name::get(DataId step_id)
{
    return getStep(step_id)->stepName;
}


bool RStep::MeshCut::set(DataId step_id, DataId mesh_id)
{
    getStep(step_id)->meshCutId = mesh_id;
    return true;
}
DataId RStep::MeshCut::get(DataId step_id)
{
    return getStep(step_id)->meshCutId;
}


bool RStep::SectionList::add(DataId step_id, DataId section_id)
{
    getStep(step_id)->meshSectionIds.push_back(section_id);
    getStep(step_id)->section_group_map[section_id] = NONE;
    model->meshData.getElement(section_id)->setStep(step_id);
    return true;
}
bool RStep::SectionList::remove(DataId step_id, DataId section_id)
{
    auto lst = getStep(step_id)->meshSectionIds;
    int i=0;
    for(;i<lst.size();i++)
    {
        if(lst[i]==section_id)
            break;
    }
    getStep(step_id)->meshSectionIds.erase(getStep(step_id)->meshSectionIds.begin()+i);
    auto _group = getStep(step_id)->section_group_map[section_id];
    getStep(step_id)->section_group_map.erase(section_id);
    getStep(step_id)->group_section_map[_group] = NONE;
    model->meshData.getElement(section_id)->setStep(NONE);
    return true;
}
RSectionList& RStep::SectionList::get(DataId step_id)
{
    return getStep(step_id)->meshSectionIds;
}
