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
	}

private:
	Niflect::TArray<CPropertyNode*> m_vecChanged;
};