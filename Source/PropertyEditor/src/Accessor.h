#pragma once
#include "Niflect/Default/Accessor.h"
#include "Niflect/NiflectType.h"

class CAccessor : public Niflect::CAccessor
{
protected:
	using CInstanceNode = Niflect::CInstanceNode;
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