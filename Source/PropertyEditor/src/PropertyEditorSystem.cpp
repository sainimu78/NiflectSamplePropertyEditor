#include "PropertyEditorSystem.h"
#include "PropertyEditor_private.h"

CPropertyEditorSystem::CPropertyEditorSystem()
	: m_table(NULL)
{

}
void CPropertyEditorSystem::Init()
{
	auto reg = Niflect::GetModuleRegistry();
	reg->InitRegisteredModules();

	ASSERT(reg->GetModulesCount() == 1);
	auto mod = reg->GetModuleByIndex(0);
	m_table = mod->GetTable();
}