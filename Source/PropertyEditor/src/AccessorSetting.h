#pragma once
#include "Niflect/NiflectAccessorSetting.h"
#include "Accessor/CompoundAccessor.h"
#include "Accessor/ValueTypeAccessor.h"
#include "Accessor/ContainerAccessor.h"

using namespace NiflectAccessorSetting;

NIFAS_B() TSetting<CCompoundAccessor>;
NIFAS_A() TSetting<CFloatAccessor, float>;
template <typename TInstance, typename T>
NIFAS_A() TSetting<TArrayAccessor<TInstance>, Niflect::TArray<T> >;