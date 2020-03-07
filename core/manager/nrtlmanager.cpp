#include "nrtlmanager.h"

NrtlModel* NrtlManager::model = nullptr;
std::vector<IStep*>NrtlManager::stepLst = {};
DataId NrtlManager::stepIdCounter = 1;

std::vector<IGroupId> NrtlManager::groupLst = {};
int NrtlManager::groupIdCounter = 1;

ResourceList NrtlManager::sectionsToHide = {};


TransactionSM::TransactionState TransactionSM::t_state = TransactionSM::TransactionState::WAIT;
TransactionSM::TransactionType TransactionSM::t_type = TransactionSM::TransactionType::SYNC;

void NrtlManager::restart()
{
    model = new NrtlModel();
    stepLst.clear();
    stepIdCounter = 1;
    groupLst.clear();
    groupIdCounter = 1;
}

void NrtlManager::createTransaction(TransactionType tp)
{
	setState(RUN);
	setType(tp);
};


void NrtlManager::commitTransaction()
{
    setState(WAIT);
    setType(SYNC); //default
};


void NrtlManager::initModel(NrtlModel* md)
{
    model = md;
}


IStep* NrtlManager::getStep(DataId id)
{
    auto it = stepLst.begin();
    for(; it != stepLst.end(); ++it)
    {
        if((*it)->stepId == id)
            break;
    }
    if(it == stepLst.end())
        return nullptr;
    else return (*it);
}
























