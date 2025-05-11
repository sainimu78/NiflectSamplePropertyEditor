#pragma once
#include "Property/PropertyNode.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include "Qt/Widget/PropertyUiNode_gen.h"

class QPropertyRow;
class QPropertyTree;

class CPropertyUiNode;
using CSharedPropertyUiNode = Niflect::TSharedPtr<CPropertyUiNode>;

class CEditWidgetCreationContext
{
public:
	CEditWidgetCreationContext(CPropertyNode* inProp, QPropertyRow* inRow, QWidget*& outLabelWidget, QWidget*& outEditWidget)
		: m_inProp(inProp)
		, m_inRow(inRow)
		, m_outLabelWidget(outLabelWidget)
		, m_outEditWidget(outEditWidget)
	{
	}
	CPropertyNode* m_inProp;
	QPropertyRow* m_inRow;
	QWidget*& m_outLabelWidget;
	QWidget*& m_outEditWidget;
};

class CChildEditWidgetCreationContext
{
public:
	CChildEditWidgetCreationContext(CPropertyNode* inProp, QPropertyRow* inRow)
		: m_inProp(inProp)
		, m_inRow(inRow)
	{
	}
	CPropertyNode* m_inProp;
	QPropertyRow* m_inRow;
};

using OnCollapserExpandedFunc = std::function<void()>;

class QCollapser : public QPushButton
{
	typedef QPushButton inherited;
public:
	QCollapser(QWidget* parentWidget);

public:
	bool IsExpanded() const;
	void SetExpanded(bool b);

public:
	void SetOnCollapserExpandedFunc(const OnCollapserExpandedFunc& Func);

private:
	bool m_expanded;
	OnCollapserExpandedFunc m_OnCollapserExpandedFunc;
};

class CPropertyUiNode
{
public:
	CPropertyUiNode()
		: m_dataNode(NULL)
		, m_row(NULL)
		, m_tree(NULL)
		, m_labelWidget(NULL)
		, m_editWidget(NULL)
		, m_collapser(NULL)
	{
	}
	~CPropertyUiNode()
	{
		printf("");
	}
	void InitRowWidget(QPropertyTree* tree, CPropertyNode* dataNode, QPropertyRow* row, const Niflect::CString& caption);
	void AddNode(const CSharedPropertyUiNode& uiNode)
	{
		m_vecNode.push_back(uiNode);
	}
	void ResetForRebuilding()
	{
		m_dataNode = NULL;
		m_tree = NULL;
		m_labelWidget = NULL;
		m_editWidget = NULL;
		
		m_row = NULL;
		m_collapser = NULL;
	}
	bool IsValid() const
	{
		return m_dataNode != NULL;
	}

public:
	Niflect::CString m_caption;
	CPropertyNode* m_dataNode;
	QPropertyTree* m_tree;
	QWidget* m_labelWidget;
	QWidget* m_editWidget;
	QPropertyRow* m_row;
	QCollapser* m_collapser;
	Niflect::TArray<CSharedPropertyUiNode> m_vecNode;
};

NIF_T()
class CCompoundPropertyCollapser : public CCompoundProperty
{
	typedef CCompoundProperty inherited;
};

NIF_T()
class CFloatPropertyLineEdit : public CFloatProperty
{
	typedef CFloatProperty inherited;
protected:
	virtual bool SaveToUiImpl(const CRwNode* rw) const override;
	virtual bool LoadFromUiImpl(CRwNode* rw) const override;

public:
	virtual void InitWithUiNode() override;
	virtual void CreateEditWidget(CEditWidgetCreationContext& ctx) override;

private:
	QLineEdit* GetEditWidget() const;
};

using OnArrayEditAddFunc = std::function<void()>;
using OnArrayEditRemoveFunc = std::function<void()>;

class QAddRemoveButtons : public QWidget
{
	typedef QWidget inherited;
public:
	QAddRemoveButtons(QWidget* parentWidget);

public:
	void SetOnArrayEditAdd(const OnArrayEditAddFunc& Func);
	void SetOnArrayEditRemove(const OnArrayEditRemoveFunc& Func);

public:
	void UpdateButtonsState(uint32 currentItemsCount);

private:
	QPushButton* m_add;
	QPushButton* m_remove;
	OnArrayEditAddFunc m_OnAddFunc;
	OnArrayEditRemoveFunc m_OnRemoveFunc;
};

using OnArrayElementEditInsertFunc = std::function<void()>;
using OnArrayElementEditDeleteFunc = std::function<void()>;

class QInsertDeleteButtons : public QWidget
{
	typedef QWidget inherited;
public:
	QInsertDeleteButtons(QWidget* parentWidget);

public:
	void SetOnArrayElementEditInsertFunc(const OnArrayElementEditInsertFunc& Func);
	void SetOnArrayElementEditDeleteFunc(const OnArrayElementEditDeleteFunc& Func);

private:
	QPushButton* m_insert;
	QPushButton* m_delete;
	OnArrayElementEditInsertFunc m_OnInsertFunc;
	OnArrayElementEditDeleteFunc m_OnDeleteFunc;
};

NIF_T()
class CArrayPropertyAddRemoveDeleteInsert : public CArrayProperty
{
	typedef CArrayProperty inherited;
public:
	virtual void CreateEditWidget(CEditWidgetCreationContext& ctx) override;
	virtual void CreateChildEditWidget(CChildEditWidgetCreationContext& ctx, uint32 idx) override;
};