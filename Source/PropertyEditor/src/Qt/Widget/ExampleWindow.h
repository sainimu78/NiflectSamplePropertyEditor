#pragma once
#include "qmainwindow.h"
#include "Niflect/NiflectTable.h"
#include "qcombobox.h"
#include "Niflect/Base/SharedPtr.h"
#include "Property/PropertyNode.h"
#include "Property/PropertyEditContext.h"

class QPropertyTree;

class QExampleWindow : public QMainWindow
{
	typedef QMainWindow inherited;
public:
	QExampleWindow(QWidget* parentWidget = NULL);
	void InitTypesSelection(const Niflect::CNiflectTable* table);

private:
	QComboBox* m_cobCollection;
	Niflect::TArray<Niflect::CNiflectType*> m_vecType;
	QPropertyTree* m_propTree;
	Niflect::TSharedPtr<void*> m_editorDummy;
	Niflect::TSharedPtr<void*> m_runtimeDummy;
	CSharedPropertyNode m_propRoot;
	Niflect::CNiflectInstanceNode m_editorInstRoot;
	Niflect::CNiflectInstanceNode m_runtimeInstRoot;
	const Niflect::CNiflectType* m_lastSelectedType;
	CPropertyEditContext m_editCtx;
};