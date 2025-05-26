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
	//关于属性修改的 FAQ
	//Q1: 此做法是否高效?
	//	A1: 不高效. 如高效, 全面工业级编辑功能等实现, 可能意味着代码量 20000+ 行而非 2000+ 行, 关键点如:
	//		1. 属性层框架
	//			1. 属性间联动的有效性更新, 如属性修改后的完整修改反应链条追踪, 典型的如
	//				1. 一些编辑器端的输入值限制属性的修改, 需要联动所关联的属性值有效性更新
	//				2. 一些以结构展开形式编辑的属性, 如 Vector3, 每个 Entry 的修改须联动更新到展开的 Entry, 如果结合值限制属性则情况更复杂
	//			2. 区分一些编辑控件的预览操作产生的修改, 典型的如可拖拽预览的数字编辑控件, 在预览时的修改不影响撤销栈, 同时能够保持修改的属性联动
	//			3. 这些复杂操作实现的关键方法是, 将属性的修改以一种树或图结构组织, 动态地遍历并添加子节点, 其中的动态要求是因为修改产生的结构不是固定的
	//				1. 此框架的终极难点在于如何即实现有效性联动, 又保持属性定义方式仍简单
	//		2. 属性控件层框架
	//			1. 为能够平衡通用性与性能, 必须在接受属性分支重建的基础上作相应的控件层优化.
	//			2. 尽可能仅在可见时更新
	//			3. 尽可能在重建后使控件保持重建前的编辑状态
	//		3. 多个实例的属性同步联动编辑
	//Q2: 如此精简的实现是否意味着没有实用性?
	//	A2: 否. 精简的意义在于演示 A1 所述所有复杂实现背后的核心操作, 即整个分支重建. 因此其实用性在于其精简地展示了所有复杂实现前的代码骨架流程, 改进可在此基础上进行.
	//Q3: 框架未实现撤销, 是否意味着不易实现?
	//	A3: 否. 单撤销/重做功能来说是能够极简实现的, 即通过属性树独立的 Save / Load 数据, 与撤销栈的进栈/出栈对应, 即可轻易实现任意操作可撤销, 这么做的问题是内存占用大且冗余.
	//		但并非不能改进, 途径是实现一种增量序列化, 撤销栈中只保存修改前后的差异数据. 此方法还可用于如要求仅传输变化属性数据的需求
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
	else if (par != NULL)
	{
		par->m_uiNode->m_row->SetEnabledResettter(enabled);
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