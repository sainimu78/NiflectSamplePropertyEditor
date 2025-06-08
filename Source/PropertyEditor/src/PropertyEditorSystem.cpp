#include "PropertyEditorSystem.h"
#include "PropertyEditor_private.h"

CPropertyEditorSystem::CPropertyEditorSystem()
	: m_table(NULL)
{

}
void CPropertyEditorSystem::Init()
{
	m_reg.InitLoadTimeModules();
	ASSERT(m_reg.GetModulesCount() == 1);
	auto mod = m_reg.GetModuleByIndex(0);
	m_table = mod->GetTable();
}