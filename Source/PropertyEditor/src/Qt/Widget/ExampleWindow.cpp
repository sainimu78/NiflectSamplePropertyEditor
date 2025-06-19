#include "Qt/Widget/ExampleWindow.h"
#include "qboxlayout.h"
#include "Qt/Util/CoreUtil.h"
#include "Qt/Widget/PropertyTree.h"
#include "Niflect/Serialization/JsonFormat.h"
#include "Niflect/Util/FileStreamUtil.h"
#include "Accessor.h"
#include "Niflect/Default/SharedInstance.h"

#include "Accessor/ValueTypeAccessor.h"
#include "Accessor/CompoundAccessor.h"
#include "Accessor/ContainerAccessor.h"

static void DebugPrintPropertyRecurs(CPropertyNode* prop, uint32 depth = 0)
{
	auto strDepth = NiflectUtil::DebugIndentToString(depth);
	auto propName = prop->m_name;
	if (propName.empty())
		propName = "Not a field";
	Niflect::CString propValue;
	if (prop->IsItem())
	{
		auto item = CPropertyItem::Cast(prop);
		CRwNode rw;
		item->LoadFromCurrent(&rw);
		ASSERT(rw.IsValue());
		switch (rw.GetValue()->GetType())
		{
		case ERwValueType::Float:
		{
			Niflect::CStringStream ss;
			ss << rw.GetValue()->GetFloat();
			propValue = ss.str();
			break;
		}
		case ERwValueType::String:
		{
			propValue = rw.GetValue()->GetString();
			break;
		}
		default:
			ASSERT(false);
			break;
		}
	}
	if (propValue.empty())
		propValue = "Not an item";
	printf("%s%s, %s\n", strDepth.c_str(), propName.c_str(), propValue.c_str());

	depth++;
	for (auto& it : prop->m_vecNode)
		DebugPrintPropertyRecurs(it.Get(), depth);
}

Niflect::TMap<Niflect::CNiflectType*, Niflect::CNiflectType*> g_mapAcsToPropType;

static void BuildPropertyFromRwNodeRecurs(CPropertyEditContext* editCtx, Niflect::CNiflectType* ownerType, const CRwNode* rw, const Niflect::CString& name, CPropertyNode* propParent, CPropertyNode* prop)
{
	prop->InitForNode(editCtx, propParent, name);
	prop->SaveToCurrent(rw);
	if (rw->IsValue())
	{
	}
	else if (rw->IsArray())
	{
		auto elemType = GetAccessor(ownerType)->GetElementType();
		ASSERT(elemType == NULL || rw->IsArray());
		auto rwArray = rw->GetArray();
		auto& propType = g_mapAcsToPropType.at(GetAccessorType(elemType));
		prop->InitForContainer(propType);
		for (uint32 idx = 0; idx < rwArray->GetItemsCount(); ++idx)
		{
			auto elemProp = Niflect::MakeSharedInstance<CPropertyNode>(propType);
			BuildPropertyFromRwNodeRecurs(editCtx, elemType, rwArray->GetItem(idx), std::to_string(idx).c_str(), prop, elemProp.Get());
			prop->AddNode(elemProp);
		}
	}
	else
	{
		Niflect::TArray<Niflect::CField> vecField;
		for (auto& it0 : ownerType->GetTypeLayout())
		{
			for (auto& it1 : it0->GetFields())
				vecField.push_back(it1);
		}
		ASSERT(vecField.size() == rw->GetNodesCount());
		for (uint32 idx = 0; idx < rw->GetNodesCount(); ++idx)
		{
			auto& field = vecField[idx];
			auto fieldType = field.GetType();
			auto& propType = g_mapAcsToPropType.at(GetAccessorType(fieldType));
			auto fieldProp = Niflect::MakeSharedInstance<CPropertyNode>(propType);
			BuildPropertyFromRwNodeRecurs(editCtx, fieldType, rw->GetNode(idx), field.GetName(), prop, fieldProp.Get());
			prop->AddNode(fieldProp);
		}
	}
}
static CSharedPropertyNode BuildPropertyFromRwNode(CPropertyEditContext* editCtx, Niflect::CNiflectType* ownerType, const CRwNode* rw)
{
	auto& propType = g_mapAcsToPropType.at(GetAccessorType(ownerType));
	auto prop = Niflect::MakeSharedInstance<CPropertyNode>(propType);
	BuildPropertyFromRwNodeRecurs(editCtx, ownerType, rw, "", NULL, prop.Get());
	return prop;
}
static bool SavePropertyToRwNodeRecurs(CPropertyNode* prop, CRwNode* rw)
{
	if (prop->IsItem())
	{
		prop->LoadFromCurrent(rw);
	}
	else
	{
		if (prop->IsContainer())
		{
			auto rwArray = rw->ToArray();
			for (auto& it : prop->m_vecNode)
			{
				auto rwElem = CreateRwNode();
				if (SavePropertyToRwNodeRecurs(it.Get(), rwElem.Get()))
					rwArray->AddItem(rwElem);
			}
		}
		else
		{
			for (auto& it : prop->m_vecNode)
			{
				auto rwField = CreateRwNode();
				if (SavePropertyToRwNodeRecurs(it.Get(), rwField.Get()))
					AddExistingRwNode(rw, it->m_name, rwField);
			}
		}
	}
	return true;
}
static bool SavePropertyToRwNode(CPropertyNode* prop, CRwNode* rw)
{
	return SavePropertyToRwNodeRecurs(prop, rw);
}

QExampleWindow::QExampleWindow(QWidget* parentWidget)
	: inherited(parentWidget)
	, m_cobCollection(NULL)
	, m_propTree(NULL)
	, m_lastSelectedType(NULL)
{
	g_mapAcsToPropType.insert({ Niflect::StaticGetType<CFloatAccessor>(), Niflect::StaticGetType<CFloatPropertyLineEdit>() });
	g_mapAcsToPropType.insert({ Niflect::StaticGetType<CCompoundAccessor>(), Niflect::StaticGetType<CCompoundPropertyCollapser>() });
	g_mapAcsToPropType.insert({ Niflect::StaticGetType<CArrayAccessor>(), Niflect::StaticGetType<CArrayPropertyAddRemoveDeleteInsert>() });
	g_mapAcsToPropType.insert({ Niflect::StaticGetType<CVector3Accessor>(), Niflect::StaticGetType<CVector3PropertyLineEditGroup>() });

	auto wdgCentral = new QWidget(this);
	this->setCentralWidget(wdgCentral);
	auto mainLayout = new QHBoxLayout(wdgCentral);
	auto wdgLeft = new QWidget(this);
	wdgLeft->setFixedWidth(200);
	mainLayout->addWidget(wdgLeft);

	auto leftLayout = new QVBoxLayout(wdgLeft);
	leftLayout->setAlignment(Qt::AlignmentFlag::AlignVCenter);

	auto cobCollection = new QComboBox(this);
	QObject::connect(cobCollection, &QComboBox::currentTextChanged, [this]()
		{
			auto& type = m_vecType[m_cobCollection->currentIndex()];
			CRwNode rwInitialRuntimeData;
			{
				m_runtimeDummy = Niflect::MakeSharedInstance<void*>(type);
				SaveInstanceToRwNode(type, m_runtimeDummy.Get(), &rwInitialRuntimeData);
				m_runtimeInstRoot.Clear();
				m_runtimeInstRoot.InitAndBuild(type, m_runtimeDummy.Get());
			}
			{
				m_propRoot = BuildPropertyFromRwNode(&m_editCtx, type, &rwInitialRuntimeData);
				DebugPrintPropertyRecurs(m_propRoot.Get());
				auto uiNodeRoot = m_propTree->GetUiNodeRoot();
				uiNodeRoot->m_caption = type->GetTypeName();
				m_propTree->RebuildRows(m_propRoot.Get(), uiNodeRoot);
			}
		});
	leftLayout->addWidget(cobCollection);
	m_cobCollection = cobCollection;

	auto btnPrint = new QPushButton("Print", this);
	QObject::connect(btnPrint, &QPushButton::clicked, [this]()
		{
			printf("----------------\n");
			DebugPrintPropertyRecurs(m_propRoot.Get());
		});
	leftLayout->addWidget(btnPrint);

	m_propTree = new QPropertyTree(this);
	m_propTree->Init(&m_editCtx);
	m_propTree->SetOnChangeFromUiAppliedFunc([this](CPropertyNode* prop)
		{
			Niflect::TArray<Niflect::CString> vecPath;
			CRwNode rw;
			{
				Niflect::BuildInstanceNodeNamePath(prop, vecPath);
				SavePropertyToRwNode(prop, &rw);
			}
			{
				auto runtimeInstNode = Niflect::VisitInstanceNodeByNamePath(&m_runtimeInstRoot, vecPath);
				runtimeInstNode->Load(&rw);
				if (!prop->IsItem())
				{
					runtimeInstNode->Clear();
					runtimeInstNode->Build();
				}
			}
			{
				CRwNode rw;
				SaveInstanceToRwNode(m_runtimeInstRoot.GetType(), m_runtimeDummy.Get(), &rw);
				std::ofstream ofs;
				if (NiflectUtil::OpenFileStream(ofs, "a.json"))
					CJsonFormat::Write(&rw, ofs);
			}
		});
	mainLayout->addWidget(m_propTree);

	//m_propRoot = Niflect::MakeShared<CCompoundPropertyCollapser>();
	//m_propRoot->InitForNode(&m_editCtx, NULL, "");
	//auto field0 = Niflect::MakeShared<CFloatPropertyLineEdit>();
	//field0->InitForNode(&m_editCtx, m_propRoot.Get(), "field0");
	//m_propRoot->AddNode(field0);
	//field0->SetValue(1.23f);
	//auto field1 = Niflect::MakeShared<CArrayPropertyAddRemoveDeleteInsert>();
	//field1->InitForNode(&m_editCtx, m_propRoot.Get(), "field1");
	//field1->InitForContainer(Niflect::StaticGetType<CFloatPropertyLineEdit>());
	//m_propRoot->AddNode(field1);
	//field1->Resize(3);
	//DebugPrintPropertyRecurs(m_propRoot.Get());
	//auto uiNodeRoot = m_propTree->GetUiNodeRoot();
	//m_propTree->RebuildRows(m_propRoot.Get(), uiNodeRoot);
}
void QExampleWindow::InitTypesSelection(const Niflect::CNiflectTable* table)
{
	for (uint32 idx = 0; idx < table->GetTypesCount(); ++idx)
	{
		auto type = table->GetTypeByIndex(idx);
		if (type->GetNata() != NULL)
		{
			m_vecType.push_back(type);
			m_cobCollection->addItem(CStringToQString(type->GetTypeName()));
		}
	}
}