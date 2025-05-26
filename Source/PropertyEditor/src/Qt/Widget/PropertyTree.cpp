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
	//���������޸ĵ� FAQ
	//Q1: �������Ƿ��Ч?
	//	A1: ����Ч. ���Ч, ȫ�湤ҵ���༭���ܵ�ʵ��, ������ζ�Ŵ����� 20000+ �ж��� 2000+ ��, �ؼ�����:
	//		1. ���Բ���
	//			1. ���Լ���������Ч�Ը���, �������޸ĺ�������޸ķ�Ӧ����׷��, ���͵���
	//				1. һЩ�༭���˵�����ֵ�������Ե��޸�, ��Ҫ����������������ֵ��Ч�Ը���
	//				2. һЩ�Խṹչ����ʽ�༭������, �� Vector3, ÿ�� Entry ���޸����������µ�չ���� Entry, ������ֵ�������������������
	//			2. ����һЩ�༭�ؼ���Ԥ�������������޸�, ���͵������קԤ�������ֱ༭�ؼ�, ��Ԥ��ʱ���޸Ĳ�Ӱ�쳷��ջ, ͬʱ�ܹ������޸ĵ���������
	//			3. ��Щ���Ӳ���ʵ�ֵĹؼ�������, �����Ե��޸���һ������ͼ�ṹ��֯, ��̬�ر���������ӽڵ�, ���еĶ�̬Ҫ������Ϊ�޸Ĳ����Ľṹ���ǹ̶���
	//				1. �˿�ܵ��ռ��ѵ�������μ�ʵ����Ч������, �ֱ������Զ��巽ʽ�Լ�
	//		2. ���Կؼ�����
	//			1. Ϊ�ܹ�ƽ��ͨ����������, �����ڽ������Է�֧�ؽ��Ļ���������Ӧ�Ŀؼ����Ż�.
	//			2. �����ܽ��ڿɼ�ʱ����
	//			3. ���������ؽ���ʹ�ؼ������ؽ�ǰ�ı༭״̬
	//		3. ���ʵ��������ͬ�������༭
	//Q2: ��˾����ʵ���Ƿ���ζ��û��ʵ����?
	//	A2: ��. ���������������ʾ A1 �������и���ʵ�ֱ���ĺ��Ĳ���, ��������֧�ؽ�. �����ʵ���������侫���չʾ�����и���ʵ��ǰ�Ĵ���Ǽ�����, �Ľ����ڴ˻����Ͻ���.
	//Q3: ���δʵ�ֳ���, �Ƿ���ζ�Ų���ʵ��?
	//	A3: ��. ������/����������˵���ܹ�����ʵ�ֵ�, ��ͨ�������������� Save / Load ����, �볷��ջ�Ľ�ջ/��ջ��Ӧ, ��������ʵ����������ɳ���, ��ô�����������ڴ�ռ�ô�������.
	//		�����ǲ��ܸĽ�, ;����ʵ��һ���������л�, ����ջ��ֻ�����޸�ǰ��Ĳ�������. �˷�������������Ҫ�������仯�������ݵ�����
	CRwNode rw;
	if (prop->LoadFromUi(&rw))
		prop->SaveToCurrent(&rw);
	this->RebuildUpdateNotify(prop->m_uiNode);
}
void QPropertyTree::ResetBranchRecurs(CPropertyNode* prop)
{
	if (prop->LoadFromDefault())
		this->SaveDefaultForJustAdded();//�������Ϳ�����ӽڵ�, ����뱣��Ĭ��ֵ
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
//��������ڵ�� DFS �ռ�
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

	if (!m_firstShown)//�������״� resizeEvent ִ��ǰ��ȡ��δ������ɵĿ��
		this->UpdateRowsResetterAndAdjustSplitterAndUpdateExpansion(uiNode);
}
void QPropertyTree::showEvent(QShowEvent* event)
{
	inherited::showEvent(event);
	if (m_uiNodeRoot.IsValid())
		this->UpdateRowsResetterAndAdjustSplitterAndUpdateExpansion(&m_uiNodeRoot);
	m_firstShown = false;
}