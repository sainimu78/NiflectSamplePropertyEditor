#pragma once
#include "Niflect/Base/Array.h"
#include "qwidget.h"
#include "qboxlayout.h"
#include "qsplitter.h"
#include "qpushbutton.h"
#include <functional>

class QRowItem;

using OnRowResetFunc = std::function<void()>;

class QPropertyRow : public QWidget
{
	typedef QWidget inherited;
public:
	QPropertyRow(QWidget* parentWidget);

public:
	void InitIndents(uint32 cnt);
	void Init2Columns();
	void InitSection();
	void InitResetter();

public:
	bool IsEnabledResetter() const;
	void SetEnabledResettter(bool b);
	void SetOnRowResetFunc(const OnRowResetFunc& Func)
	{
		m_OnRowResetFunc = Func;
	}

public:
	QHBoxLayout* m_indentLayout;
	QHBoxLayout* m_labelLayout;
	QHBoxLayout* m_editLayout;
	QHBoxLayout* m_toolLayout;
	QSplitter* m_splitter;
	Niflect::TArray<QWidget*> m_vecIndent;
	QHBoxLayout* m_mainLayout;
	QRowItem* m_column0;
	QRowItem* m_column1;
	QPushButton* m_reset;
	OnRowResetFunc m_OnRowResetFunc;
};