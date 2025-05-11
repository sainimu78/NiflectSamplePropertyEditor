#include "Qt/Widget/PropertyRow.h"
#include "qpushbutton.h"
#include "Qt/Util/WidgetUtil.h"
#include "qpainter.h"

class QRowItem : public QWidget
{
	typedef QWidget inherited;
public:
	QRowItem(QWidget* parentWidget);

public:
	virtual void paintEvent(QPaintEvent* event) override;
};

QRowItem::QRowItem(QWidget* parentWidget)
	: inherited(parentWidget)
{
}
void QRowItem::paintEvent(QPaintEvent* event)
{
	inherited::paintEvent(event);
	//QPainter painter(this);
	//auto rc = this->rect();
	//QPen pen;
	//pen.setColor(Qt::red);
	//painter.setPen(pen);
	//painter.setBrush(Qt::NoBrush);
	//painter.drawRect(rc);
}

QPropertyRow::QPropertyRow(QWidget* parentWidget)
	: inherited(parentWidget)
	, m_mainLayout(NULL)
	, m_column0(NULL)
	, m_column1(NULL)
	, m_indentLayout(NULL)
	, m_labelLayout(NULL)
	, m_editLayout(NULL)
	, m_toolLayout(NULL)
	, m_splitter(NULL)
	//, m_caption(NULL)
	, m_reset(NULL)
{
	m_mainLayout = CreateLayoutNoMargin<QHBoxLayout>(this);
	//mainLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);
	{
		m_column0 = new QRowItem(this);
		auto column0Layout = CreateLayoutNoMargin<QHBoxLayout>(m_column0);
		auto indentArea = new QWidget(this);
		m_indentLayout = CreateLayoutNoMargin<QHBoxLayout>(indentArea);
		column0Layout->addWidget(indentArea);
		{
			auto labelArea = new QWidget(this);
			m_labelLayout = CreateLayoutNoMargin<QHBoxLayout>(labelArea);
			column0Layout->addWidget(labelArea);
		}
	}
	{
		m_column1 = new QRowItem(this);
		auto column1Layout = CreateLayoutNoMargin<QHBoxLayout>(m_column1);
		{
			auto editArea = new QWidget(this);
			m_editLayout = CreateLayoutNoMargin<QHBoxLayout>(editArea);
			column1Layout->addWidget(editArea);
		}
		{
			auto toolArea = new QWidget(this);
			m_toolLayout = CreateLayoutNoMargin<QHBoxLayout>(toolArea);
			column1Layout->addWidget(toolArea);
		}
	}
}
void QPropertyRow::InitIndents(uint32 cnt)
{
	auto indentArea = m_indentLayout->widget();
	int32 diff = static_cast<uint32>(m_vecIndent.size()) - cnt;
	while (diff > 0)
	{
		delete m_vecIndent[0];
		m_vecIndent.erase(m_vecIndent.begin());
		diff--;
	}
	diff = cnt - static_cast<uint32>(m_vecIndent.size());
	while (diff > 0)
	{
		auto indentWdg = new QPushButton("-", indentArea);
		indentWdg->setFixedWidth(20);
		indentWdg->setFlat(true);
		m_vecIndent.push_back(indentWdg);
		m_indentLayout->addWidget(indentWdg);
		diff--;
	}
	ASSERT(m_vecIndent.size() == cnt);
}
void QPropertyRow::Init2Columns()
{
	m_splitter = new QSplitter(this);
	m_splitter->addWidget(m_column0);
	m_splitter->addWidget(m_column1);

	m_mainLayout->addWidget(m_splitter);
}
void QPropertyRow::InitSection()
{
	m_mainLayout->addWidget(m_column0);
	m_mainLayout->addWidget(m_column1);
}
void QPropertyRow::InitResetter()
{
	m_reset = new QPushButton("rst", this);
	m_reset->setFixedWidth(40);
	QObject::connect(m_reset, &QPushButton::clicked, [this]()
		{
			m_OnRowResetFunc();
		});
	m_toolLayout->addWidget(m_reset);
}
void QPropertyRow::SetEnabledResettter(bool b)
{
	m_reset->setEnabled(b);
	m_reset->setFlat(!m_reset->isEnabled());
}