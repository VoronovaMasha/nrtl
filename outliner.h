#ifndef OUTLINER_H
#define OUTLINER_H

#include <QString>
#include <stdint.h>
#include <vector>
#include "ivcontainer.h"
#include "NrtlType.h"
#include <unordered_map>
#include <unordered_set>

class DataHash
{
public:
    size_t operator()(DataId const& d) const
    {
        return d;
    }
};

class GroupHash
{
public:
    size_t operator()(IGroupId const& g) const
    {
        return g._id;
    }
};


struct IStep
{
    IStep() = delete;
    IStep(const QString& name);

    DataId stepId;
    QString stepName;
	DataId meshCutId;
	std::vector<DataId> meshSectionIds;
	std::vector<DataId> rasterIdList;
	std::vector<DataId> polyLineIdList;	
    std::unordered_map<DataId, IGroupId, DataHash> section_group_map;
    std::unordered_map<IGroupId, DataId, GroupHash> group_section_map;
};

struct OutlinerData
{
public:
    OutlinerData();
    ~OutlinerData();
    void addStep(DataId);
    bool deleteStep(DataId id);
/*	bool deleteStep(int idx);
	bool clearDataStructure();
    Step* at(int idx) const;
	Step* operator[](int idx);
	const Step* operator[](int idx) const;
	int stepIdByStepPtr(Step* stp) const;
	size_t size();
	Step* getWorkingStep();
	void setWorkingStep(Step* stp);

    GroupId newGroup(int color);*/
public:
    std::vector<DataId> stepVec;
    std::vector<IGroupId> groups;

    DataId workingStep;
	int groupIdCounter;
};
#endif //OUTLINER_H

