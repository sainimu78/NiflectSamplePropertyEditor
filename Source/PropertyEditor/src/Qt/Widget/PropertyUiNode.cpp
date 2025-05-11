#include "Qt/Widget/PropertyUiNode.h"
#include "Qt/Util/CoreUtil.h"
#include "Qt/Util/WidgetUtil.h"
#include "Qt/Widget/PropertyRow.h"
#include "Qt/Widget/PropertyTree.h"

void CPropertyUiNode::InitRowWidget(QPropertyTree* tree, CPropertyNode* dataNode, QPropertyRow* row, const Niflect::CString& caption)
{
	m_tree = tree;
	m_dataNode = dataNode;
	m_row = row;
	m_caption = caption;

	{
		ASSERT(m_labelWidget == NULL);
		ASSERT(m_editWidget == NULL);
		CEditWidgetCreationContext ctx(m_dataNode, m_row, m_labelWidget, m_editWidget);
		if (auto& Func = m_dataNode->m_OnCreateEditWidgetFunc)
			m_dataNode->m_OnCreateEditWidgetFunc(ctx);
		else
			m_dataNode->CreateEditWidget(ctx);
	}

	if (m_dataNode->IsItem())
	{
		m_row->Init2Columns();
		if (m_labelWidget == NULL)
		{
			auto btn = new QPushButton(m_row);
			InitPushButtonInPropertyLabelStyle(btn);
			btn->setText(CStringToQString(m_caption));
			m_labelWidget = btn;
		}
	}
	else
	{
		m_row->InitSection();
		if (m_labelWidget == NULL)
		{
			auto cps = new QCollapser(m_row);
			InvokeNoSignal(cps, &QCollapser::SetExpanded, true);//折叠消耗更少, 但为方便测试操作, 默认改为展开
			cps->setText(CStringToQString(m_caption));
			cps->SetOnCollapserExpandedFunc([this, cps]()
				{
					m_tree->UpdateRowsExpansion(this);
				});
			m_collapser = cps;
			m_labelWidget = m_collapser;
		}
	}

	if (m_labelWidget != NULL)
		m_row->m_labelLayout->addWidget(m_labelWidget);
	if (m_editWidget != NULL)
		m_row->m_editLayout->addWidget(m_editWidget);

	m_dataNode->InitWithUiNode();
}

QCollapser::QCollapser(QWidget* parentWidget)
	: inherited(parentWidget)
	, m_expanded(false)
{
	InitPushButtonInPropertyLabelStyle(this);
	QObject::connect(this, &QCollapser::clicked, [this]()
		{
			m_expanded = !m_expanded;
			m_OnCollapserExpandedFunc();
		});
}
bool QCollapser::IsExpanded() const
{
	return m_expanded;
}
void QCollapser::SetExpanded(bool b)
{
	m_expanded = b;
	emit QCollapser::clicked();
}
void QCollapser::SetOnCollapserExpandedFunc(const OnCollapserExpandedFunc& Func)
{
	m_OnCollapserExpandedFunc = Func;
}

bool CFloatPropertyLineEdit::SaveToUiImpl(const CRwNode* rw) const
{
	auto edt = this->GetEditWidget();
	InvokeNoSignal(edt, &QLineEdit::setText, QString("%1").arg(GetRwValueAs<float>(rw->GetValue())));
	return true;
}
bool CFloatPropertyLineEdit::LoadFromUiImpl(CRwNode* rw) const
{
	auto edt = this->GetEditWidget();
	SetRwValueAs<float>(rw->ToValue(), edt->text().toFloat());
	return true;
}
void CFloatPropertyLineEdit::InitWithUiNode()
{
	auto edt = this->GetEditWidget();
	QObject::connect(edt, &QLineEdit::textChanged, [this]()
		{
			m_uiNode->m_tree->MarkChangeFromUi(this);
		});
}
void CFloatPropertyLineEdit::CreateEditWidget(CEditWidgetCreationContext& ctx)
{
	auto edt = new QLineEdit(ctx.m_inRow);
	ctx.m_outEditWidget = edt;
}
QLineEdit* CFloatPropertyLineEdit::GetEditWidget() const
{
	ASSERT(dynamic_cast<QLineEdit*>(m_uiNode->m_editWidget) != NULL);
	return static_cast<QLineEdit*>(m_uiNode->m_editWidget);
}

QAddRemoveButtons::QAddRemoveButtons(QWidget* parentWidget)
	: inherited(parentWidget)
	, m_add(NULL)
	, m_remove(NULL)
{
	auto mainLayout = CreateLayoutNoMargin<QHBoxLayout>(this);
	m_add = CreateSmallButton("add", this);
	QObject::connect(m_add, &QPushButton::clicked, [this]()
		{
			m_OnAddFunc();
		});
	mainLayout->addWidget(m_add);
	m_remove = CreateSmallButton("rmv", this);
	QObject::connect(m_remove, &QPushButton::clicked, [this]()
		{
			m_OnRemoveFunc();
		});
	mainLayout->addWidget(m_remove);
}
void QAddRemoveButtons::SetOnArrayEditAdd(const OnArrayEditAddFunc& Func)
{
	m_OnAddFunc = Func;
}
void QAddRemoveButtons::SetOnArrayEditRemove(const OnArrayEditRemoveFunc& Func)
{
	m_OnRemoveFunc = Func;
}
void QAddRemoveButtons::UpdateButtonsState(uint32 currentItemsCount)
{
	m_remove->setEnabled(currentItemsCount > 0);
}

QInsertDeleteButtons::QInsertDeleteButtons(QWidget* parentWidget)
	: inherited(parentWidget)
	, m_insert(NULL)
	, m_delete(NULL)
{
	auto mainLayout = CreateLayoutNoMargin<QHBoxLayout>(this);
	m_insert = CreateSmallButton("ins", this);
	QObject::connect(m_insert, &QPushButton::clicked, [this]()
		{
			m_OnInsertFunc();
		});
	mainLayout->addWidget(m_insert);
	m_delete = CreateSmallButton("del", this);
	QObject::connect(m_delete, &QPushButton::clicked, [this]()
		{
			m_OnDeleteFunc();
		});
	mainLayout->addWidget(m_delete);
}
void QInsertDeleteButtons::SetOnArrayElementEditInsertFunc(const OnArrayElementEditInsertFunc& Func)
{
	m_OnInsertFunc = Func;
}
void QInsertDeleteButtons::SetOnArrayElementEditDeleteFunc(const OnArrayElementEditDeleteFunc& Func)
{
	m_OnDeleteFunc = Func;
}

void CArrayPropertyAddRemoveDeleteInsert::CreateEditWidget(CEditWidgetCreationContext& ctx)
{
	auto aed = new QAddRemoveButtons(ctx.m_inRow);
	aed->SetOnArrayEditAdd([this]()
		{
			m_editCtx->MarkChanged(this);
			this->AddElement();
			m_uiNode->m_tree->MarkChangeFromUi(this);
		});
	aed->SetOnArrayEditRemove([this]()
		{
			m_editCtx->MarkChanged(this);
			this->DeleteLastElement();
			m_uiNode->m_tree->MarkChangeFromUi(this);
		});
	aed->UpdateButtonsState(ctx.m_inProp->GetNodesCount());//现数组操作后整个分支重建, 因此仅在此更新即可
	ctx.m_outEditWidget = aed;
}
void CArrayPropertyAddRemoveDeleteInsert::CreateChildEditWidget(CChildEditWidgetCreationContext& ctx, uint32 idx)
{
	auto eed = new QInsertDeleteButtons(ctx.m_inRow);
	eed->SetOnArrayElementEditInsertFunc([this, idx]()
		{
			m_editCtx->MarkChanged(this);
			this->InsertElement(idx);
			m_uiNode->m_tree->MarkChangeFromUi(this);
		});
	eed->SetOnArrayElementEditDeleteFunc([this, idx]()
		{
			m_editCtx->MarkChanged(this);
			this->DeleteElement(idx);
			m_uiNode->m_tree->MarkChangeFromUi(this);
		});
	ctx.m_inRow->m_toolLayout->addWidget(eed);
}