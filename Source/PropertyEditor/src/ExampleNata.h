#pragma once
#include "Niflect/NiflectNata.h"

enum class ETypeCategory
{
	Default,
	Reserved,
};

class CExampleNata : public Niflect::CNata
{
	typedef CExampleNata CThis;
public:
	CExampleNata()
		: m_typeCategory(ETypeCategory::Default)
	{

	}
	CThis& SetCategory(ETypeCategory cat)
	{
		m_typeCategory = cat;
		return *this;
	}
	ETypeCategory m_typeCategory;
};