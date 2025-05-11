#pragma once
#include "Niflect/Base/Memory.h"
#include <vector>

namespace Niflect
{
	template <typename TElement, typename TAllocator>
	using TSTLVector = std::vector<TElement, TAllocator>;

	template <typename TElement>
	using TArray = TSTLVector<TElement, THeapAllocator<TElement> >;
	template <typename TElement>
	using TArrayNif = TSTLVector<TElement, THeapAllocator<TElement> >;

	//����, Ӧ���Ƕ�����������ļ�
	//template <typename TKey, typename TCompare = std::less<TKey>, typename TAllocator = THeapAllocator<TKey> >
	//using TSortedArray = StlCompliantType2::TSortedVector<TKey, TCompare, TAllocator>;
}