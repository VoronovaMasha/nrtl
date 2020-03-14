//подключение заголовочных файлов
#include "nrtlmanager.h"

bool ROutlinerData::StepList::add(DataId step_id)
    //сборка модели
{
    model->outliner.addStep(step_id); // 
    IStep* stp = getStep(step_id); //
    stp->group_section_map.clear();
    stp->section_group_map.clear();

    for(auto group : model->outliner.groups)
        
    {
        stp->group_section_map[group] = NONE;
    }

    return true;
}
bool ROutlinerData::StepList::remove(DataId step_id) //удаление идентификатора степа
{
    model->outliner.deleteStep(step_id);
    return true;
}
bool ROutlinerData::StepList::changeId(DataId old_step_id,DataId new_step_id) // изменение идентификатора существующего степа
{
    for(unsigned int i=0;i<model->outliner.stepVec.size();i++)
    {
        if(model->outliner.stepVec[i]==old_step_id)
        {
            model->outliner.stepVec[i] = new_step_id;
            break;
        }
    }
    return true;
}
RStepList& ROutlinerData::StepList::get() // новый список стэпов
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

bool ROutlinerData::GroupList::add(IGroupId group)//группировка сечений в степы  
{
    model->outliner.groups.push_back(group);
    for(auto stp : model->outliner.stepVec)
    {
        getStep(stp)->group_section_map[group] = NONE;
    }

    return true;
}

bool ROutlinerData::GroupList::changeIdCounter(int newIdCounter)// изменение идентификатора счетчика
{
    groupIdCounter=newIdCounter;
    return true;
}

bool ROutlinerData::GroupList::remove(IGroupId group_id)
{
    model->outliner.groups.erase(std::find(model->outliner.groups.begin(),
                                           model->outliner.groups.end(),
                                           group_id));
    for(auto stp : model->outliner.stepVec)
    {
        DataId s_id = getStep(stp)->group_section_map[group_id];
        getStep(stp)->group_section_map.erase(group_id);
        getStep(stp)->section_group_map[s_id] = IGroupId();
    }
    for(auto stp : model->outliner.stepVec)
    {
        auto map=getStep(stp)->section_group_map;
        for(auto it=map.begin();it!=map.end();it++)
        {
            if(it->second==group_id)
            {
                RSectionModel::GroupId::set(it->first,NONE);
                model->meshData.getElement(it->first)->setColor(QColor(0,0,0));
            }
        }
    }
    return true;
}
bool ROutlinerData::GroupList::setGroupLstInOutliner(std::vector<IGroupId> new_groupLst)
{
    groupLst=new_groupLst;
    return true;
}
bool ROutlinerData::GroupList::setGroupLstInManagers(std::vector<IGroupId> new_groupLst)
{
    model->outliner.groups=new_groupLst;
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
    RStepList& stpLst = ROutlinerData::StepList::get();
    for(DataId stp : stpLst)
    {
        DataId mh = RStep::MeshCut::get(stp);
        RMeshModel::Visibility::set(mh, false);
    }
    for(DataId sec : sectionsToHide)
    {
        RSectionModel::Visibility::set(sec, false);
    }

    DataId mh = RStep::MeshCut::get(step_id);
    RMeshModel::Visibility::set(mh, true);
    return true;
}

DataId ROutlinerData::WorkingStep::get()
{
    return model->outliner.workingStep;
}
