#pragma once
#include "Niflect/NiflectCommon.h"
#include "Niflect/NiflectRegisteredType.h"
#include "Niflect/NiflectMethod.h"
#include "Niflect/NiflectAccessor.h"
#ifdef USING_STD_ARRAY_TO_FILL_ARGS
#include <array>
#endif

namespace Niflect
{
#ifdef REFACTORING_0_TYPE_ACCESSOR_FIELD_RESTRUACTURING
#else
	typedef CSharedAccessor (*CreateTypeAccessorFunc)();
	using CreateFieldLayoutOfTypeFuncOld = CreateTypeAccessorFunc;

	struct STypeLifecycleMeta
	{
		NifUint32 m_typeSize;
		InvokeConstructorFunc m_InvokeConstructorFunc;//Ĭ�Ϻ���������ʽΪ InvokeConstructorFunc
		InvokeDestructorFunc m_InvokeDestructorFunc;
	};
#endif

	class CNiflectTable;
	class CNiflectType;
	using CStaticNiflectTypeAddr = CNiflectType*;

#ifdef REFACTORING_0_TYPE_ACCESSOR_FIELD_RESTRUACTURING
	typedef void (*BuildTypeMetaFunc)(CNiflectType* type);
#endif

#ifdef REFACTORING_0_TYPE_ACCESSOR_FIELD_RESTRUACTURING
	class CNiflectType
	{
		friend class CTypeLayout;
	public:
		CNiflectType()
			: m_table(NULL)
			, m_tableIdx(NifInvalidIndex)
			, m_typeSize(0)
#ifdef NIFLECTDEV_GENERATED_TYPE_ALIGNMENT
			, m_typeAlignment(0)
#else
#endif
			, m_InvokeDestructorFunc(NULL)
			, m_BuildTypeMetaFunc(NULL)
			, m_staticTypePtrAddr(NULL)
			, m_typeHash(NifInvalidHash)
		{
		}
		~CNiflectType()
		{
			if (m_staticTypePtrAddr != NULL)
				*m_staticTypePtrAddr = NULL;
		}

	public:
		void InitTypeMeta(CNiflectTable* table, NifUint32 tableIdx, NifUint32 typeSize, NifUint32 typeAlignment, const InvokeDestructorFunc& inInvokeDestructorFunc, const HashInt& typeHash, const CString& id, const BuildTypeMetaFunc& inBuildTypeMetaFunc, CStaticNiflectTypeAddr* staticTypePtrAddr, const CSharedNata& nata)
		{
			m_name = id;
			m_table = table;
			m_tableIdx = tableIdx;
			m_nata = nata;
			m_typeSize = typeSize;
#ifdef NIFLECTDEV_GENERATED_TYPE_ALIGNMENT
			m_typeAlignment = typeAlignment;
#else
#endif
			m_InvokeDestructorFunc = inInvokeDestructorFunc;
			m_BuildTypeMetaFunc = inBuildTypeMetaFunc;
			m_staticTypePtrAddr = staticTypePtrAddr;
			*m_staticTypePtrAddr = this;
			m_typeHash = typeHash;
		}

	public:
		CNiflectTable* GetTable() const
		{
			return m_table;
		}
		const NifUint32& GetTableIndex() const
		{
			return m_tableIdx;
		}
		const CString& GetTypeName() const//todo: �ƻ�����Ϊ GetNativeTypeName
		{
			return m_name;
		}
		const NifUint32& GetTypeSize() const//todo: �ƻ�����Ϊ GetNativeTypeSize
		{
			return m_typeSize;//����C++ Built in����, ��������Ϊconst ref��Ϊ�˷��㸳ֵ������auto
		}
#ifdef NIFLECTDEV_GENERATED_TYPE_ALIGNMENT
		const NifUint32& GetTypeAlignment() const
		{
			return m_typeAlignment;
		}
#else
#endif
		const HashInt& GetTypeHash() const
		{
			return m_typeHash;
		}
		const CTypeLayout& GetTypeLayout() const
		{
			return m_layout;
		}
		const Niflect::TArray<CField>& GetFields() const
		{
			return m_vecFiled;
		}
		CNiflectAccessor* GetAccessor() const
		{
			return m_accessor.Get();
		}

	public:
		inline bool SaveInstanceToRwNode(const InstanceType* base, CRwNode* rw) const
		{
			return m_layout.AccessorsSaveToRwNode(base, rw);
		}
		inline bool LoadInstanceFromRwNode(InstanceType* base, const CRwNode* rw) const
		{
			return m_layout.AccessorsLoadFromRwNode(base, rw);
		}
		inline bool BuildInstanceNode(CNiflectInstanceNode* node)
		{
			return m_layout.AccessorsBuildInstanceNode(node);
		}

	public:
		void BuildTypeMeta()
		{
			NIFLECT_ASSERT(m_layout.m_vecSubobject.size() == 0);
			this->InitTypeLayout(m_layout);
			m_BuildTypeMetaFunc(this);
		}
		void InitAccessor(const CSharedAccessor& accessor)
		{
			NIFLECT_ASSERT(m_accessor == NULL);
			m_accessor = accessor;
		}
		void InitAddField(const Niflect::CString& name, const OffsetType& offset, CNiflectType* type, const CSharedNata& nata, const HashInt& fieldHash)
		{
			CField field;
			field.Init(name, offset, type, nata, fieldHash);
			m_vecFiled.push_back(field);
		}
		void InitAddMethodInfo(const CMethodInfo& info)
		{
			m_vecMethodInfo.push_back(info);
		}
		void InitAddStaticMethodInfo(const CFunctionInfo& info)
		{
			m_vecStaticMemberFunctionInfo.push_back(info);
		}
		void InitAddConstructorInfo(const CConstructorInfo& info)
		{
			m_vecConstructorInfo.push_back(info);
		}

	protected:
		virtual void InitTypeLayout(CTypeLayout& layout)
		{
			layout.m_vecSubobject.push_back(this);
		}

	public:
		void InitNata(const CSharedNata& nata)
		{
			m_nata = nata;
		}
		CNata* GetNata() const
		{
			return m_nata.Get();
		}

	private:
		CString m_name;
		CSharedNata m_nata;
		CNiflectTable* m_table;
		NifUint32 m_tableIdx;
		CTypeLayout m_layout;
		CSharedAccessor m_accessor;
		Niflect::TArray<CField> m_vecFiled;

	public:
		Niflect::TArray<CConstructorInfo> m_vecConstructorInfo;
		Niflect::TArray<CMethodInfo> m_vecMethodInfo;
		Niflect::TArray<CFunctionInfo> m_vecStaticMemberFunctionInfo;
		InvokeDestructorFunc m_InvokeDestructorFunc;

	private:
		NifUint32 m_typeSize;
#ifdef NIFLECTDEV_GENERATED_TYPE_ALIGNMENT
		NifUint32 m_typeAlignment;
#else
#endif
		BuildTypeMetaFunc m_BuildTypeMetaFunc;
		CStaticNiflectTypeAddr* m_staticTypePtrAddr;
		HashInt m_typeHash;
	};
#else
	class CNiflectType
	{
	public:
		CNiflectType()
			: m_table(NULL)
			, m_tableIdx(INDEX_NONE)
			, m_lifecycleMeta{}
			, m_CreateTypeAccessorFunc(NULL)
			, m_staticTypePtrAddr(NULL)
			, m_typeHash(0)
		{
		}
		~CNiflectType()
		{
			if (m_staticTypePtrAddr != NULL)
				*m_staticTypePtrAddr = NULL;
		}

	public:
		void InitTypeMeta(NifUint32 niflectTypeSize, size_t typeHash, const CString& name, NifUint32 index, const STypeLifecycleMeta& cb)
		{
			NIFLECT_ASSERT(false);
			//m_name = name;
			//m_index = index;
			//m_niflectTypeSize = niflectTypeSize;
			//m_cb = cb;
			//m_typeHash = typeHash;
		}
		void InitTypeMeta2(CNiflectTable* table, NifUint32 tableIdx, const STypeLifecycleMeta& lifecycleMeta, size_t typeHash, const CString& id, const CreateTypeAccessorFunc& inCreateTypeAccessorFunc, CStaticNiflectTypeAddr* staticTypePtrAddr, const CSharedNata& nata)
		{
			m_name = id;
			m_table = table;
			m_tableIdx = tableIdx;
			m_nata = nata;
			m_lifecycleMeta = lifecycleMeta;
			m_CreateTypeAccessorFunc = inCreateTypeAccessorFunc;
			m_staticTypePtrAddr = staticTypePtrAddr;
			*m_staticTypePtrAddr = this;
			m_typeHash = typeHash;
		}
		
	public:
		//CField* GetRootField() const
		//{
		//	return m_fieldRoot.Get();
		//}
		//const CSharedAccessor& GetSharedAccessorRoot() const
		//{
		//	return m_accessorRoot;
		//}
		CNiflectTable* GetTable() const
		{
			return m_table;
		}
		const NifUint32& GetTableIndex() const//todo: �ƻ�����Ϊ GetTableIndex
		{
			return m_tableIdx;
		}
		const CString& GetTypeName() const//todo: �ƻ�����Ϊ GetNativeTypeName
		{
			return m_name;
		}
		const NifUint32& GetTypeSize() const//todo: �ƻ�����Ϊ GetNativeTypeSize
		{
			return m_lifecycleMeta.m_typeSize;//����C++ Built in����, ��������Ϊconst ref��Ϊ�˷��㸳ֵ������auto
		}
		const CreateTypeAccessorFunc& GetCreateTypeAccessorFunc() const
		{
			return m_CreateTypeAccessorFunc;
		}
		const STypeLifecycleMeta& GetLifecycleMeta() const
		{
			return m_lifecycleMeta;
		}

	public:
		bool SaveInstanceToRwNode(const InstanceType* base, CRwNode* rw) const
		{
			for (auto& it : m_layout.m_vecAccessor)
			{
				if (!it->SaveToRwNode(base, rw))
					return false;
			}
			return true;
		}
		bool LoadInstanceFromRwNode(InstanceType* base, const CRwNode* rw) const
		{
			for (auto& it : m_layout.m_vecAccessor)
			{
				if (!it->LoadFromRwNode(base, rw))
					return false;
			}
			return true;
		}

	public:
		//void Construct(void* instanceBase) const
		//{
		//	if (m_cb.m_InvokeConstructorFunc != NULL)
		//		m_cb.m_InvokeConstructorFunc(instanceBase);
		//}
		//void Destruct(void* instanceBase) const
		//{
		//	if (m_cb.m_InvokeDestructorFunc != NULL)
		//		m_cb.m_InvokeDestructorFunc(instanceBase);
		//}
		CSharedAccessor CreateFieldLayout() const//todo:�ƻ�ɾ��
		{
			NIFLECT_ASSERT(false);
			if (m_CreateTypeAccessorFunc != NULL)
				return m_CreateTypeAccessorFunc();
			return NULL;
		}
		CSharedAccessor CreateAccessor() const
		{
			NIFLECT_ASSERT(false);
			if (m_CreateTypeAccessorFunc != NULL)
				return m_CreateTypeAccessorFunc();
			return NULL;
		}
		void InitFieldLayout()
		{
			NIFLECT_ASSERT(false);//todo: �ƻ�����, ����Ӧ�������м̳����ϵ�layout, ����layoutҲ�����������layout������Ӧ�ĳ�ʼ��
			//todo: δȷ������, �õ��ٴ���������Module��ʼ��ʱͳһ��������, ���ú���ʵ��
			//NIFLECT_ASSERT(m_fieldRoot == NULL);
			//m_fieldRoot = this->CreateFieldLayout();
		}
		void InitTypeLayout()
		{
			NIFLECT_ASSERT(m_layout.m_vecAccessor.size() == 0);
			this->CreateTypeLayout(m_layout);
		}
		void InitAddFieldToAccessor(CNiflectAccessor* owner, const Niflect::CString& name, const OffsetType& offset, const CSharedNata& nata) const
		{
			CField field;
			field.Init(name, nata);
			this->CreateTypeLayout(field.m_layout);
			for (auto& it1 : field.m_layout.m_vecAccessor)
				it1->InitOffset(offset);
			owner->InitAddField(field);
		}
		void InitAccessorElementLayout(CNiflectAccessor* owner) const
		{
			CTypeLayout layout;
			this->CreateTypeLayout(layout);
			owner->InitElementLayout(layout);
		}

	protected:
		virtual void CreateTypeLayout(CTypeLayout& layout) const
		{
			if (m_CreateTypeAccessorFunc != NULL)
				layout.m_vecAccessor.push_back(m_CreateTypeAccessorFunc());
		}

	//public:
		//template <typename TBase>
		//inline TSharedPtr<TBase> MakeSharableInstance(TBase* obj) const
		//{
		//	return GenericMakeSharable<TBase, CMemory>(obj, m_cb.m_InvokeDestructorFunc);
		//}

	//public:
	//	template <typename T>
	//	inline T& GetInstanceRef(void* parentBase) const
	//	{
	//		return this->GetAccessorRoot()->GetInstanceRef<T>(parentBase);
	//	}
	public:
		//todo: �˺����ɷ���, ��Ϊ��̬������ȡm_typeHash
		template <typename T>
		inline T& GetInstanceRef(InstanceType* base) const
		{
			auto Check = [this]()
			{
				NIFLECT_ASSERT(GetTypeHash<T>() == m_typeHash);
				return true;
			};
			NIFLECT_ASSERT(Check());
			return *static_cast<T*>(base);
		}

	public:
		void InitNata(const CSharedNata& nata)
		{
			m_nata = nata;
		}
		CNata* GetNata() const
		{
			return m_nata.Get();
		}

	public:
		template <typename T>
		static size_t GetTypeHash()
		{
			return typeid(T).hash_code();
		}

	//private:
	//	virtual void DebugFuncForDynamicCast() {}//��Ϊ��̬������ͱ������, ���Ѷ���ǵ����õ�virtual��������Ƴ�, ��ע: error C2683: 'dynamic_cast': 'XXX' is not a polymorphic type 

	private:
		//todo: �ƻ�����Ϊ m_id, ͨ��Ϊ natiev type name
		//NiflectGen �����ɵ� id Ϊ�� namespace �� type name, ������� NIFLECT_TYPE_REGISTER ��ע��� id �� namespace, ������Ҫ��������װ.
		//����Ҫ��ȷһ��, id ���������ڱ����ظ������л�ʹ��, ʵ���Ƿ�� namespace ͨ���޹ؽ�Ҫ, һ�������ͨ��������������ּ���
		CString m_name;
		CSharedNata m_nata;
		CNiflectTable* m_table;
		NifUint32 m_tableIdx;//todo: �ƻ�����Ϊ m_tableIdx;
		CTypeLayout m_layout;
		STypeLifecycleMeta m_lifecycleMeta;//todo: �ƻ�����Ϊ m_typeFuncs
		CreateTypeAccessorFunc m_CreateTypeAccessorFunc;
		CStaticNiflectTypeAddr* m_staticTypePtrAddr;
		size_t m_typeHash;
	};
#endif
	using CSharedNiflectType = TSharedPtr<CNiflectType>;

#ifdef REFACTORING_0_TYPE_ACCESSOR_FIELD_RESTRUACTURING
	#ifdef USING_STD_ARRAY_TO_FILL_ARGS
	template <typename TMemory, typename TBase, typename ...TArgs>
	inline static TSharedPtr<TBase> NiflectTypeGenericMakeShared(const CNiflectType* type, TArgs&& ...args)
	{
		std::array<Niflect::InstanceType*, sizeof ...(TArgs)> argArray = { (&args)... };
		return GenericPlacementMakeShared<TBase, TMemory>(type->GetTypeSize(), type->m_InvokeDestructorFunc, type->m_vecConstructorInfo[0].m_Func, argArray.data());
	}
	template <typename TBase, typename ...TArgs>
	inline static TSharedPtr<TBase> NiflectTypeMakeShared(const CNiflectType* type, TArgs&& ...args)
	{
		return NiflectTypeGenericMakeShared<CMemory, TBase>(type, std::forward<TArgs>(args)...);
	}
	#else
	template <typename TBase, typename ...TArgs>
	inline static TSharedPtr<TBase> NiflectTypeMakeShared(const CNiflectType* type)
	{
		Niflect::InstanceType** argArray = NULL;
		NIFLECT_ASSERT(type->m_vecConstructorInfo[0].m_vecInput.size() == 0);
		return GenericPlacementMakeShared<TBase, CMemory>(type->GetTypeSize(), type->m_InvokeDestructorFunc, type->m_vecConstructorInfo[0].m_Func, argArray);
	}
	template <typename TBase, typename ...TArgs>
	inline static TSharedPtr<TBase> NiflectTypeMakeShared(const CNiflectType* type, TArgs&& ...args)
	{
		//std::array<Niflect::InstanceType*, sizeof ...(TArgs)> args_array = { (&args)... };//��ʹ�� std::array ��ɲ������޲����汾�ĺ���
		Niflect::InstanceType* argArray[] = { (&args)... };
		NIFLECT_ASSERT(type->m_vecConstructorInfo[0].m_vecInput.size() > 0);
		return GenericPlacementMakeShared<TBase, CMemory>(type->GetTypeSize(), type->m_InvokeDestructorFunc, type->m_vecConstructorInfo[0].m_Func, argArray);
	}
	#endif
#else
	template <typename TBase>
	inline static TSharedPtr<TBase> NiflectTypeMakeShared(const CNiflectType* type)
	{
		auto& meta = type->GetLifecycleMeta();
		return GenericPlacementMakeShared<TBase, CMemory>(meta.m_typeSize, meta.m_InvokeDestructorFunc, meta.m_InvokeConstructorFunc);
	}
#endif
	
	class CEnumConstMeta
	{
	public:
		CEnumConstMeta()
		{

		}
		CEnumConstMeta(const Niflect::CString& name, const CSharedNata& nata)
			: m_name(name)
			, m_nata(nata)
		{
		}
		CNata* GetNata() const
		{
			return m_nata.Get();
		}
		Niflect::CString m_name;

	private:
		CSharedNata m_nata;
	};

	class CEnumMeta
	{
	public:
		void InitAddConst(const Niflect::CString& name, const CSharedNata& nata)
		{
			m_vecEnumConstMeta.push_back(CEnumConstMeta(name, nata));
		}
		Niflect::TArrayNif<CEnumConstMeta> m_vecEnumConstMeta;
	};

	class CEnum : public CNiflectType
	{
		typedef CNiflectType inherited;
	public:
		void InitEnumMeta(const CEnumMeta& data)
		{
			m_enumMeta = data;
		}
		const CEnumMeta& GetEnumMeta() const
		{
			return m_enumMeta;
		}
		const CString& GetEnumConstNameByIndex(NifUint32 idx) const
		{
			return m_enumMeta.m_vecEnumConstMeta[idx].m_name;
		}
		NifUint32 FindEnumConstMetaIndex(const CString& name) const
		{
			for (NifUint32 idx = 0; idx < m_enumMeta.m_vecEnumConstMeta.size(); ++idx)
			{
				if (m_enumMeta.m_vecEnumConstMeta[idx].m_name == name)
					return idx;
			}
			return NifInvalidIndex;
		}
		template <typename TEnumType>
		const CString& GetEnumConstName(const TEnumType& e) const
		{
			auto idx = static_cast<NifUint32>(e);
			return this->GetEnumConstNameByIndex(idx);
		}

	public:
		static CEnum* Cast(CNiflectType* base)
		{
			NIFLECT_ASSERT(dynamic_cast<CEnum*>(base) != NULL);
			return static_cast<CEnum*>(base);
		}

	private:
		CEnumMeta m_enumMeta;
	};

	class CFunction : public CNiflectType
	{
	};

#ifdef REFACTORING_0_TYPE_ACCESSOR_FIELD_RESTRUACTURING
	class CInheritableType : public CNiflectType
	{
		typedef CNiflectType inherited;
	public:
		CInheritableType()
			: m_parent(NULL)
		{
		}
		void InitInheritableTypeMeta(CInheritableType* parent)
		{
			m_parent = parent;
		}
		CInheritableType* GetParent() const
		{
			return m_parent;
		}

	protected:
		virtual void InitTypeLayout(CTypeLayout& layout) override
		{
			if (m_parent != NULL)
			{
				auto par = m_parent;
				while (par != NULL)
				{
					layout.m_vecSubobject.insert(layout.m_vecSubobject.begin(), par);
					par = par->m_parent;
				}
			}
			inherited::InitTypeLayout(layout);
		}

	public:
		static CInheritableType* Cast(CNiflectType* base)
		{
			NIFLECT_ASSERT(dynamic_cast<CInheritableType*>(base) != NULL);
			return static_cast<CInheritableType*>(base);
		}
		static CInheritableType* CastChecked(CNiflectType* base)
		{
			return dynamic_cast<CInheritableType*>(base);
		}

	private:
		CInheritableType* m_parent;//todo: Ӧ����AddChild�����㼶��ϵ; ��һ����, ���ܻ���Ҫ����������������ù�ϵ, Ŀǰ��ȷ�����ù�ϵ�Ǿ�̬��̬����
	};
#else
	class CInheritableType : public CNiflectType
	{
		typedef CNiflectType inherited;
	public:
		CInheritableType()
			: m_parent(NULL)
		{
		}
		void InitInheritableTypeMeta(CInheritableType* parent)
		{
			m_parent = parent;
		}
		CInheritableType* GetParent() const
		{
			return m_parent;
		}

	protected:
		virtual void CreateTypeLayout(CTypeLayout& layout) const
		{
			inherited::CreateTypeLayout(layout);
			if (m_parent != NULL)
			{
				NIFLECT_ASSERT(layout.m_vecAccessor.size() == 1);
				auto par = m_parent;
				while (par != NULL)
				{
					auto& Func = par->GetCreateTypeAccessorFunc();
					NIFLECT_ASSERT(Func != NULL);
					layout.m_vecAccessor.insert(layout.m_vecAccessor.begin(), Func());
					par = par->m_parent;
				}
			}
		}

	//private:
	//	static void GetStackedTypes(CNiflectType* derivedType, bool reversed, TArrayNif<CInheritableType*>& vecStackedType)
	//	{
	//		auto baseType = CInheritableType::CastChecked(derivedType);
	//		while (baseType != NULL)
	//		{
	//			if (!reversed)
	//				vecStackedType.insert(vecStackedType.begin(), baseType);
	//			else
	//				vecStackedType.push_back(baseType);
	//			baseType = baseType->GetParent();
	//		}
	//	}

	//public:
	//	inline static void GetStackedTypesConstructionOrder(CNiflectType* derivedType, TArrayNif<CInheritableType*>& vecStackedType)
	//	{
	//		GetStackedTypes(derivedType, false, vecStackedType);
	//	}
	//	inline static void GetStackedTypesDestructionOrder(CNiflectType* derivedType, TArrayNif<CInheritableType*>& vecStackedType)
	//	{
	//		GetStackedTypes(derivedType, true, vecStackedType);
	//	}

	public:
		static CInheritableType* Cast(CNiflectType* base)
		{
			NIFLECT_ASSERT(dynamic_cast<CInheritableType*>(base) != NULL);
			return static_cast<CInheritableType*>(base);
		}
		static CInheritableType* CastChecked(CNiflectType* base)
		{
			return dynamic_cast<CInheritableType*>(base);
		}

	private:
		CInheritableType* m_parent;//todo: Ӧ����AddChild�����㼶��ϵ; ��һ����, ���ܻ���Ҫ����������������ù�ϵ, Ŀǰ��ȷ�����ù�ϵ�Ǿ�̬��̬����
	};
#endif

	class CStruct : public CInheritableType
	{
		typedef CInheritableType inherited;
	public:
		static CStruct* Cast(CNiflectType* base)
		{
			NIFLECT_ASSERT(dynamic_cast<CStruct*>(base) != NULL);
			return static_cast<CStruct*>(base);
		}
	};
	
	//todo: �����Ƿ��CStruct�̳��Ը������͹�ϵ, ��m_parent�ǲ�ͬ��, ���������CClass��m_parentʹ��CStruct��m_parent����ӦCast, �Ƿ�ɽ���
	class CClass : public CInheritableType
	{
		typedef CInheritableType inherited;
	public:
		static CClass* Cast(CNiflectType* base)
		{
			NIFLECT_ASSERT(dynamic_cast<CClass*>(base) != NULL);
			return static_cast<CClass*>(base);
		}

	public:
		//TArray<CNiflectMethod> m_vecMethod;
	};

	inline static bool IsSameType(const CNiflectType* a, const CNiflectType* b)
	{
		return a->GetTypeHash() == b->GetTypeHash();
	}
	NIFLECT_API HashInt ComputeTypeHash(const Niflect::CString& str);
}