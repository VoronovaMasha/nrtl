#ifndef IVCONTAINER_H
#define IVCONTAINER_H

#include <stdint.h>
#include <vector>
#include <iterator>
#include <initializer_list>
#include <type_traits>
#include "NrtlType.h"
template<typename DocType>
class IVContainerIter;

template<typename DocType>
class const_IVContainerIter;

template<typename DocType>
class IVContainer
{
public:
    friend class IVContainerIter<DocType>;
	friend class const_IVContainerIter<DocType>;

	IVContainer<DocType>()
	{
		static_assert(std::is_pointer<DocType>::value, "Expected a pointer");
		idCounter = 1;   // 0 stand for NONE in outliner etc.
        data        = {};
        dataVisMask = {};
        dataIdMask  = {};
    }

	IVContainer<DocType>(std::initializer_list<DocType> lst)
		: data(lst)
	{
		static_assert(std::is_pointer<DocType>::value, "Expected a pointer");
		idCounter = 1; // 0 stand for NONE in outliner etc.
        for (uint64_t i = 0; i < data.size(); i++)
		{
			dataVisMask.push_back(false);
			dataIdMask.push_back(idCounter++);
		}
    }

	void setVisFlag(const DocType& obj, bool vis);
	bool isVisible(const DocType& obj) const;

    void makeVisibleOnlyOne(DataId id);
    void makeAllUnvisible();

    void makeFirst(DataId id);
    void makeLast(DataId id);

    void changeId(DataId old_id,DataId new_id);
    void changeIdCounter(DataId new_id);

	DocType& create(bool visible=true);
	DocType& allocate(bool visible = true);

	bool remove(const DocType& obj);
    bool removeById(DataId id);
	
    DataId getId(const DocType& obj) const;
    DocType getElement(DataId id) const;

	typedef IVContainerIter<DocType> iterator;
	typedef const_IVContainerIter<DocType> const_iterator;

	iterator begin();
	iterator end();

	const_iterator begin() const;
	const_iterator end() const;

	DataId curId() { return idCounter; }

    const ResourceList& getResource() const
    {
        return dataIdMask;
    }

private:
	template<typename T>
    uint64_t find(std::vector<T> vec, const T& obj) const;
	DataId idCounter;

	std::vector<DocType> data;
    std::vector<bool>    dataVisMask;
    std::vector<DataId>  dataIdMask;

	struct IterObject
	{
		std::vector<DocType>* data;
		std::vector<bool>* dataVis;
        uint64_t index;
		bool operator==(const IterObject& other) const
		{
			return (index == other.index && data == other.data);
		}
		bool operator!=(const IterObject& other) const
		{
			return (index != other.index || data != other.data);
		}
        IterObject(std::vector<DocType>* d, std::vector<bool>* dv, uint64_t id)
		{
			data = d;
			dataVis = dv;
			index = id;
		}
	};

	struct const_IterObject
	{
		const std::vector<DocType>* data;
		const std::vector<bool>* dataVis;
        uint64_t index;
		bool operator==(const const_IterObject& other) const
		{
			return (index == other.index && data == other.data);
		}
		bool operator!=(const const_IterObject& other) const
		{
			return (index != other.index || data != other.data);
		}
        const_IterObject(const std::vector<DocType>* d, const std::vector<bool>* dv, const uint64_t id)
		{
			data = d;
			dataVis = dv;
			index = id;
		}
	};
};

template<typename DocType>
class IVContainerIter : public std::iterator<std::input_iterator_tag, DocType>
{
	friend class IVContainer<DocType>;
	IVContainerIter(typename IVContainer<DocType>::IterObject const& iter_obj);
public:
	IVContainerIter(const IVContainerIter& it);
	bool operator!=(IVContainerIter const& other) const;
	bool operator==(IVContainerIter const& other) const;
	DocType& operator*() const;
	IVContainerIter& operator++();
private:
	typename IVContainer<DocType>::IterObject curObj;
};

template<typename DocType>
class const_IVContainerIter : public std::iterator<std::input_iterator_tag, DocType>
{
	friend class IVContainer<DocType>;
	const_IVContainerIter(typename IVContainer<DocType>::const_IterObject const& iter_obj);
public:
	const_IVContainerIter(const const_IVContainerIter& it);
	bool operator!=(const_IVContainerIter const& other) const;
	bool operator==(const_IVContainerIter const& other) const;
	const DocType& operator*() const;
	const_IVContainerIter& operator++();
private:
	typename IVContainer<DocType>::const_IterObject curObj;
};

template<typename DocType>
template<typename T>
uint64_t IVContainer<DocType>::find(std::vector<T> vec, const T& obj) const
{
    for (uint64_t i = 0; i < vec.size(); i++)
	{
		if (obj == vec[i])
			return i;
	}
	return vec.size();
}

template<typename DocType>
void IVContainer<DocType>::setVisFlag(const DocType& obj, bool vis)
{
    uint64_t pos = find(data, obj);
	if (pos != data.size())
		dataVisMask[pos] = vis;
}

template<typename DocType>
bool IVContainer<DocType>::isVisible(const DocType& obj) const
{
    uint64_t pos = find(data, obj);
	if (pos != data.size())
		return dataVisMask[pos];
    return false;
}

template<typename DocType>
void IVContainer<DocType>::makeVisibleOnlyOne(DataId id)
{
    for (uint64_t i = 0; i < dataIdMask.size(); i++)
    {
        setVisFlag(data[i], dataIdMask[i]==id);
    }
}

template<typename DocType>
void IVContainer<DocType>::makeAllUnvisible()
{
    for (uint64_t i = 0; i < dataIdMask.size(); i++)
    {
        setVisFlag(data[i],false);
    }
}

template<typename DocType>
void IVContainer<DocType>::makeFirst(DataId id)
{
    uint64_t i=0;
    for (; i < dataIdMask.size(); i++)
    {
        if(dataIdMask[i]==id)
        {
            data.insert(data.begin(),data[i]);
            dataIdMask.insert(dataIdMask.begin(),dataIdMask[i]);
            dataVisMask.insert(dataVisMask.begin(),dataVisMask[i]);
            data.erase(data.begin()+i+1);
            dataIdMask.erase(dataIdMask.begin()+i+1);
            dataVisMask.erase(dataVisMask.begin()+i+1);
            break;
        }
    }
}

template<typename DocType>
void IVContainer<DocType>::makeLast(DataId id)
{
    uint64_t i=0;
    for (; i < dataIdMask.size(); i++)
    {
        if(dataIdMask[i]==id)
        {
            data.push_back(data[i]);
            dataIdMask.push_back(dataIdMask[i]);
            dataVisMask.push_back(dataVisMask[i]);
            data.erase(data.begin()+i);
            dataIdMask.erase(dataIdMask.begin()+i);
            dataVisMask.erase(dataVisMask.begin()+i);
            break;
        }
    }
}

template<typename DocType>
void IVContainer<DocType>::changeId(DataId old_id,DataId new_id)
{
    uint64_t i=0;
    for (; i < dataIdMask.size(); i++)
    {
        if(dataIdMask[i]==old_id)
        {
            dataIdMask[i]=new_id;
            break;
        }
    }
}

template<typename DocType>
void IVContainer<DocType>::changeIdCounter(DataId new_id)
{
    idCounter=new_id;
}

template<typename DocType>
DataId IVContainer<DocType>::getId(const DocType& obj) const
{
    uint64_t pos = find(data, obj);
	return dataIdMask[pos];
}

template<typename DocType>
DocType IVContainer<DocType>::getElement(DataId id) const
{
    uint64_t pos = find(dataIdMask, id);
    return data[pos];
}



template<typename DocType>
DocType& IVContainer<DocType>::create(bool visible)
{
	DocType val;
	data.push_back(val);
	dataVisMask.push_back(visible);
	dataIdMask.push_back(idCounter);
	idCounter++;

	return data.at(data.size() - 1);
}

template<typename DocType>
DocType& IVContainer<DocType>::allocate(bool visible)
{
	DocType val;
    val = new typename std::remove_pointer<DocType>::type();
	data.push_back(val);
	dataVisMask.push_back(visible);
	dataIdMask.push_back(idCounter);
	idCounter++;

	return data.at(data.size() - 1);
}

template<typename DocType>
bool IVContainer<DocType>::remove(const DocType& obj)
{
    const uint64_t pos = find(data, obj);
	if (pos == data.size())
		return false;
	data.erase(data.begin() + pos);
	dataVisMask.erase(dataVisMask.begin() + pos);
	dataIdMask.erase(dataIdMask.begin() + pos);
    return true;
}

template<typename DocType>
bool IVContainer<DocType>::removeById(DataId id)
{
    const uint64_t pos = find(dataIdMask, id);
    if (pos == data.size())
        return false;
    data.erase(data.begin() + pos);
    dataVisMask.erase(dataVisMask.begin() + pos);
    dataIdMask.erase(dataIdMask.begin() + pos);
    return true;
}

template<typename DocType>
typename IVContainer<DocType>::iterator IVContainer<DocType>::begin()
{
	return IVContainerIter<DocType>(IterObject{ &data, &dataVisMask, find(dataVisMask, true) });
}

template<typename DocType>
typename IVContainer<DocType>::iterator IVContainer<DocType>::end()
{
	return IVContainerIter<DocType>(IterObject{ &data, &dataVisMask, data.size() == 0 ? 0 : data.size() });
}

template<typename DocType>
typename IVContainer<DocType>::const_iterator IVContainer<DocType>::begin() const
{
	const_IterObject obj( &data, &dataVisMask, find(dataVisMask, true) );
	return const_iterator(obj);
}

template<typename DocType>
typename IVContainer<DocType>::const_iterator IVContainer<DocType>::end() const
{
	const_IterObject obj{ &data, &dataVisMask, data.size() == 0 ? 0 : data.size() };
	return const_iterator(obj);
}


template<typename DocType>
IVContainerIter<DocType>::IVContainerIter(typename IVContainer<DocType>::IterObject const& iter_obj) :
	curObj(iter_obj)
{
		
}

template<typename DocType>
IVContainerIter<DocType>::IVContainerIter(const IVContainerIter& it) :
	curObj(it.curObj)
{

}

template<typename DocType>
bool IVContainerIter<DocType>::operator!=(const IVContainerIter& other) const
{
	return curObj != other.curObj;
}

template<typename DocType>
bool IVContainerIter<DocType>::operator==(const IVContainerIter& other) const
{
	return curObj == other.curObj;
}

template<typename DocType>
DocType& IVContainerIter<DocType>::operator*() const
{
	return (*curObj.data)[curObj.index];
}

template<typename DocType>
IVContainerIter<DocType>& IVContainerIter<DocType>::operator++()
{
    uint64_t pos = curObj.index;
    for (uint64_t i = pos+1; i < (*curObj.data).size(); i++)
		if ((*curObj.dataVis)[i])
		{
			curObj.index = i;
			return *this;
		}

	curObj.index = (*curObj.data).size();
	return *this;
}

template<typename DocType>
const_IVContainerIter<DocType>::const_IVContainerIter(typename IVContainer<DocType>::const_IterObject const& iter_obj) :
	curObj(iter_obj)
{

}

template<typename DocType>
const_IVContainerIter<DocType>::const_IVContainerIter(const const_IVContainerIter& it) :
	curObj(it.curObj)
{

}

template<typename DocType>
bool const_IVContainerIter<DocType>::operator!=(const const_IVContainerIter& other) const
{
	return curObj != other.curObj;
}

template<typename DocType>
bool const_IVContainerIter<DocType>::operator==(const const_IVContainerIter& other) const
{
	return curObj == other.curObj;
}

template<typename DocType>
const DocType& const_IVContainerIter<DocType>::operator*() const
{
	return (*curObj.data)[curObj.index];
}

template<typename DocType>
const_IVContainerIter<DocType>& const_IVContainerIter<DocType>::operator++()
{
    uint64_t pos = curObj.index;
    for (uint64_t i = pos + 1; i < (*curObj.data).size(); i++)
		if ((*curObj.dataVis)[i])
		{
			curObj.index = i;
			return *this;
		}

	curObj.index = (*curObj.data).size();
	return *this;
}
#endif //ivcontainer.h
