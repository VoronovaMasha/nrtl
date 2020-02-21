#include "outliner.h"


IStep::IStep(const QString& name) :
    stepName(name), meshCutId(NONE)
{
    meshCutId = NONE;
}	

OutlinerData::OutlinerData()
{
    workingStep = NONE;
}

OutlinerData::~OutlinerData()
{
}

void OutlinerData::addStep(DataId stp)
{
    stepVec.push_back(stp);
    workingStep = stepVec[stepVec.size() - 1];
}


bool OutlinerData::deleteStep(DataId id)
{
    auto it = stepVec.begin();
    for(; it != stepVec.end(); ++it)
    {
        if((*it) == id)
            break;
    }
    stepVec.erase(it);
    workingStep = NONE;
    return true;
}
/*bool OutlinerData::deleteStep(Step* st)
{
	for (int i = 0; i<stepVec.size(); i++)
		if (stepVec[i] == st)
		{
            workingStep = OutlinerData::NONE;
			delete st;
			stepVec.erase(stepVec.begin() + i);
			return true;
		}
	return false;
}
bool OutlinerData::deleteStep(int idx)
{
	if (idx < stepVec.size() && idx >= 0)
	{
        workingStep = OutlinerData::NONE;
		delete stepVec[idx];
		stepVec.erase(stepVec.begin() + idx);
		return true;
	}
	return false;
}

bool OutlinerData::clearDataStructure()
{
	for (int i = 0; i<stepVec.size(); i++)
		delete stepVec[i];
	stepVec.clear();
    workingStep = OutlinerData::NONE;
	return true;
}

Step* OutlinerData::at(int idx) const
{
	return stepVec.at(idx);
}

Step* OutlinerData::operator[](int idx)
{
	return stepVec.at(idx);
}

const Step* OutlinerData::operator[](int idx) const
{
	return stepVec.at(idx);
}


size_t OutlinerData::size()
{
	return stepVec.size();
}


int OutlinerData::stepIdByStepPtr(Step* stp) const
{
	for (int i = 0; i<stepVec.size(); i++)
	{
		if (stepVec[i] == stp)
			return i;
	}

    return OutlinerData::NONE;
}


Step* OutlinerData::getWorkingStep()
{
	if (workingStep >= 0)
	{
		return stepVec[workingStep];
	}
	else return nullptr;
}

void OutlinerData::setWorkingStep(Step* stp)
{
	workingStep = stepIdByStepPtr(stp);
}

GroupId OutlinerData::newGroup(int color)
{
	GroupId gr(groupIdCounter, color);
	groups.push_back(gr);
	groupIdCounter++;
	return groups.at(groups.size() - 1);
}
*/
