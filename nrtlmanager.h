#ifndef NRTLMANAGER_H
#define NRTLMANAGER_H

#include <string>
#include "nrtlmodel.h"

class TransactionSM
{
public:
	enum TransactionState { WAIT, RUN };
	enum TransactionType { SYNC, ASYNC };

	static TransactionState state() { return t_state; }
	static TransactionType type() { return t_type; }

protected:
	static void setState(TransactionState st)
	{
		t_state = st;
	}
	static void setType(TransactionType tp)
	{
		t_type = tp;
	}

private:
	static TransactionState t_state;
	static TransactionType t_type;
};

class NrtlManager : public TransactionSM
{
public:
	NrtlManager() = delete;

    static void initModel(NrtlModel* md);
	enum RemoveType { SIMPLE, CASCADE };

	static void createTransaction(TransactionType tp);
    static void commitTransaction();
    static void undo() {}
    static void redo() {}

    static QString errorString()
    {
        return QString("Error");
    }

protected:
	static NrtlModel* model;

    /*! \todo: переместить в модель */
    static std::vector<IStep*> stepLst;
    static DataId stepIdCounter;
    static IStep* getStep(DataId id);
    static std::vector<IGroupId> groupLst;
    static int groupIdCounter;
};

class MeshData : public NrtlManager
{
public:
    static RMeshData& get()
    {
        return model->meshData;
    }

    static ResourceList getMeshIdList()
    {
        return model->meshData.getResource();
    }
};

struct RMeshModel : public NrtlManager
{
    static DataId create(MeshModel* obj);
    /*! \warning: after deleting memory should be deallocated, but not directly */
    static bool deleteMesh(DataId id);
    static bool align(DataId mesh_id, DataId dst_id, QVector<QVector3D> mesh_points, QVector<QVector3D> dst_points);

    static MeshModel* getMeshData(DataId mesh_id)
    {
        return model->meshData.getElement(mesh_id);
    }
    class Name
    {
    public:
        static bool set(DataId mesh_id, const QString& name);
        static QString get(DataId mesh_id);
    };

    class Transperancy
    {
    public:
        static bool set(DataId mesh_id, uint8_t val);
        static uint8_t get(DataId mesh_id);
    };

    class Visibility
    {
    public:
        static bool set(DataId mesh_id, bool vis);
        static bool get(DataId mesh_id);
        static void makeVisibleOnlyOne(DataId mesh_id);
        static void makeAllUnvisible();
    };

    class ModelMatrix
    {
    public:
        static bool set(DataId mesh_id, QMatrix4x4 mt);
        static QMatrix4x4 get(DataId mesh_id);
    };

};



struct RSectionModel : public NrtlManager
{
    static DataId create(MeshModel* obj);

    class Name
    {
    public:
        static bool set(DataId mesh_id, const QString& name);
        static QString get(DataId mesh_id);
    };

    class Transperancy
    {
    public:
        static bool set(DataId mesh_id, uint8_t val);
        static uint8_t get(DataId mesh_id);
    };

    class Visibility
    {
    public:
        static bool set(DataId mesh_id, bool vis);
        static bool get(DataId mesh_id);
    };

    class ModelMatrix
    {
    public:
        static bool set(DataId mesh_id, QMatrix4x4 mt);
        static QMatrix4x4 get(DataId mesh_id);
    };

    class GroupId
    {
    public:
        static bool set(DataId section, IGroupId id);
        static IGroupId get(DataId mesh);
    };

    class Step
    {
    public:
        static DataId get(DataId section);
    };
};

typedef std::vector<DataId> RSectionList;
class RStep : public NrtlManager
{
public:
    static DataId create(const QString& name);
    static bool remove(DataId step_id);

    class Name
    {
    public:
        static bool set(DataId step_id, const QString& name);
        static QString get(DataId step_id);
    };

    class MeshCut
    {
    public:
        static bool set(DataId step_id, DataId mesh_id);
        static DataId get(DataId step_id);
    };

    class SectionList
    {
    public:
        static bool add(DataId step_id, DataId section_id);
        static bool remove(DataId step_id, DataId id);
        static RSectionList& get(DataId step_id);
    };
};


class GroupId : public NrtlManager
{
public:
    static IGroupId create(const QString& name, QColor clr);
    static bool remove(IGroupId);

    class Name
    {
    public:
        static bool set(IGroupId id, const QString& name);
        static QString get(IGroupId id);
    };

    class Color
    {
    public:
        static bool set(IGroupId id, QColor clr);
        static QColor get(IGroupId id);
    };
};


class ROutlinerData : public NrtlManager
{
public:
    class StepList
    {
    public:
        static bool add(DataId step_id);
        static bool remove(DataId step_id);
        static RStepList& get();
        static bool relocate(DataId step_id, int pos);
    };

    class GroupList
    {
    public:
        static bool add(IGroupId group);
        static bool remove(IGroupId group_id);
        static RGroupList& get();
    };

    class MainMesh
    {
    public:
        static bool set(DataId mesh_id);
        static DataId get();
    };

    class WorkingStep
    {
    public:
        static bool set(DataId step_id);
        static DataId get();
    };
};

struct RTractM : public NrtlManager
{
    static DataId create(IGroupId gid);
    static bool remove(DataId);
    static std::vector<DataId> get()
    {
        std::vector<DataId> id_vec;
        for(auto p_tr : model->tractLst)
        {
            id_vec.push_back(p_tr->_tr_id);
        }
        return id_vec;
    }

    class Name
    {
    public:
        static bool set(DataId tr_id, const QString& name);
        static QString get(DataId tr_id);
    };

    class Visibility
    {
    public:
        static bool set(DataId tr_id, bool vis);
        static bool get(DataId tr_id);
    };

};

#endif //NRTLMANAGER_H

