#pragma once
#include "Accessor.h"
#include "Accessor/ContainerAccessor_gen.h"

NIF_T()
class CArrayAccessor : public CAccessor
{
public:
	virtual Niflect::CNiflectType* GetType() const override { return Niflect::StaticGetType<CArrayAccessor>(); }
};

template <typename TArrayType>
class TArrayAccessor : public CArrayAccessor
{
protected:
	virtual bool SaveImpl(const InstanceType* base, CRwNode* rw) const override
	{
		auto& instance = *static_cast<const TArrayType*>(base);
		ASSERT(!rw->IsArray());
		auto rwArray = rw->ToArray();
		auto elemType = this->GetElementType();
		for (auto idx = 0; idx < instance.size(); ++idx)
		{
			auto rwItem = CreateRwNode();
			auto elemBase = &instance[idx];//如std::vector<bool>无法支持, 因此额外定义特化模板 GetElementBaseToX, 也可改用std::vector<uint8>, 或另定义Accessor
			if (SaveInstanceToRwNode(elemType, elemBase, rwItem.Get()))
				rwArray->AddItem(rwItem);
		}
		return true;
	}
	virtual bool LoadImpl(InstanceType* base, const CRwNode* rw) const override
	{
		auto& instance = *static_cast<TArrayType*>(base);
		ASSERT(rw->IsArray());
		auto rwArray = rw->GetArray();
		auto elemType = this->GetElementType();
		instance.resize(rwArray->GetItemsCount());
		for (auto idx = 0; idx < instance.size(); ++idx)
		{
			auto rwItem = rwArray->GetItem(idx);
			auto elemBase = &instance[idx];
			LoadInstanceFromRwNode(elemType, elemBase, rwItem);
		}
		return true;
	}
	virtual bool BuildInstanceNodeImpl(CRwInstanceNode* node) const override
	{
		auto& instance = *static_cast<TArrayType*>(node->GetBase());
		auto elemType = this->GetElementType();
		uint32 idx = 0;
		for (auto& it : instance)
		{
			auto elemNode = CreateRwInstanceNode();
			auto elemBase = &it;
			if (elemNode->InitAndBuild(elemType, elemBase, std::to_string(idx++).c_str(), node))
				node->AddNode(elemNode);
		}
		return true;
	}
};