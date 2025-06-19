#pragma once
#include "qmainwindow.h"
#include "Niflect/NiflectTable.h"
#include "qcombobox.h"
#include "Niflect/Base/SharedPtr.h"
#include "Property/PropertyNode.h"
#include "Property/PropertyEditContext.h"
#include "Niflect/Component/RwTree/RwInstanceNode.h"

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
	Niflect::CRwInstanceNode m_editorInstRoot;
	Niflect::CRwInstanceNode m_runtimeInstRoot;
	const Niflect::CNiflectType* m_lastSelectedType;
	CPropertyEditContext m_editCtx;
};