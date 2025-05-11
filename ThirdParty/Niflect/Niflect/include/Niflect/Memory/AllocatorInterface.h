#pragma once
#include <cstddef>

namespace Niflect
{
	class IAllocatorInterface
	{
	public:
		virtual void* Alloc(size_t size) = 0;
		virtual void* Realloc(void* ptr, size_t size) = 0;
		virtual void Free(void* ptr) = 0;
	};
}