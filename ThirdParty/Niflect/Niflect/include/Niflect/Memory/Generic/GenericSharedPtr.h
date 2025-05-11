#pragma once
#include "Niflect/NiflectCommon.h"
#include "Niflect/Memory/Generic/GenericInstance.h"
//#define TEST_GENERIC_SHARED_PTR
#ifdef TEST_GENERIC_SHARED_PTR
#include <stdio.h>
#endif

//��ʹ��һ��bool�������洴����ԴΪMakeShared��MakeSharable, bool����������Ϊ4�ֽ�
//Ϊ���ٴ�΢С�ռ�ռ��, Ҳ�ɿ��ǽ� SGenericSharedPtrData ָ��Ϊ�Ƕ���ṹ
//��ͨ���ٶ�������ַ�Ƕ����, ����ʹ��, �������λ���洴����Դ
//��������ⷽ��������, ��ĳЩƽ̨�������������, �����ô˺��ʵ�ְ汾���뼴��
#define DETERMINE_CONSTRUCTED_FROM_MAKESHARED_OR_MAKESHARABLE_BY_A_BOOL

#ifdef DETERMINE_CONSTRUCTED_FROM_MAKESHARED_OR_MAKESHARABLE_BY_A_BOOL
#else
//#include <sstream>//��ӡDestructFunc������ַ
#endif

namespace Niflect
{
#ifdef TEST_GENERIC_SHARED_PTR
	namespace TestSharedPtr
	{
		static void TestReferences();
	}
#endif

	//�������ָ��
	//��Ҫ�ص�˵��:
	//1. ��ָ��Constructor��Destructor�ص�, ��֧�ַ���ϵͳʵ��������
	//2. ��Ҫ����κλ���̳�
	//3. ��ͨ��ģ�����TMemoryָ���ڴ�������
	//4. �ϸ�涨��rawָ�뽨������ֻ��ͨ��MakeShared��MkaeSharable
	//5. ���ü�������0ʱ, ͨ��MakeSharable�������õ�ָ����Ҫ�ͷ���������, ��һΪ�����õĶ���, ���ΪSGenericSharedPtrData��������, ͨ��MakeShared�������õ�ָ��ֻ��Ҫ�ͷ�һ������, ��Ϊ�����ö�����SGenericSharedPtrData��ͬһ���ڴ���
	//6. ��ֲ����, ������ڸ�дĳЩ����, ʹ������ָ��Ķ���Ҳ�ܽ����ڴ����
	//----------------------------------------------------------------------------------------------
	//��std::shared_ptr��ͬ��:
	//1. std::shared_ptr ���ͷ�ʱ, ͨ��MakeShared������SharedPtr, ����Ҫɾ����������, ��std::shared_ptr��Ҫɾ�����ö������������
	//2. ��֧��week_ptr
	//3. std::shared_ptr ģ��ʵ������ռ�ÿռ�Ϊ2��ָ��, ��std::shared_ptr��������ָ�����������ü���
	//4. std::shared_ptr ���麯������ͨ�� std::make_shared �� std::shared_ptr ���콨��������, �麯������ʹ��8�ֽ�, �� std::_Ref_count_base(��ʹʹ���� VC �е� __declspec(novtable) Ҳ��Ҫ�����8�ֽ�)
	//�����������ԭʼָ�뽨������ʱ, �������std::shared_ptr���滻
	//----------------------------------------------------------------------------------------------
	//��UE TSharedPtr��ͬ��:
	//1. ɾ�����̸���һЩ, ��ֱ�ӵ�����������ָ��, ��UE TSharedPtr��Ҫͨ��һ���麯��������������, �����Զ�һЩ
	//2. ������ͬ������������std::shared_ptr�ĵ�2��3��
	//3. UE TSharedPtr�� VC �� STL ��ͬ, ͨ���麯������ͨ�� MakeShared �� MakeSharable ��������, �麯������ʹ��8�ֽ�

	struct SGenericSharedPtrData
	{
		uint32 m_refCount;//�ɸ�Ϊƽ̨��ص�������64λ����, ����Ϊ32λ�㹻, �����͸�Ϊ64λ����, �򹫹�����ָ������ռ�ÿռ���Ӧ����4�ֽ�
#ifdef DETERMINE_CONSTRUCTED_FROM_MAKESHARED_OR_MAKESHARABLE_BY_A_BOOL
		bool m_isAllocatedByMakeShared;//Ϊ֧��MakeShared, �ڲ�ȷ�����ɾ��data, ���й淶Ҫ��ֻ����MakeShared��MakeSharable, ��ɲ������bool�Լ���ռ���ڴ�����΢���ͷŶ���Ĺ���
#else
		bool m_debugIsAllocatedByMakeShared;
#endif
		InvokeDestructorFunc m_InvokeDestructorFunc;//Ϊͨ�������Ӹú���ָ����֧�ֻ���δ����virtual����Ҳ�ܹ���ȫ�ͷ�, ���й淶Ҫ�������붨��virtual����, ��ɲ�����ú���ָ���Լ���ռ���ڴ�
	};

	template <typename TPtr, typename TMemory>
	class TGenericSharedPtr
	{
	public:
		typedef TPtr PtrType;
		typedef TMemory MemoryType;

	private:
		template <typename TDerived, typename TSameMemory>
		friend class TGenericSharedPtr;
		template <typename TBase, typename TMemory2, typename TConstructFunc, typename ...TArgs>
		friend TGenericSharedPtr<TBase, TMemory2> GenericPlacementMakeShared(uint32 objSize, const InvokeDestructorFunc& DestructFunc, const TConstructFunc& ConstructFunc, TArgs&& ...args);
		template <typename TBase, typename TMemory2>
		friend TGenericSharedPtr<TBase, TMemory2> GenericPlacementMakeSharable(TBase* rawPtr, const InvokeDestructorFunc& DestructFunc);

#ifdef TEST_GENERIC_SHARED_PTR
		friend void TestSharedPtr::TestReferences();
#endif

	public:
		TGenericSharedPtr()
			: m_ptr(NULL)
			, m_data(NULL)
		{
		}
		TGenericSharedPtr(const TGenericSharedPtr& rhs)
			: m_ptr(rhs.m_ptr)
			, m_data(rhs.m_data)
		{
			this->IncRef();
		}
		//֧�����ำֵ������ָ��
		template <typename TDerived, std::enable_if_t<std::is_convertible<TDerived*, PtrType*>::value, int> = 0>
		TGenericSharedPtr(const TGenericSharedPtr<TDerived, MemoryType>& rhs)
			: m_ptr(rhs.m_ptr)
			, m_data(rhs.m_data)
		{
			this->IncRef();
		}
		TGenericSharedPtr(const std::nullptr_t nullPtr)
			: m_ptr(NULL)
			, m_data(NULL)
		{
			ASSERT(nullPtr == nullptr);
		}
		~TGenericSharedPtr()
		{
			this->DecRef();
		}
		template <typename TDerived>
		TGenericSharedPtr<TDerived, MemoryType> Cast() const
		{
			return {static_cast<TDerived*>(m_ptr), m_data};
		}
		template <typename TDerived>
		TGenericSharedPtr<TDerived, MemoryType> CastChecked() const
		{
			return { dynamic_cast<TDerived*>(m_ptr), m_data };
		}

	private:
		TGenericSharedPtr(PtrType* ptr, SGenericSharedPtrData* data)
			: m_ptr(ptr)
			, m_data(data)
		{
			this->IncRef();
		}

	public:
		inline PtrType* Get() const
		{
			return m_ptr;
		}

	public:
		inline TGenericSharedPtr& operator=(const TGenericSharedPtr& rhs)
		{
			if (m_ptr != rhs.m_ptr)
				this->AssignFrom(rhs.m_ptr, rhs.m_data);
			return *this;
		}
		inline PtrType& operator*()
		{
			return *m_ptr;
		}
		inline const PtrType& operator*() const
		{
			return *m_ptr;
		}
		inline PtrType* operator->() const
		{
			return m_ptr;
		}
		inline bool operator==(const TGenericSharedPtr& rhs) const
		{
			return m_ptr == rhs.m_ptr;
		}
		inline bool operator!=(const TGenericSharedPtr& rhs) const
		{
			return !this->operator==(rhs);
		}
		inline bool operator==(const PtrType* ptr) const
		{
			return m_ptr == ptr;
		}
		inline bool operator!=(const PtrType* ptr) const
		{
			return !this->operator==(ptr);
		}
		inline friend bool operator==(const PtrType* ptr, const TGenericSharedPtr& rhs)
		{
			return rhs.operator==(ptr);
		}
		inline friend bool operator!=(const PtrType* ptr, const TGenericSharedPtr& rhs)
		{
			return !(rhs.operator==(ptr));
		}

	public:
		inline explicit operator bool() const
		{
			return m_ptr != NULL;
		}

	private:
		void InitWithData(PtrType* ptr, const InvokeDestructorFunc& DestructFunc)
		{
			ASSERT(m_data == NULL);
			m_ptr = ptr;
			m_data = static_cast<SGenericSharedPtrData*>(MemoryType::Alloc(sizeof(SGenericSharedPtrData)));
			StaticInitData(m_data, DestructFunc, false);
			this->IncRef();
		}
		void InitWithSharedData(PtrType* ptr, const InvokeDestructorFunc& DestructFunc, SGenericSharedPtrData* data)
		{
			ASSERT(m_data == NULL);
			m_ptr = ptr;
			m_data = data;
#ifdef DETERMINE_CONSTRUCTED_FROM_MAKESHARED_OR_MAKESHARABLE_BY_A_BOOL
			StaticInitData(m_data, DestructFunc, true);
#else
			//std::stringstream ss;
			//ss << DestructFunc;
			//printf("My DestructFunc %s\n", ss.str().c_str());
			//���Է����� MinGW Build �� DestructFunc ��ַΪ 0x1
			ASSERT((reinterpret_cast<ptrdiff_t>(DestructFunc) & 0x1) == 0);

			auto flaggedDestructFunc = reinterpret_cast<InvokeDestructorFunc>(reinterpret_cast<ptrdiff_t>(DestructFunc) | 0x1);
			StaticInitData(m_data, flaggedDestructFunc, true);
#endif
			this->IncRef();
		}
		inline void IncRef()
		{
			if (m_data != NULL)
				m_data->m_refCount++;
		}
		inline void DecRef()
		{
			if (m_data != NULL)
			{
				m_data->m_refCount--;
				this->DeletePointer();
			}
		}
		inline static void StaticInitData(SGenericSharedPtrData* data, const InvokeDestructorFunc& DestructFunc, bool isAllocatedByMakeShared)
		{
			data->m_refCount = 0u;
			data->m_InvokeDestructorFunc = DestructFunc;
#ifdef DETERMINE_CONSTRUCTED_FROM_MAKESHARED_OR_MAKESHARABLE_BY_A_BOOL
			data->m_isAllocatedByMakeShared = isAllocatedByMakeShared;
#else
			data->m_debugIsAllocatedByMakeShared = isAllocatedByMakeShared;
#endif
		}
		void DeletePointer()
		{
			if (m_data->m_refCount == 0)
			{
#ifdef DETERMINE_CONSTRUCTED_FROM_MAKESHARED_OR_MAKESHARABLE_BY_A_BOOL
				if (m_data->m_isAllocatedByMakeShared)
				{
					m_data->m_InvokeDestructorFunc(m_ptr);
					auto mem = reinterpret_cast<char*>(m_ptr) - sizeof(SGenericSharedPtrData);
					MemoryType::Free(mem);
				}
				else
				{
					CGenericInstance::DestructAndFree<MemoryType>(m_ptr, m_data->m_InvokeDestructorFunc);
					//UE��ʵ�ַ���, ����һ������������(��DefaultDeleter), MakeShareableʱʹ��CRTĬ��new����, ��ɾ��ʱʹ�õ�CRTĬ��delete�ͷ�
					MemoryType::Free(m_data);
				}
#else
				//��Ȼ�������ڴ�, MkaeShared �� m_ptr ��ַһ���� m_data ��, ���������� m_data ��������һЩ������ͷŲ������ٷ����, ����һ�������Ըձ� m_ptr С
				//if (reinterpret_cast<ptrdiff_t>(m_ptr) > reinterpret_cast<ptrdiff_t>(m_data))
				//{
				//	...
				//	void* mem = reinterpret_cast<char*>(m_ptr) - sizeof(SGenericSharedPtrData);
				//	if (mem == m_data)
				//		...
				//}
				
				//��˵�������ܱ�֤������ַ�Ƕ����, ������λ��ʹ��
				//begin, GPT����
				//����ָ�������������:
				//	<cstdint> �����Ͷ��� :
				//		C++��׼�е� alignas �� alignof �ؼ��ֿ��԰��������ƺͲ�ѯ���͵Ķ���Ҫ���Ա���֪����δ���ͬ���͵�ָ�롣
				//	�����Ķ���Ҫ�� :
				//		��ȻC++��׼û����ȷ�涨����ָ��Ķ��뷽ʽ����ͨ������£���������ȷ������ָ��Ķ���������ָ����ͬ���Ա���ʵ��ʹ���б������⡣
				//	������� :
				//		��Ȼû��ֱ�ӵ����ӿ���֤������ָ�������룬��������һЩC++��׼�ĵ�����زο���
				//			C++ Standard(ISO / IEC 14882)�������ڴ˲������µ�C++��׼�ݰ�����ϸ˵����
				//			C++ Core Guidelines����Щָ�ϳ���ǿ�����Ͱ�ȫ���ڴ�������Ҫ�ԣ�
				//end

				auto funcAsInt = reinterpret_cast<ptrdiff_t>(m_data->m_InvokeDestructorFunc);
				if ((funcAsInt & 0x1) != 0)
				{
					funcAsInt &= ~0x1;
					void* mem = reinterpret_cast<char*>(m_ptr) - sizeof(SGenericSharedPtrData);
					ASSERT(m_data->m_debugIsAllocatedByMakeShared);
					auto DestructorFunc = reinterpret_cast<InvokeDestructorFunc>(funcAsInt);
					DestructorFunc(m_ptr);
					MemoryType::Free(mem);
					return;
				}

				ASSERT((reinterpret_cast<ptrdiff_t>(m_data->m_InvokeDestructorFunc) & 0x1) == 0);
				ASSERT(!m_data->m_debugIsAllocatedByMakeShared);

				CGenericInstance::DestructAndFree<MemoryType>(m_ptr, m_data->m_InvokeDestructorFunc);
				//UE��ʵ�ַ���, ����һ������������(��DefaultDeleter), MakeShareableʱʹ��CRTĬ��new����, ��ɾ��ʱʹ�õ�CRTĬ��delete�ͷ�
				MemoryType::Free(m_data);
#endif
			}
		}
		inline void AssignFrom(PtrType* ptr, SGenericSharedPtrData* data)
		{
			this->DecRef();
			m_ptr = ptr;
			m_data = data;
			this->IncRef();
		}

	private:
		template <typename TDerived>
		inline static bool StaticCheck(TDerived& rhsRawPtr)
		{
			const PtrType* thisRawPtr = rhsRawPtr;//��̬����Ƿ�Ϊ��ȫָ��ת��, �縸��ת����ָ��Ϊ����ȫ, Ӧ����ȷcast���ٸ�TSharedPtr
			return true;
		}

	private:
		PtrType* m_ptr;
		SGenericSharedPtrData* m_data;
	};
	template <typename TBase, typename TMemory, typename TConstructFunc, typename ...TArgs>
	static TGenericSharedPtr<TBase, TMemory> GenericPlacementMakeShared(uint32 objSize, const InvokeDestructorFunc& DestructFunc, const TConstructFunc& ConstructFunc, TArgs&& ...args)
	{
		TGenericSharedPtr<TBase, TMemory> shared;
		auto dataSize = sizeof(SGenericSharedPtrData);
		auto mem = TMemory::Alloc(dataSize + objSize);
		auto obj = reinterpret_cast<TBase*>(static_cast<char*>(mem) + dataSize);
		ConstructFunc(obj, std::forward<TArgs>(args)...);
		shared.InitWithSharedData(obj, DestructFunc, static_cast<SGenericSharedPtrData*>(mem));
		return shared;
	}
	template <typename TDerived, typename TMemory, typename ...TArgs>
	static TGenericSharedPtr<TDerived, TMemory> GenericMakeShared(TArgs&& ...args)
	{
		return GenericPlacementMakeShared<TDerived, TMemory>(sizeof(TDerived), &GenericInstanceInvokeDestructor<TDerived>, &GenericInstanceInvokeConstructor<TDerived, TArgs...>, std::forward<TArgs>(args)...);
	}
	//������δ����virtual����ʱ, Ϊȷ����ȫ�ͷ��������, Ӧʹ��MakeSharable, �ڴ���ԭʼ����rawָ���������������
	//�����ȷ�����ඨ��virtual����, ��ֱ��ʹ�ù��캯����=�����ܰ�ȫ�ͷŵ�
	template <typename TBase, typename TMemory>
	static TGenericSharedPtr<TBase, TMemory> GenericPlacementMakeSharable(TBase* rawPtr, const InvokeDestructorFunc& DestructFunc)
	{
		TGenericSharedPtr<TBase, TMemory> shared;
		ASSERT(rawPtr != NULL);
		shared.InitWithData(rawPtr, DestructFunc);
		return shared;
	}
	template <typename TDerived, typename TMemory>
	static TGenericSharedPtr<TDerived, TMemory> GenericMakeSharable(TDerived* rawPtr)
	{
		return GenericPlacementMakeSharable<TDerived, TMemory>(rawPtr, &GenericInstanceInvokeDestructor<TDerived>);
	}

#ifdef TEST_GENERIC_SHARED_PTR
	namespace TestSharedPtr
	{
		class CMyClass0
		{
		public:
			CMyClass0()
				: m_myFloat0(111.0f)
				, m_myFloat1(111.1f)
			{
				static int cnt = 0;
				printf("CMyClass0, constructor, %d\n", cnt++);
			}
			~CMyClass0()
			{
				static int cnt = 0;
				printf("CMyClass0, destructor, %d\n", cnt++);
			}

		public:
			float m_myFloat0;
			float m_myFloat1;
		};
		class CMyClassBase1
		{
		public:
			CMyClassBase1()
				: m_myFloatBase2(111.2f)
				, m_myFloatBase3(111.3f)
			{
				static int cnt = 0;
				printf("CMyClassBase1, constructor, %d\n", cnt++);
			}
			~CMyClassBase1()
			{
				static int cnt = 0;
				printf("CMyClassBase1, destructor, %d\n", cnt++);
			}

		public:
			float m_myFloatBase2;
			float m_myFloatBase3;
		};
		class CMyClass1 : public CMyClassBase1
		{
		public:
			CMyClass1()
				: m_myFloat4(111.4f)
				, m_myFloat5(111.5f)
			{
				static int cnt = 0;
				printf("CMyClass1, constructor, %d\n", cnt++);
			}
			CMyClass1(float arg0, bool arg1)
				: CMyClass1()
			{
			}
			~CMyClass1()
			{
				static int cnt = 0;
				printf("CMyClass1, destructor, %d\n", cnt++);
			}

		public:
			float m_myFloat4;
			CMyClass0 m_sub0;
			float m_myFloat5;
		};
		static size_t s_runtimeBytes = 0;
		
		class CTestMemory
		{
		public:
			static void* Alloc(size_t size)
			{
				auto actual = sizeof(size_t) + size;
				s_runtimeBytes += size;
				auto mem = malloc(actual);
				*static_cast<size_t*>(mem) = size;
				return static_cast<char*>(mem) + sizeof(size_t);
			}
			static void* Realloc(void* ptr, size_t size)
			{
				ASSERT(false);
				return realloc(ptr, size);
			}
			static void Free(void* ptr)
			{
				void* mem = (static_cast<char*>(ptr) - sizeof(size_t));
				const auto& size = *static_cast<size_t*>(mem);
				s_runtimeBytes -= size;
				free(mem);
			}
		};

		template <typename T>
		using TTestSharedPtr = TGenericSharedPtr<T, CTestMemory>;

		template <typename T>
		inline static TTestSharedPtr<T> TestMakeSharable(T* rawPtr)
		{
			return GenericPlacementMakeSharable<T, CTestMemory>(rawPtr, &GenericInstanceInvokeDestructor<T>);
		}
		template <typename T, typename ...TArgs>
		inline static TTestSharedPtr<T> TestMakeShared(TArgs&& ...args)
		{
			return GenericPlacementMakeShared<T, CTestMemory>(sizeof(T), &GenericInstanceInvokeDestructor<T>, &GenericInstanceInvokeConstructor<T, TArgs...>, std::forward<TArgs>(args)...);
		}

		static void TestSharedPtrConstRef(const TTestSharedPtr<CMyClass1>& obj)
		{
		}
		static void TestSharedPtrRef(TTestSharedPtr<CMyClass1>& obj)
		{
		}
		static void TestSharedPtrCopy(TTestSharedPtr<CMyClass1> obj)
		{
		}
		static void TestSharedPtrRefForError(TTestSharedPtr<CMyClass1>& obj)
		{
		}

		static void TestForCompiling()
		{
			{
				//auto a = sizeof(std::shared_ptr<float>);
				//auto b = sizeof(TGenericSharedPtrData<float>);
				//auto c = sizeof(TGenericSharedPtr<float, MemoryType>);
				//auto e = sizeof(std::aligned_union<1, float>::type);
				//std::shared_ptr<float> dddddd;
				//auto g = sizeof(dddddd.use_count());
				//printf("");
			}
			//begin, TSharedPtr����
			TTestSharedPtr<CMyClass1> a;
			a = NULL;
			a = nullptr;
			TTestSharedPtr<CMyClass1> b;
			a = b;
			TTestSharedPtr<CMyClass1> c = a;
			TTestSharedPtr<CMyClassBase1> d = a;
			TTestSharedPtr<CMyClassBase1> e;
			e = a;
			TestSharedPtrConstRef(NULL);
			TestSharedPtrConstRef(nullptr);
			TestSharedPtrConstRef(a);
			TestSharedPtrRef(a);
			TestSharedPtrCopy(NULL);
			TestSharedPtrCopy(nullptr);
			TestSharedPtrCopy(a);
			//�������º��Ӧ���ֱ������, ��������ֱ��ͨ��rawָ�븳ֵ
	//#define TEST_FOR_ERROR
	#ifdef TEST_FOR_ERROR
			CMyClass1* f = NULL;
			a = f;
			TSharedPtr<CMyClass1> g = f;
			TestSharedPtrRefForError(NULL);
			TestSharedPtrRefForError(nullptr);
	#endif
			//end
		}
		void TestReferences()
		{
			auto& runtimeBytes = s_runtimeBytes;
			{
				TTestSharedPtr<CMyClass1> a = TestMakeShared<CMyClass1>();
				auto sizeWithSharedPtrUnit = sizeof(CMyClass1)+sizeof(TTestSharedPtr<CMyClass1>);
				ASSERT(a.m_data->m_refCount == 1);
				ASSERT(runtimeBytes == sizeWithSharedPtrUnit);
				auto b = TestMakeShared<CMyClass1>(1.0f, true);
				ASSERT(runtimeBytes == sizeWithSharedPtrUnit * 2);
				ASSERT(b.m_data->m_refCount == 1);
				b = a;
				ASSERT(a.m_data->m_refCount == 2);
				ASSERT(a.m_data == b.m_data);
				auto c = b;
				ASSERT(a.m_data->m_refCount == 3);
				b = NULL;
				ASSERT(a.m_data->m_refCount == 2);
				ASSERT(runtimeBytes == sizeWithSharedPtrUnit);
				CMyClass1* d = new (CTestMemory::Alloc(sizeof(CMyClass1))) CMyClass1();
				ASSERT(runtimeBytes == sizeWithSharedPtrUnit + sizeof(CMyClass1));
				auto e = TestMakeSharable(d);
				ASSERT(e.m_data->m_refCount == 1);
				ASSERT(runtimeBytes == sizeWithSharedPtrUnit * 2);
				auto f = e;
				ASSERT(e.m_data->m_refCount == 2);
				ASSERT(runtimeBytes == sizeWithSharedPtrUnit * 2);
				f = NULL;
				ASSERT(e.m_data->m_refCount == 1);
				e = nullptr;
				ASSERT(e.m_data == NULL);
				ASSERT(runtimeBytes == sizeWithSharedPtrUnit);
			}
			ASSERT(runtimeBytes == 0);
		}
	}
#endif
}