#pragma once
#include "Niflect/NiflectAccessor.h"
#include "Niflect/NiflectType.h"

class CAccessor : public Niflect::CNiflectAccessor
{
public:
	virtual Niflect::CNiflectType* GetType() const = 0;

public:
	static CAccessor* Cast(Niflect::CNiflectAccessor* base)
	{
		ASSERT(dynamic_cast<CAccessor*>(base) != NULL);
		return static_cast<CAccessor*>(base);
	}
};

static Niflect::CNiflectType* GetAccessorType(Niflect::CNiflectType* ownerType)
{
	auto accessor = CAccessor::Cast(ownerType->GetAccessor());
	return accessor->GetType();
}