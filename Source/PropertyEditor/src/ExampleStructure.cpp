#include "ExampleStructure.h"
#include "Niflect/Util/StringUtil.h"

void SaveCVector3ToRwNode(const CVector3& vec, CRwNode* rw)
{
	auto str = NiflectUtil::FormatString("%f %f %f", vec.m_x, vec.m_y, vec.m_z);
	SetRwValueAs<Niflect::CString>(rw->ToValue(), str);
}
void LoadCVector3FromRwNode(const CRwNode* rw, CVector3& vec)
{
	auto str = GetRwValueAs<Niflect::CString>(rw->GetValue());
	auto vecEntryStr = NiflectUtil::Split(str, ' ');
	vec.m_x = atof(vecEntryStr[0].c_str());
	vec.m_y = atof(vecEntryStr[1].c_str());
	vec.m_z = atof(vecEntryStr[2].c_str());
}