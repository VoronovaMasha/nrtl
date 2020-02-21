#ifndef NRTLMODEL_H
#define NRTLMODEL_H

#include "NrtlType.h"
#include "ivcontainer.h"
#include "outliner.h"
#include "meshmodel.h"

class NrtlManager;

struct ITract
{
    QString _name;
    IGroupId groupId;
    DataId _tr_id;
    ResourceList secLst, latSurfLst;
};

class NrtlModel
{
public:
	friend class NrtlManager;
    NrtlModel();
    DataId getMainMeshId();

/*! \todo: Не должно быть public; прописать интерфейс */
public:
	void setMainMeshId(DataId id);
	void resetMainMesh();  //set to NONE
    DataId mainMeshId;
    OutlinerData outliner;

    std::vector<ITract*> tractLst;
    DataId tractIdCounter;

    IVContainer<MeshModel*> meshData;	
};

#endif // NRTLMODEL_H
