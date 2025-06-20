#pragma once
#include "Niflect/Component/RwTree/RwAccessor.h"
#include "Niflect/NiflectType.h"

using namespace RwTree;

class CAccessor : public CRwAccessor
{
protected:
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