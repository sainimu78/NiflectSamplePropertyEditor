#pragma once
#include "qwidget.h"
#include "qpushbutton.h"

template <typename T>
T* CreateLayoutNoMargin(QWidget* owner)
{
	auto layout = new T(owner);
	layout->setMargin(0);
	return layout;
}
static QPushButton* CreateSmallButton(const QString& cap, QWidget* parentWidget)
{
	auto btn = new QPushButton(cap, parentWidget);
	btn->setFixedWidth(40);
	return btn;
}
template <typename TQtObject, typename TMethod, typename ...TArgs>
static void InvokeNoSignal(TQtObject* object, TMethod MemberFunc, TArgs&& ...args)
{
	bool wasBlocked = object->signalsBlocked();
	object->blockSignals(true);
	(object->*MemberFunc)(std::forward<TArgs>(args)...);
	object->blockSignals(wasBlocked);
}
static void InitPushButtonInPropertyLabelStyle(QPushButton* wdg)
{
	wdg->setSizePolicy(QSizePolicy::Expanding, wdg->sizePolicy().verticalPolicy());
	wdg->setStyleSheet(
R"(QPushButton {
text-align: left;
})"
	);
}