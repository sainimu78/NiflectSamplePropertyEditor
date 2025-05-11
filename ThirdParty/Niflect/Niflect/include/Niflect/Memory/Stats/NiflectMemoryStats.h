#pragma once
#include "Niflect/NiflectCommon.h"

namespace Niflect
{
	//���ڴ�ͳ����Ҫ����:
	//1. ���㿪���׶�ȷ���ڴ洦����ȷ��
	//2. �鿴��ʵ�ַ������ڴ�����
	//3. ���Ӷ����ڴ�snapshot
	//����profiler�ȹ��ߵ�ԭ��:
	//1. ���ڵ��Թ�����ֱ�ӿ�ͳ�ƽ��
	//2. �����޸���Ҫͳ����Ϣ
	//3. �����һЩʵ�ֽ�������Բ���
	class CMemoryStats
	{
	public:
		CMemoryStats()
			: m_bytesRuntime(0)
			, m_bytesMemoryInfo(0)
			, m_allocCount(0)
			, m_freeCount(0)
			, m_allocatedBytesRuntimeTotal(0)
			, m_allocatedBytesTotal(0)
			, m_freedBytesRuntimeTotal(0)
			, m_freedBytesTotal(0)
		{

		}
		~CMemoryStats()
		{
		}
		void DebugCheck()
		{
			//����2023.08.21�Ľ���
			//����ȱ��, �޷���֤�ò���������ͷ�
			//�ƻ�ʵ��ȫ�ֵ���ר�ŵĹ������, �ɸù����������ͷ���Щ����, �Ա�֤�ͷ�˳��
			//2023.08.21, ֻ��Ҫ��֤�ò��Զ���������ִ���깹�캯��, ���ܱ�֤�������
			//���ַ�����ͷ�����Ĵ������ע������
			//��˽�����, ΪʹCMemoryTest����ִ���깹�캯��, ������̬��������Ҳ��Ҫ��CDefaultMemory, ����new
			//�����new, ����Ҫ��֤����newִ��������CDefaultMemory::Alloc
			//���õİ취��ͳһʹ��CDefaultMemory��, TSingletonPtrҲ֧��CDefaultMemory, CMemoryTest������ܿ�����new����
			ASSERT(m_bytesRuntime == 0);
			ASSERT(m_bytesMemoryInfo == 0);
			ASSERT(m_allocCount == m_freeCount);
			ASSERT(m_allocatedBytesRuntimeTotal == m_freedBytesRuntimeTotal);
			ASSERT(m_allocatedBytesTotal == m_freedBytesTotal);
			ASSERT(m_allocatedBytesTotal == 0 || m_allocatedBytesTotal > m_allocatedBytesRuntimeTotal);
			ASSERT(m_freedBytesTotal == 0 || m_freedBytesTotal > m_freedBytesRuntimeTotal);
		}
		size_t m_bytesRuntime;
		size_t m_bytesMemoryInfo;
		size_t m_allocCount;
		size_t m_freeCount;
		size_t m_allocatedBytesRuntimeTotal;
		size_t m_allocatedBytesTotal;
		size_t m_freedBytesRuntimeTotal;
		size_t m_freedBytesTotal;
	};

	template <typename TMemory>
	class TStackedStatsScope
	{
	public:
		TStackedStatsScope()
			: m_lastStats(NULL)
		{
			TMemory::PushStats(&m_stats, m_lastStats);
		}
		~TStackedStatsScope()
		{
			TMemory::PopStats(m_lastStats);
			m_stats.DebugCheck();
		}

	public:
		class CDisabled
		{
		public:
			CDisabled(TStackedStatsScope& parentScope)
				: m_parentScope(parentScope)
			{
				CMemoryStats* placeholder;
				TMemory::PushStats(m_parentScope.m_lastStats, placeholder);
				ASSERT(placeholder == &m_parentScope.m_stats);
			}
			~CDisabled()
			{
				TMemory::PopStats(&m_parentScope.m_stats);
			}

		private:
			TStackedStatsScope& m_parentScope;
		};

	private:
		CMemoryStats m_stats;
		CMemoryStats* m_lastStats;
	};
}