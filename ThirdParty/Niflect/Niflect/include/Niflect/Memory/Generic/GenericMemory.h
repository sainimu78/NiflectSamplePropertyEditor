#pragma once
#include "Niflect/Memory/Stats/NiflectMemoryStats.h"
#include <stdlib.h>//malloc, realloc, free

namespace Niflect
{	
	class CGenericMemory
	{
		//�ƻ�:
		//1.CDefaultMemory�ڴ����ȫ��ʵ��, CAlignedMemory�ɸ���
		//2.�����ڴ��, ��ʹ�����, ��������(���ù�ϵϵͳ)
		//3.�󶨷�����Ϣ, ͬ���ڴ�ѵ��ڷ�(ECS), ���cache����, ������Ҫר��һ�ַ��䷽ʽ��������ͬ���ڴ�ѵ�
		//4.�̶߳ѹ���, �߳�ID��pageӳ��, ��Ҫȷ�ϲ�ͬƽ̨���߳�ID�����ֵ�Ƿ�ͳ���ƽ̨��Windows��ͬС��65536
		//5.�����ڴ�ֲ����������Թ�˼����֤, ��Vector���ݺ�Ԥ���ռ�ɱ�����
		//6.�ڴ��Ż���������
		
	private:
		typedef uint64 BytesType;

	private:
		struct SMemoryInfo
		{
			BytesType m_size;
		};

		//begin, �ɸ���CMemoryTest������ͷ�����Ĵ���, ��Ҫ�����ȵ���CDefaultMemory::Alloc�ĺ����ڶ���static CMyTest g;
		//�����ȵ���CDefaultMemory::Alloc�ķ������¶ϵ�, ���ϵ�󼴿��ҵ����ȵ��õĺ���
		//class CDDDDDDDDDDD
		//{
		//};
		//class CASSSSSS
		//{
		//public:
		//	CASSSSSS()
		//	{
		//		//m_a = ClassMemory::New<CDDDDDDDDDDD, CDefaultMemory>();
		//	}
		//	~CASSSSSS()
		//	{
		//		printf("");
		//	}
		//	ClassMemory::TLiteSharedPointer<CDDDDDDDDDDD, CDefaultMemory> m_a;
		//};
		//class CMyTest
		//{
		//public:
		//	CMyTest()
		//	{
		//		static TSingletonPtr<CASSSSSS> g_holder(new CASSSSSS);
		//		static auto g_singleton = g_holder.Ref();
		//		g_singleton->m_a = ClassMemory::New<CDDDDDDDDDDD, CDefaultMemory>();
		//	}
		//	~CMyTest()
		//	{
		//		printf("");
		//	}
		//};
		//end

	private:
		inline static void* InternalMalloc(BytesType sz)
		{
			return malloc(sz);
		}
		inline static void* InternalRealloc(void* p, BytesType sz)
		{
			return realloc(p, sz);
		}
		inline static void InternalFree(void* p)
		{
			free(p);
		}

	public:
		static void* Alloc(BytesType size, CMemoryStats* stats)
		{
			const auto memInfoSize = sizeof(SMemoryInfo);
			auto actualSize = memInfoSize + size;
			auto base = InternalMalloc(actualSize);
			auto memInfo = static_cast<SMemoryInfo*>(base);
			memInfo->m_size = size;
			if (stats != NULL)
			{
				stats->m_bytesRuntime += memInfo->m_size;
				stats->m_bytesMemoryInfo += memInfoSize;
				stats->m_allocCount++;
				stats->m_allocatedBytesRuntimeTotal += memInfo->m_size;
				stats->m_allocatedBytesTotal += memInfoSize + memInfo->m_size;
			}
			auto mem = static_cast<char*>(base) + memInfoSize;
			return mem;
		}
		static void* Realloc(void* mem, BytesType size, CMemoryStats* stats)
		{
			if (mem == NULL)
				return Alloc(size, stats);
			ASSERT(size != 0);
			const auto memInfoSize = sizeof(SMemoryInfo);
			auto actualSize = memInfoSize + size;
			auto base = static_cast<char*>(mem) - memInfoSize;
			auto oldSize = reinterpret_cast<SMemoryInfo*>(base)->m_size;//todo: reallocӦ����ȷ�����ݲ���, ���Ӧ�ò��ر���, ȷ��������Ƴ�����
			auto newBase = InternalRealloc(base, actualSize);
			auto memInfo = static_cast<SMemoryInfo*>(newBase);
			memInfo->m_size = size;
			if (stats != NULL)
			{
				auto diff = oldSize - memInfo->m_size;
				if (oldSize > memInfo->m_size)
				{
					stats->m_freedBytesRuntimeTotal += diff;
					stats->m_freedBytesTotal += diff;
				}
				else
				{
					stats->m_allocatedBytesRuntimeTotal -= diff;
					stats->m_allocatedBytesTotal -= diff;
				}
				stats->m_bytesRuntime -= diff;
			}
			mem = static_cast<char*>(newBase) + memInfoSize;
			return mem;
		}
		static void Free(void* mem, CMemoryStats* stats)
		{
			if (mem == NULL)
				return;
			const auto memInfoSize = sizeof(SMemoryInfo);
			auto base = static_cast<char*>(mem) - memInfoSize;
			auto memInfo = reinterpret_cast<SMemoryInfo*>(base);
			if (stats != NULL)
			{
				ASSERT(stats->m_bytesRuntime == memInfo->m_size || stats->m_bytesRuntime > memInfo->m_size);
				stats->m_bytesRuntime -= memInfo->m_size;
				ASSERT(stats->m_bytesMemoryInfo == memInfoSize || stats->m_bytesMemoryInfo > memInfoSize);
				stats->m_bytesMemoryInfo -= memInfoSize;
				stats->m_freeCount++;
				stats->m_freedBytesRuntimeTotal += memInfo->m_size;
				stats->m_freedBytesTotal += memInfoSize + memInfo->m_size;
			}
			InternalFree(base);
		}
	};
}