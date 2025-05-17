#pragma once
#include "ExampleNata.h"
#include "Niflect/Default/DefaultMacroTag.h"
#include "Niflect/Base/Array.h"
#include "ExampleStructure.h"

NIF_T(CExampleNata())
class CExample0
{
public:
	CExample0();

public:
	NIF_F()
	float m_value;
	NIF_F()
	std::vector<float> m_float_array;
	NIF_F()
	CVector3 m_vector3;
};

NIF_T(CExampleNata())
class CExample1 : public CExample0
{
public:
	CExample1();

public:
	NIF_F()
	std::vector<float> m_float_array_1;
	NIF_F()
	float m_value_1;
};