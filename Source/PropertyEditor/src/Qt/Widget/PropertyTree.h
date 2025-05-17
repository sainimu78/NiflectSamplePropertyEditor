#pragma once
#include "Niflect/Base/Array.h"
#include "Qt/Widget/PropertyRow.h"
#include "qboxlayout.h"
#include "Qt/Widget/PropertyUiNode.h"
#include "Property/PropertyEditContext.h"

using OnChangeFromUiAppliedFunc = std::function<void(CPropertyNode* prop)>;

class QPropertyTree : public QWidget
{
	typedef QWidget inherited;
	friend class CPropertyEditContext;
public:
	QPropertyTree(QWidget* parentWidget);

public:
	void Init(CPropertyEditContext* editCtx);
	CPropertyUiNode* GetUiNodeRoot() { return &m_uiNodeRoot; }
	void RebuildRows(CPropertyNode* prop, CPropertyUiNode* uiNode);
	void UpdateRowsExpansion(CPropertyUiNode* uiNode) const;

private:
	uint32 FindRowIndex(const CPropertyUiNode* uiNode) const;
	void DeleteRows(CPropertyUiNode* uiNode) const;
	void BuildRows(CPropertyNode* prop, CPropertyUiNode* uiNode);
	void InsertRows(CPropertyUiNode* uiNode, uint32 insertIdx) const;
	void SaveDefaultForJustAdded() const;
	void SaveToRows(CPropertyUiNode* uiNode) const;

private:
	void BuildRowsRecurs(CPropertyNode* prop, CPropertyUiNode* uiNode, uint32 depth);
	void ResetBranch(CPropertyUiNode* uiNode);
	void ResetBranchRecurs(CPropertyNode* prop);
	void UpdateRowsResetterAndAdjustSplitterAndUpdateExpansion(CPropertyUiNode* uiNode) const;
	void UpdateResetter(CPropertyUiNode* uiNode) const;
	bool UpdateResetterRecurs(CPropertyUiNode* uiNode) const;
	void RebuildUpdateNotify(CPropertyUiNode* uiNode);
	void UpdateRowsExpansionRecurs(CPropertyUiNode* uiNode) const;

public:
	void MarkChangeFromUi(CPropertyNode* prop);

public:
	void SetOnChangeFromUiAppliedFunc(const OnChangeFromUiAppliedFunc& Func) { m_OnChangeFromUiAppliedFunc = Func; }

public:
	virtual void showEvent(QShowEvent* event) override;

private:
	QVBoxLayout* m_mainLayout;
	QVBoxLayout* m_rowsLayout;
	OnChangeFromUiAppliedFunc m_OnChangeFromUiAppliedFunc;
	CPropertyUiNode m_uiNodeRoot;
	CPropertyEditContext* m_editCtx;
	bool m_firstShown;
};