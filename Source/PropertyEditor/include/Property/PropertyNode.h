#pragma once
#include "Niflect/Base/SharedPtr.h"
#include "Niflect/Base/Array.h"
#include "Niflect/Serialization/RwTree.h"
#include "Niflect/NiflectType.h"
#include "ExampleStructure.h"

class CPropertyNode;
using CSharedPropertyNode = Niflect::TSharedPtr<CPropertyNode>;

class CPropertyEditContext;

class CPropertyUiNode;
class CEditWidgetCreationContext;
class CChildEditWidgetCreationContext;

using namespace RwTree;

using OnCreateEditWidgetFunc = std::function<void(CEditWidgetCreationContext& ctx)>;
using OnCreateChildEditWidgetFunc = std::function<void(CChildEditWidgetCreationContext& ctx, uint32 idx)>;

class CPropertyNode
{
	friend class CPropertyEditScope;
public:
	CPropertyNode(bool falseGroup_trueItem)
		: m_parent(NULL)
		, m_falseGroup_trueItem(falseGroup_trueItem)
		, m_editCtx(NULL)
		, m_uiNode(NULL)
		, m_elemType(NULL)
	{

	}
	~CPropertyNode()
	{
		printf("");
	}
	void InitForNode(CPropertyEditContext* editCtx, CPropertyNode* parent, const Niflect::CString& name)
	{
		m_editCtx = editCtx;
		m_parent = parent;
		m_name = name;
	}
	void InitForContainer(Niflect::CNiflectType* elemType)
	{
		ASSERT(m_elemType == NULL);
		m_elemType = elemType;
	}
	void InitForUi(CPropertyUiNode* uiNode)
	{
		m_uiNode = uiNode;
	}
	bool IsItem() const
	{
		return m_falseGroup_trueItem;
	}
	const Niflect::CString& GetName() const
	{
		return m_name;
	}
	bool IsContainer() const
	{
		return m_elemType != NULL;
	}
	CPropertyNode* GetParent() const
	{
		return m_parent;
	}
	void InsertNode(const CSharedPropertyNode& node, uint32 idx);
	void AddNode(const CSharedPropertyNode& node);
	void DeleteNode(uint32 idx);
	void DeleteLastNode();
	void Clear();
	uint32 GetNodesCount() const
	{
		return static_cast<uint32>(m_vecNode.size());
	}
	void SetOnCreateEditWidgetFunc(const OnCreateEditWidgetFunc& Func)
	{
		m_OnCreateEditWidgetFunc = Func;
	}
	void SetOnCreateChildEditWidgetFunc(const OnCreateChildEditWidgetFunc& Func)
	{
		m_OnCreateChildEditWidgetFunc = Func;
	}

public:
	bool SaveToDefault()
	{
		return this->LoadFromCurrentImpl(&m_rwDefault);
	}
	bool LoadFromDefault()
	{
		return this->SaveToCurrentImpl(&m_rwDefault);
	}
	bool SaveToCurrent(const CRwNode* rw)
	{
		return this->SaveToCurrentImpl(rw);
	}
	bool LoadFromCurrent(CRwNode* rw) const
	{
		return this->LoadFromCurrentImpl(rw);
	}
	bool IsEqualToDefault(const CRwNode* rw) const
	{
		if (rw->IsValue() == m_rwDefault.IsValue())
		{
			auto a = rw->GetValue();
			auto b = m_rwDefault.GetValue();
			if (a->GetType() == b->GetType())
				return rw->GetData() == m_rwDefault.GetData();
		}
		else if(rw->IsArray() == m_rwDefault.IsArray())
		{
			ASSERT(false);//CRwNode 中已记录 DeepSeek 生成的比较函数备用, 如确需可考虑启用
		}
		else
		{
			ASSERT(false);//CRwNode 中已记录 DeepSeek 生成的比较函数备用, 如确需可考虑启用
		}
		return false;
	}
	bool IsDefaultValid() const
	{
		return m_rwDefault.IsValue() || m_rwDefault.IsArray() || m_rwDefault.GetNodesCount() > 0;
	}
	bool SaveToUi(const CRwNode* rw) const
	{
		return this->SaveToUiImpl(rw);
	}
	bool LoadFromUi(CRwNode* rw) const
	{
		return this->LoadFromUiImpl(rw);
	}

protected:
	virtual bool SaveToUiImpl(const CRwNode* rw) const { return false; }
	virtual bool LoadFromUiImpl(CRwNode* rw) const { return false; }

	virtual bool SaveToCurrentImpl(const CRwNode* rw) { return false; }
	virtual bool LoadFromCurrentImpl(CRwNode* rw) const { return false; }

public:
	virtual void InitWithUiNode() {}
	virtual void CreateEditWidget(CEditWidgetCreationContext& ctx) {}
	virtual void CreateChildEditWidget(CChildEditWidgetCreationContext& ctx, uint32 idx) {}

public:
	Niflect::TArray<CSharedPropertyNode> m_vecNode;
	Niflect::CString m_name;
	CPropertyNode* m_parent;
	OnCreateEditWidgetFunc m_OnCreateEditWidgetFunc;
	OnCreateChildEditWidgetFunc m_OnCreateChildEditWidgetFunc;
	CPropertyUiNode* m_uiNode;

protected:
	CPropertyEditContext* m_editCtx;
	Niflect::CNiflectType* m_elemType;

private:
	const bool m_falseGroup_trueItem;
	CRwNode m_rwDefault;
};

class CPropertyGroup : public CPropertyNode
{
	typedef CPropertyNode inherited;
public:
	CPropertyGroup()
		: inherited(false)
	{
	}

public:
	static CPropertyGroup* Cast(inherited* base)
	{
		ASSERT(dynamic_cast<CPropertyGroup*>(base) != NULL);
		return static_cast<CPropertyGroup*>(base);
	}
};

class CPropertyItem : public CPropertyNode
{
	typedef CPropertyNode inherited;
public:
	CPropertyItem()
		: inherited(true)
	{
	}

public:
	static CPropertyItem* Cast(inherited* base)
	{
		ASSERT(dynamic_cast<CPropertyItem*>(base) != NULL);
		return static_cast<CPropertyItem*>(base);
	}
};

class CCompoundProperty : public CPropertyGroup
{
};

class CBoolProperty : public CPropertyItem
{
public:
	CBoolProperty();

public:
	void SetValue(bool value);
	bool GetValue() const;

protected:
	virtual bool SaveToCurrentImpl(const CRwNode* rw) override;
	virtual bool LoadFromCurrentImpl(CRwNode* rw) const override;

private:
	bool m_value;
};

class CFloatProperty : public CPropertyItem
{
public:
	CFloatProperty();

public:
	void SetValue(float value);
	float GetValue() const;

protected:
	virtual bool SaveToCurrentImpl(const CRwNode* rw) override;
	virtual bool LoadFromCurrentImpl(CRwNode* rw) const override;

private:
	float m_value;
};

class CArrayProperty : public CPropertyGroup
{
public:
	void AddElement();
	void DeleteLastElement();
	void InsertElement(uint32 idx);
	void DeleteElement(uint32 idx);
	uint32 GetElementsCount() const;
	void Resize(uint32 cnt);

private:
	void InternalInsertElement(uint32 idx);
	void InternalDeleteElement(uint32 idx);
	void UpdateElementsName();

protected:
	virtual bool SaveToCurrentImpl(const CRwNode* rw) override;
	virtual bool LoadFromCurrentImpl(CRwNode* rw) const override;
};

class CVector3Property : public CPropertyItem
{
public:
	void SetValue(const CVector3& value);
	const CVector3& GetValue() const;
	void SetEntry(float entry, uint32 idx);
	const float& GetEntry(uint32 idx) const;

protected:
	virtual bool SaveToCurrentImpl(const CRwNode* rw) override;
	virtual bool LoadFromCurrentImpl(CRwNode* rw) const override;

private:
	CVector3 m_value;
};