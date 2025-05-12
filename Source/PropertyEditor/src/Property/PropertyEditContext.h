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