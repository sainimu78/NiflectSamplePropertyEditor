#include "Property/PropertyNode.h"
#include "Property/PropertyEditContext.h"

void CPropertyNode::InsertNode(const CSharedPropertyNode& node, uint32 idx)
{
	m_editCtx->MarkChanged(node.Get());
	m_vecNode.insert(m_vecNode.begin() + idx, node);
}
void CPropertyNode::AddNode(const CSharedPropertyNode& node)
{
	this->InsertNode(node, this->GetNodesCount());
}
void CPropertyNode::DeleteNode(uint32 idx)
{
	m_editCtx->MarkDeleted(m_vecNode[idx]);
	m_vecNode.erase(m_vecNode.begin() + idx);
}
void CPropertyNode::DeleteLastNode()
{
	this->DeleteNode(this->GetNodesCount() - 1);
}
void CPropertyNode::Clear()
{
	for (auto& it : m_vecNode)
		m_editCtx->MarkDeleted(it);
	m_vecNode.clear();
}

CBoolProperty::CBoolProperty()
	: m_value(false)
{
}
void CBoolProperty::SetValue(bool value)
{
	m_value = value;
}
bool CBoolProperty::GetValue() const
{
	return m_value;
}
bool CBoolProperty::SaveToCurrentImpl(const CRwNode* rw)
{
	m_value = GetRwValueAs<bool>(rw->GetValue());
	return true;
}
bool CBoolProperty::LoadFromCurrentImpl(CRwNode* rw) const
{
	SetRwValueAs<bool>(rw->ToValue(), m_value);
	return true;
}

CFloatProperty::CFloatProperty()
	: m_value(0.0f)
{
}
void CFloatProperty::SetValue(float value)
{
	m_value = value;
}
float CFloatProperty::GetValue() const
{
	return m_value;
}
bool CFloatProperty::SaveToCurrentImpl(const CRwNode* rw)
{
	m_value = GetRwValueAs<float>(rw->GetValue());
	return true;
}
bool CFloatProperty::LoadFromCurrentImpl(CRwNode* rw) const
{
	SetRwValueAs<float>(rw->ToValue(), m_value);
	return true;
}

void CArrayProperty::AddElement()
{
	this->InternalInsertElement(this->GetElementsCount());
	this->UpdateElementsName();
}
void CArrayProperty::DeleteLastElement()
{
	this->InternalDeleteElement(this->GetElementsCount() - 1);
	this->UpdateElementsName();
}
void CArrayProperty::InsertElement(uint32 idx)
{
	this->InternalInsertElement(idx);
	this->UpdateElementsName();
}
void CArrayProperty::DeleteElement(uint32 idx)
{
	this->InternalDeleteElement(idx);
	this->UpdateElementsName();
}
uint32 CArrayProperty::GetElementsCount() const
{
	return this->GetNodesCount();
}
void CArrayProperty::Resize(uint32 cnt)
{
	int32 diff = this->GetElementsCount() - cnt;
	while (diff > 0)
	{
		this->InternalDeleteElement(this->GetElementsCount() - 1);
		diff--;
	}
	diff = cnt - this->GetElementsCount();
	while (diff > 0)
	{
		this->InternalInsertElement(this->GetElementsCount());
		diff--;
	}
	ASSERT(this->GetElementsCount() == cnt);
	this->UpdateElementsName();
}
void CArrayProperty::InternalInsertElement(uint32 idx)
{
	ASSERT(m_elemType != NULL);
	auto elem = Niflect::NiflectTypeMakeShared<CPropertyNode>(m_elemType);
	elem->InitForNode(m_editCtx, this, "");
	this->InsertNode(elem, idx);
}
void CArrayProperty::InternalDeleteElement(uint32 idx)
{
	this->DeleteNode(idx);
}
void CArrayProperty::UpdateElementsName()
{
	for (uint32 idx = 0; idx < m_vecNode.size(); ++idx)
	{
		auto& it = m_vecNode[idx];
		it->m_name = std::to_string(idx).c_str();
	}
}
bool CArrayProperty::SaveToCurrentImpl(const CRwNode* rw)
{
	auto cnt = GetRwValueAs<uint32>(rw->GetValue());
	this->Resize(cnt);
	return true;
}
bool CArrayProperty::LoadFromCurrentImpl(CRwNode* rw) const
{
	//Element 初始值也作为数组属性默认值的罕见需求是可行的, 但须区分保存载入的是否作默认值, 流程上显得繁琐.
	SetRwValueAs<uint32>(rw->ToValue(), this->GetElementsCount());
	return true;
}

void CVector3Property::SetValue(const CVector3& value)
{
	m_value = value;
}
const CVector3& CVector3Property::GetValue() const
{
	return m_value;
}
void CVector3Property::SetEntry(float entry, uint32 idx)
{
	ASSERT(sizeof(m_value) == sizeof(float) * 3);
	auto vec = static_cast<float*>(&m_value.m_x);
	vec[idx] = entry;
}
const float& CVector3Property::GetEntry(uint32 idx) const
{
	ASSERT(sizeof(m_value) == sizeof(float) * 3);
	auto vec = static_cast<const float*>(&m_value.m_x);
	return vec[idx];
}
bool CVector3Property::SaveToCurrentImpl(const CRwNode* rw)
{
	ASSERT(rw->IsValue());
	LoadCVector3FromRwNode(rw, m_value);//需要确保使用相同的编解码方法, 但不要求保存使用同变量类型, 如 m_value 可定义为 float m_value3
	return true;
}
bool CVector3Property::LoadFromCurrentImpl(CRwNode* rw) const
{
	SaveCVector3ToRwNode(m_value, rw);
	return true;
}