#ifndef NRTLMODEL_H
#define NRTLMODEL_H

#include "NrtlType.h"
#include "ivcontainer.h"
#include "outliner.h"
#include "meshmodel.h"

class NrtlManager;

struct ITract
{
    IGroupId groupId;
    RMeshData secLst, latSurfLst;
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
    IVContainer<MeshModel*> meshData;	
};

#endif // NRTLMODEL_H
