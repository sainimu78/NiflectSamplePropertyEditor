#pragma once
#include "Niflect/Serialization/RwTree.h"

using namespace RwTree;

class CVector3
{
public:
	CVector3()
		: m_x(0.0f)
		, m_y(0.0f)
		, m_z(0.0f)
	{
	}
	float m_x;
	float m_y;
	float m_z;
};

void SaveCVector3ToRwNode(const CVector3& vec, CRwNode* rw);
void LoadCVector3FromRwNode(const CRwNode* rw, CVector3& vec);