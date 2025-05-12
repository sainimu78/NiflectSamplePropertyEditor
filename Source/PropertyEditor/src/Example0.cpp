#include "Example0.h"

CExample0::CExample0()
	: m_value(1.23f)
{
	m_float_array.resize(3);
	m_vector3.m_x = 2.34f;
	m_vector3.m_y = 3.45f;
	m_vector3.m_z = 4.56f;
}

CExample1::CExample1()
	: m_value_1(5.67f)
{
	m_float_array_1.resize(2);
}