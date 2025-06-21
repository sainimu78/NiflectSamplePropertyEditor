#include "Qt/Widget/PropertyTree.h"
#include "PropertyEditorSystem.h"
#include "qpushbutton.h"
#include "Qt/Util/CoreUtil.h"

QPropertyTree::QPropertyTree(QWidget* parentWidget)
	: inherited(parentWidget)
	, m_mainLayout(NULL)
	, m_rowsLayout(NULL)
	, m_editCtx(NULL)
	, m_firstShown(true)
{
	m_mainLayout = new QVBoxLayout(this);
	m_rowsLayout = new QVBoxLayout;
	m_rowsLayout->setAlignment(Qt::AlignmentFlag::AlignTop);
	m_mainLayout->addLayout(m_rowsLayout);
}
void QPropertyTree::Init(CPropertyEditContext* editCtx)
{
	m_editCtx = editCtx;
}
void QPropertyTree::RebuildRows(CPropertyNode* prop, CPropertyUiNode* uiNode)
{
	this->SaveDefaultForJustAdded();
	auto insertingIdx = this->FindRowIndex(uiNode);
	this->DeleteRows(uiNode);
	this->BuildRows(prop, uiNode);
	this->InsertRows(uiNode, insertingIdx);
}
uint32 QPropertyTree::FindRowIndex(const CPropertyUiNode* uiNode) const
{
	uint32 insertIdx = 0;
	if (uiNode->m_row != NULL)
		insertIdx = m_rowsLayout->indexOf(uiNode->m_row);
	return insertIdx;
}
void QPropertyTree::RebuildUpdateNotify(CPropertyUiNode* uiNode)
{
	auto& prop = uiNode->m_dataNode;
	if (!prop->IsItem())
		this->RebuildRows(prop, uiNode);
	this->UpdateResetter(uiNode);
	m_OnChangeFromUiAppliedFunc(prop);
}
void QPropertyTree::MarkChangeFromUi(CPropertyNode* prop)
{
	CRwNode rw;
	if (prop->LoadFromUi(&rw))
		prop->SaveToCurrent(&rw);
	this->RebuildUpdateNotify(prop->m_uiNode);
}
void QPropertyTree::ResetBranchRecurs(CPropertyNode* prop)
{
	if (prop->LoadFromDefault())
		this->SaveDefaultForJustAdded();//如数组型可能添加节点, 因此须保存默认值
	for (auto& it : prop->m_vecNode)
		this->ResetBranchRecurs(it.Get());
}
void QPropertyTree::ResetBranch(CPropertyUiNode* uiNode)
{
	auto& prop = uiNode->m_dataNode;
	this->ResetBranchRecurs(prop);
	CRwNode rw;
	if (prop->LoadFromCurrent(&rw))
		prop->SaveToUi(&rw);
	this->RebuildUpdateNotify(uiNode);
}
void QPropertyTree::BuildRowsRecurs(CPropertyNode* prop, CPropertyUiNode* uiNode, uint32 depth)
{
	prop->InitForUi(uiNode);

	{
		auto row = new QPropertyRow(this);
		row->InitIndents(depth);
		row->SetOnRowResetFunc([this, uiNode]()
			{
				this->ResetBranch(uiNode);
			});

		auto cap = uiNode->m_caption;
		if (cap.empty())
			cap = prop->m_name;
		if (cap.empty())
			cap = "(No named)";// cap = prop->m_instNode->GetType()->GetTypeName();
		uiNode->InitRowWidget(this, prop, row, cap);
	}

	depth++;
	for (uint32 idx = 0; idx < prop->m_vecNode.size(); ++idx)
	{
		auto propChild = prop->m_vecNode[idx].Get();
		auto uiChild = Niflect::MakeShared<CPropertyUiNode>();
		this->BuildRowsRecurs(propChild, uiChild.Get(), depth);
		{
			CChildEditWidgetCreationContext ctx(propChild, uiChild->m_row);
			if (auto& Func = prop->m_OnCreateChildEditWidgetFunc)
				Func(ctx, idx);
			else
				prop->CreateChildEditWidget(ctx, idx);
		}
		uiNode->AddNode(uiChild);
	}
}
//包括传入节点的 DFS 收集
static void CollectRowsRecurs(CPropertyUiNode* ui, Niflect::TArray<CPropertyUiNode*>& vecNode, Niflect::TArray<QPropertyRow*>& vecRow, Niflect::TSet<QPropertyRow*>& setRow)
{
	if (ui->m_row != NULL)
	{
		if (setRow.insert(ui->m_row).second)
			vecRow.push_back(ui->m_row);
		vecNode.push_back(ui);
	}
	for (auto& it : ui->m_vecNode)
		CollectRowsRecurs(it.Get(), vecNode, vecRow, setRow);
}
void QPropertyTree::BuildRows(CPropertyNode* prop, CPropertyUiNode* uiNode)
{
	uint32 depth = 0;
	auto par = prop->m_parent;
	while (par != NULL)
	{
		depth++;
		par = par->m_parent;
	}
	this->BuildRowsRecurs(prop, uiNode, depth);

	this->SaveToRows(uiNode);
}
void QPropertyTree::SaveDefaultForJustAdded() const
{
	for (auto& it : m_editCtx->m_vecChanged)
	{
		if (!it->IsDefaultValid())
			it->SaveToDefault();
	}
	m_editCtx->m_vecChanged.clear();
}
void QPropertyTree::SaveToRows(CPropertyUiNode* uiNode) const
{
	Niflect::TArray<CPropertyUiNode*> vecNode;
	Niflect::TArray<QPropertyRow*> vecRow;
	Niflect::TSet<QPropertyRow*> setRow;
	CollectRowsRecurs(uiNode, vecNode, vecRow, setRow);
	for (auto& it : vecNode)
	{
		auto& prop = it->m_dataNode;
		CRwNode rw;
		if (prop->LoadFromCurrent(&rw))
			prop->SaveToUi(&rw);
	}
}
void QPropertyTree::DeleteRows(CPropertyUiNode* uiNode) const
{
	Niflect::TArray<CPropertyUiNode*> vecNode;
	Niflect::TArray<QPropertyRow*> vecRow;
	Niflect::TSet<QPropertyRow*> setRow;
	CollectRowsRecurs(uiNode, vecNode, vecRow, setRow);
	for (auto& it : vecNode)
		it->ResetForRebuilding();
	for (auto& it : vecRow)
		delete it;
	uiNode->m_vecNode.clear();
}
void QPropertyTree::UpdateResetter(CPropertyUiNode* uiNode) const
{
	bool enabled = this->UpdateResetterRecurs(uiNode);
	auto par = uiNode->m_dataNode->m_parent;
	if (enabled)
	{
		while (par != NULL)
		{
			par->m_uiNode->m_row->SetEnabledResettter(enabled);
			par = par->m_parent;
		}
	}
	else
	{
		while (par != NULL)
		{
			bool enabled = false;
			for (auto& it : par->m_uiNode->m_vecNode)
			{
				if (it->m_row->IsEnabledResetter())
				{
					enabled = true;
					break;
				}
			}
			if (!enabled)
				par->m_uiNode->m_row->SetEnabledResettter(enabled);
			else
				break;
			par = par->m_parent;
		}
	}
}
void QPropertyTree::UpdateRowsExpansion(CPropertyUiNode* uiNode) const
{
	this->UpdateRowsExpansionRecurs(uiNode);
}
void QPropertyTree::UpdateRowsExpansionRecurs(CPropertyUiNode* uiNode) const
{
	bool isParentRowExpanded = true;
	if (auto cps = uiNode->m_collapser)
		isParentRowExpanded = cps->IsExpanded();
	if (isParentRowExpanded)
	{
		for (auto& it : uiNode->m_vecNode)
		{
			it->m_row->setVisible(isParentRowExpanded);
			this->UpdateRowsExpansionRecurs(it.Get());
		}
	}
	else
	{
		Niflect::TArray<CPropertyUiNode*> vecNode;
		Niflect::TArray<QPropertyRow*> vecRow;
		Niflect::TSet<QPropertyRow*> setRow;
		for (auto& it : uiNode->m_vecNode)
			CollectRowsRecurs(it.Get(), vecNode, vecRow, setRow);
		for (auto& it : vecRow)
			it->setVisible(false);
	}
}
bool QPropertyTree::UpdateResetterRecurs(CPropertyUiNode* uiNode) const
{
	bool enabled = false;
	for (auto& it : uiNode->m_vecNode)
	{
		if (this->UpdateResetterRecurs(it.Get()))
			enabled = true;
	}
	if (!enabled)
	{
		auto& prop = uiNode->m_dataNode;
		if (prop->IsDefaultValid())
		{
			CRwNode rw;
			if (prop->LoadFromCurrent(&rw))
				enabled = !prop->IsEqualToDefault(&rw);
		}
		uiNode->m_row->SetEnabledResettter(enabled);
	}
	return enabled;
}
void QPropertyTree::UpdateRowsResetterAndAdjustSplitterAndUpdateExpansion(CPropertyUiNode* uiNode) const
{
	Niflect::TArray<CPropertyUiNode*> vecNode;
	Niflect::TArray<QPropertyRow*> vecRow;
	Niflect::TSet<QPropertyRow*> setRow;
	CollectRowsRecurs(uiNode, vecNode, vecRow, setRow);

	this->UpdateResetter(uiNode);

	this->UpdateRowsExpansionRecurs(uiNode);

	auto width = this->width();
	QList<int> lstColumnWidth;
	int w0 = width / 2;
	lstColumnWidth.push_back(w0);
	lstColumnWidth.push_back(width - w0);
	for (auto& it : vecRow)
	{
		if (it->m_splitter != NULL)
			it->m_splitter->setSizes(lstColumnWidth);
	}
}
void QPropertyTree::InsertRows(CPropertyUiNode* uiNode, uint32 insertIdx) const
{
	Niflect::TArray<CPropertyUiNode*> vecNode;
	Niflect::TArray<QPropertyRow*> vecRow;
	Niflect::TSet<QPropertyRow*> setRow;
	CollectRowsRecurs(uiNode, vecNode, vecRow, setRow);
	for (uint32 idx = 0; idx < vecRow.size(); ++idx)
	{
		auto& row = vecRow[idx];
		row->InitResetter();
		m_rowsLayout->insertWidget(insertIdx + idx, row);
	}

	if (!m_firstShown)//避免在首次 resizeEvent 执行前获取到未调整完成的宽度
		this->UpdateRowsResetterAndAdjustSplitterAndUpdateExpansion(uiNode);
}
void QPropertyTree::showEvent(QShowEvent* event)
{
	inherited::showEvent(event);
	if (m_uiNodeRoot.IsValid())
		this->UpdateRowsResetterAndAdjustSplitterAndUpdateExpansion(&m_uiNodeRoot);
	m_firstShown = false;
}