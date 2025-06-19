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
	virtual bool SaveImpl(const InstanceType* base, CRwNode* rw) const override
	{
		for (auto& it : this->GetOwnerType()->GetFields())
		{
			NIFLECT_ASSERT(!it.GetName().empty());
			auto rwField = CreateRwNode();
			if (SaveInstanceToRwNode(it.GetType(), it.GetAddr(base), rwField.Get()))
				AddExistingRwNode(rw, it.GetName(), rwField);
		}
		return true;
	}
	virtual bool LoadImpl(InstanceType* base, const CRwNode* rw) const override
	{
		for (auto& it : this->GetOwnerType()->GetFields())
		{
			NIFLECT_ASSERT(!it.GetName().empty());
			if (auto rwField = FindRwNode(rw, it.GetName()))
				LoadInstanceFromRwNode(it.GetType(), it.GetAddr(base), rwField);
		}
		return true;
	}
	virtual bool BuildInstanceNodeImpl(CRwInstanceNode* node) const override
	{
		for (auto& it : this->GetOwnerType()->GetFields())
		{
			NIFLECT_ASSERT(!it.GetName().empty());
			auto fieldNode = Niflect::CreateInstanceNode();
			if (this->FieldBuildInstanceNode(it, node, fieldNode.Get()))
				node->AddNode(fieldNode);
		}
		return true;
	}

private:
	bool FieldBuildInstanceNode(CField& field, CRwInstanceNode* parent, CRwInstanceNode* node) const
	{
		return node->InitAndBuild(field.GetType(), field.GetAddr(parent->GetBase()), field.GetName(), parent);
	}
};