#pragma once
#include "Niflect/NiflectAccessor.h"

namespace Niflect
{
	class CArrayAccessor : public CNiflectAccessor
	{
	public:
		//ר�ŷ�װ Cast ��Ϊ��ʾ������Ϊ���֧�ŵ�����, ��Ҫ�� Cast
		static CArrayAccessor* CastChecked(Niflect::CNiflectAccessor* base)
		{
			return dynamic_cast<CArrayAccessor*>(base);
		}
		static const CArrayAccessor* CastChecked(const Niflect::CNiflectAccessor* base)
		{
			return dynamic_cast<const CArrayAccessor*>(base);
		}
	};

	template <typename TArrayType>
	class TArrayAccessor : public CArrayAccessor
	{
	public:
		virtual bool SaveInstanceImpl(const InstanceType* base, CRwNode* rw) const override
		{
			auto& instance = *static_cast<const TArrayType*>(base);
			ASSERT(!rw->IsArray());
			auto rwArray = rw->ToArray();
			auto elemType = this->GetElementType();
			for (auto idx = 0; idx < instance.size(); ++idx)
			{
				auto rwItem = CreateRwNode();
				auto elemBase = &instance[idx];//��std::vector<bool>�޷�֧��, ��˶��ⶨ���ػ�ģ�� GetElementBaseToX, Ҳ�ɸ���std::vector<uint8>, ������Accessor
				if (elemType->SaveInstanceToRwNode(elemBase, rwItem.Get()))
					rwArray->AddItem(rwItem);
			}
			return true;
		}
		virtual bool LoadInstanceImpl(InstanceType* base, const CRwNode* rw) const override
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
				elemType->LoadInstanceFromRwNode(elemBase, rwItem);
			}
			return true;
		}
		virtual bool BuildInstanceNodeImpl(CNiflectInstanceNode* node) const override
		{
			auto& instance = *static_cast<TArrayType*>(node->GetBase());
			auto elemType = this->GetElementType();
			uint32 idx = 0;
			for (auto& it : instance)
			{
				auto elemNode = CreateInstanceNode();
				auto elemBase = &it;
				if (elemNode->InitAndBuild(elemType, elemBase, std::to_string(idx++).c_str(), node))
					node->AddNode(elemNode);
			}
			return true;
		}
	};

	class CBitsArrayAccessor : public CNiflectAccessor
	{
	public:
		//ר�ŷ�װ Cast ��Ϊ��ʾ������Ϊ���֧�ŵ�����, ��Ҫ�� Cast
		static CBitsArrayAccessor* CastChecked(CNiflectAccessor* base)
		{
			return dynamic_cast<CBitsArrayAccessor*>(base);
		}
		static const CBitsArrayAccessor* CastChecked(const CNiflectAccessor* base)
		{
			return dynamic_cast<const CBitsArrayAccessor*>(base);
		}
	};

	//todo: Bits Array Ӧ��Ϊ�������л�, �粻��Ҫ�����������, �ò�ͬ��С����������, ��ʡ�ռ�
	template <typename TBitsArrayType>
	class TBitsArrayAccessor : public CBitsArrayAccessor
	{
	public:
		virtual bool SaveInstanceImpl(const InstanceType* base, CRwNode* rw) const override
		{
			auto& instance = *static_cast<const TBitsArrayType*>(base);
			ASSERT(!rw->IsArray());
			auto rwArray = rw->ToArray();
			for (auto idx = 0; idx < instance.size(); ++idx)
				rwArray->AddItemBool(instance[idx]);
			return true;
		}
		virtual bool LoadInstanceImpl(InstanceType* base, const CRwNode* rw) const override
		{
			auto& instance = *static_cast<TBitsArrayType*>(base);
			ASSERT(rw->IsArray());
			auto rwArray = rw->GetArray();
			instance.resize(GetRwItemsCount(rwArray));
			for (auto idx = 0; idx < instance.size(); ++idx)
			{
				auto rwItem = rwArray->GetItem(idx)->ToValue();
				instance[idx] = rwItem->GetBool();
			}
			return true;
		}
	};

	class CMapAccessor : public CNiflectAccessor
	{
	public:
		//ר�ŷ�װ Cast ��Ϊ��ʾ������Ϊ���֧�ŵ�����, ��Ҫ�� Cast
		static CMapAccessor* CastChecked(CNiflectAccessor* base)
		{
			return dynamic_cast<CMapAccessor*>(base);
		}
		static const CMapAccessor* CastChecked(const CNiflectAccessor* base)
		{
			return dynamic_cast<const CMapAccessor*>(base);
		}
	};

	template <typename TMapType>
	class TMapAccessor : public CMapAccessor
	{
		using TElem = typename TMapType::allocator_type::value_type;
	public:
		virtual bool SaveInstanceImpl(const InstanceType* base, CRwNode* rw) const override
		{
			auto& instance = *static_cast<const TMapType*>(base);
			ASSERT(!rw->IsArray());
			auto rwArray = rw->ToArray();
			auto elemType = this->GetElementType();
			for (auto& it : instance)
			{
				auto rwItem = CreateRwNode();
				auto elemBase = &it;
				if (elemType->SaveInstanceToRwNode(elemBase, rwItem.Get()))
					rwArray->AddItem(rwItem);
			}
			return true;
		}
		virtual bool LoadInstanceImpl(InstanceType* base, const CRwNode* rw) const override
		{
			auto& instance = *static_cast<TMapType*>(base);
			ASSERT(rw->IsArray());
			auto rwArray = rw->GetArray();
			auto elemType = this->GetElementType();
			auto cnt = rwArray->GetItemsCount();
			for (uint32 idx = 0; idx < cnt; ++idx)
			{
				auto rwItem = rwArray->GetItem(idx);
				TElem item;
				auto elemBase = &item;
				if (elemType->LoadInstanceFromRwNode(elemBase, rwItem))
					instance.insert(item);
			}
			return true;
		}
		virtual bool BuildInstanceNodeImpl(CNiflectInstanceNode* node) const override
		{
			auto& instance = *static_cast<TMapType*>(node->GetBase());
			auto elemType = this->GetElementType();
			uint32 idx = 0;
			for (auto& it : instance)
			{
				auto elemBase = &it;
				auto elemNode = CreateInstanceNode();
				if (elemNode->InitAndBuild(elemType, elemBase, std::to_string(idx++).c_str(), node))
					node->AddNode(elemNode);
			}
			return true;
		}
	};
}