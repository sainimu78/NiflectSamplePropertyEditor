#pragma once
#include "Niflect/Memory/AllocatorInterface.h"
#include "Niflect/NiflectBase.h"
#include "Niflect/Memory/Generic/GenericHeapAllocator.h"

namespace Niflect
{
	class CDefaultMemoryPoolScope : public IAllocatorInterface
	{
		struct SChunk
		{
			void* m_mem;
			size_t m_sizeUsaed;
		};
	public:
		CDefaultMemoryPoolScope()
			: m_chunkSize(32)//������
		{
			//��Ҫ����Ƕ�׵����?
			CMemory::SetCurrentAllocator(this);
		}
		~CDefaultMemoryPoolScope()
		{
			CMemory::SetCurrentAllocator(NULL);
			for (auto& it : m_vecChunk)
				Niflect::CDefaultMemory::Free(it.m_mem);
		}
		virtual void* Alloc(size_t size) override
		{
			size_t sizeUsed = 0;
			if (m_vecChunk.size() > 0)
				sizeUsed = m_vecChunk.back().m_sizeUsaed;

			{
				auto maxChunkSize = sizeUsed > m_chunkSize ? sizeUsed : m_chunkSize;
				if (maxChunkSize - sizeUsed < size || m_vecChunk.size() == 0)
					m_vecChunk.push_back({ Niflect::CDefaultMemory::Alloc(size > m_chunkSize ? size : m_chunkSize), 0 });
			}

			auto& chunk = m_vecChunk.back();
			auto addr = static_cast<char*>(chunk.m_mem) + chunk.m_sizeUsaed;
			chunk.m_sizeUsaed += size;
			return addr;
		}
		virtual void* Realloc(void* ptr, size_t size) override
		{
			return this->Alloc(size);
		}
		virtual void Free(void* ptr) override
		{
		}

	private:
		//��������, ��Ϊ�ı���ȫ�� CMemory �� allocator, ��������̬��Ա���� CMemory �ķ��亯��, ������Ƕ��ִ�б���� Alloc
		//���ʹ�� DefaultMemory, ��Ȼʹ�� CRT �� std::vector ��Ĭ�� Allocator ���ǿ��е�, ֻҪ���� CMemory �ķ��伴��
		std::vector<SChunk, TGenericHeapAllocator<SChunk, CDefaultMemory> > m_vecChunk;
		const uint32 m_chunkSize;
	};
}