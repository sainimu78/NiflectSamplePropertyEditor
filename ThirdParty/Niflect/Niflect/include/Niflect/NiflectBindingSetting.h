//todo: �ƻ��ļ�����Ϊ NiflectAccessorBinding.h
#pragma once

#define NIFLECTGENINTERNAL_COMBINE_INNER(A,B,C) A##B##C
#define NIFLECTGENINTERNAL_COMBINE(A,B,C) NIFLECTGENINTERNAL_COMBINE_INNER(A,B,C)

#define NIF_BS() using NIFLECTGENINTERNAL_COMBINE(__NiflectGen_BindingSetting,_,__LINE__) = 

namespace Niflect
{
	using BindingTypePlaceholder = void*;

	template <typename TAccessorType, typename TBindingType, typename TNextBindingType = BindingTypePlaceholder>
	class TBindingSetting//todo: �ƻ�����Ϊ TAccessorBinding
	{
	};
}