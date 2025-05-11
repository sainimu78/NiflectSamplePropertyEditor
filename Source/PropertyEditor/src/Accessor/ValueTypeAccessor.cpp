#include "Accessor/ValueTypeAccessor.h"
#include "Niflect/Util/StringUtil.h"

bool CVector3Accessor::SaveInstanceImpl(const InstanceType* base, CRwNode* rw) const
{
	auto& instance = *static_cast<const CVector3*>(base);
	SaveCVector3ToRwNode(instance, rw);//表明可任意编解码
	return true;
}
bool CVector3Accessor::LoadInstanceImpl(InstanceType* base, const CRwNode* rw) const
{
	auto& instance = *static_cast<CVector3*>(base);
	ASSERT(rw->IsValue());
	LoadCVector3FromRwNode(rw, instance);
	return true;
}