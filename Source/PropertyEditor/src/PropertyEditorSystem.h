#pragma once
#include "Niflect/NiflectLoadTimeModuleRegistry.h"

class CPropertyEditorSystem
{
public:
	CPropertyEditorSystem();
	void Init();

public:
	Niflect::CNiflectTable* GetTable() const { return m_table; }

private:
	Niflect::CNiflectTable* m_table;
	Niflect::CNiflectModuleRegistry2 m_reg;
};