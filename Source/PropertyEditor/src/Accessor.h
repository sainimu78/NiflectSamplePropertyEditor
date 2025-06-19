#pragma once
#include "Niflect/Component/RwTree/RwAccessor.h"
#include "Niflect/NiflectType.h"

class CAccessor : public Niflect::CRwAccessor
{
protected:
	using CRwInstanceNode = Niflect::CRwInstanceNode;
	using CField = Niflect::CField;
public:
	virtual Niflect::CNiflectType* GetType() const = 0;

//public:
//	static CAccessor* Cast(Niflect::CNiflectAccessor* base)
//	{
//		ASSERT(dynamic_cast<CAccessor*>(base) != NULL);
//		return static_cast<CAccessor*>(base);
//	}
};

static CAccessor* GetAccessor(Niflect::CNiflectType* ownerType)
{
	auto accessor = ownerType->GetDerivedAccessor<CAccessor>();
	return accessor;
}
static Niflect::CNiflectType* GetAccessorType(Niflect::CNiflectType* ownerType)
{
	auto accessor = GetAccessor(ownerType);
	return accessor->GetType();
}