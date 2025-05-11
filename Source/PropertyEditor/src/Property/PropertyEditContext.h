#pragma once
#include "Property/PropertyNode.h"

class CPropertyEditContext
{
	friend class QPropertyTree;
public:
	void MarkChanged(CPropertyNode* prop)
	{
		m_vecChanged.push_back(prop);
	}
	void MarkDeleted(const CSharedPropertyNode& prop)
	{
		//m_vecDeleted.push_back(prop);
	}

private:
	Niflect::TArray<CPropertyNode*> m_vecChanged;
	//Niflect::TArray<CSharedPropertyNode> m_vecDeleted;
};

namespace PropertyTreeUtil
{
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
}