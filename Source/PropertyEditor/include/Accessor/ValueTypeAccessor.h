#pragma once
#include "Accessor.h"
#include "ExampleStructure.h"
#include "Accessor/ValueTypeAccessor_gen.h"

template <typename TValue>
class TValueTypeAccessor : public CAccessor
{
protected:
	virtual bool SaveImpl(const InstanceType* base, CRwNode* rw) const override
	{
		auto& instance = *static_cast<const TValue*>(base);
		auto rwValue = rw->ToValue();
		SetRwValueAs<TValue>(rwValue, instance);
		return true;
	}
	virtual bool LoadImpl(InstanceType* base, const CRwNode* rw) const override
	{
		auto& instance = *static_cast<TValue*>(base);
		ASSERT(rw->IsValue());
		auto rwValue = rw->GetValue();
		instance = GetRwValueAs<TValue>(rwValue);
		return true;
	}
};

NIF_T()
class CFloatAccessor : public TValueTypeAccessor<float>
{
public:
	virtual Niflect::CNiflectType* GetType() const override { return Niflect::StaticGetType<CFloatAccessor>(); }
};

NIF_T()
class CVector3Accessor : public CAccessor
{
protected:
	virtual bool SaveImpl(const InstanceType* base, CRwNode* rw) const override;
	virtual bool LoadImpl(InstanceType* base, const CRwNode* rw) const override;

public:
	virtual Niflect::CNiflectType* GetType() const override { return Niflect::StaticGetType<CVector3Accessor>(); }
};