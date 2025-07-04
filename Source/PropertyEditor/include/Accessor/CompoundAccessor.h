#pragma once
#include "Accessor.h"
#include "Accessor/CompoundAccessor_gen.h"

using namespace RwTree;

NIF_T()
class CCompoundAccessor : public CAccessor
{
public:
	virtual Niflect::CNiflectType* GetType() const override { return Niflect::StaticGetType<CCompoundAccessor>(); }

protected:
	virtual bool SaveInstanceImpl(const InstanceType* base, CRwNode* rw) const override
	{
		for (auto& it : this->GetOwnerType()->GetFields())
		{
			ASSERT(!it.GetName().empty());
			auto rwField = CreateRwNode();
			if (it.TypeSaveInstanceToRwNode(base, rwField.Get()))
				AddExistingRwNode(rw, it.GetName(), rwField);
		}
		return true;
	}
	virtual bool LoadInstanceImpl(InstanceType* base, const CRwNode* rw) const override
	{
		for (auto& it : this->GetOwnerType()->GetFields())
		{
			ASSERT(!it.GetName().empty());
			auto rwChild = FindRwNode(rw, it.GetName());
			it.TypeLoadInstanceFromRwNode(base, rwChild);
		}
		return true;
	}
	virtual bool BuildInstanceNodeImpl(CNiflectInstanceNode* node) const override
	{
		for (auto& it : this->GetOwnerType()->GetFields())
		{
			ASSERT(!it.GetName().empty());
			auto fieldNode = Niflect::CreateInstanceNode();
			if (it.TypeBuildInstanceNode(node, fieldNode.Get()))
				node->AddNode(fieldNode);
		}
		return true;
	}
};