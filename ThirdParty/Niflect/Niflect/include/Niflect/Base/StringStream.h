#pragma once
#include "Niflect/Base/Memory.h"
#include <sstream>

namespace Niflect
{
	//begin, �ɵ�ʵ��, �ɼ���std::string, ʹ��Reflector::CMemoryStats����, �����޷����û����һЩoperator=���¶�����ͷ�, ��ʱ��������, �ƻ�ɾ��
	//template <typename TAllocator>
	//class TBasicString : public std::basic_string<char, std::char_traits<char>, TAllocator>
	//{
	//	typedef std::basic_string<char, std::char_traits<char>, TAllocator> inherited;
	//public:
	//	TBasicString()
	//	{
	//	}
	//	//֧��ִ�л��ຯ�����ػ�������, �Լ�֧�ֲ�ͬAllocator
	//	template <typename TOtherAllocator>
	//	TBasicString(const std::basic_string<char, std::char_traits<char>, TOtherAllocator>& rhs)
	//		: inherited(rhs)
	//	{
	//	}
	//	TBasicString(const char* psz)
	//		: inherited(psz)
	//	{
	//	}
	//	TBasicString(const std::string& str)
	//		: inherited(str.c_str())
	//	{
	//	}
	//	//Ϊ�滻Ϊstd::string����ʹ��std::string���׵Ľӿ�, ����c_str()�ܱ���ͨ�����ܵ���΢С����, �����Ҫ��ȷʹ�ô��Ƿ�Ӧ��ʹ��Ϊc_str
	//	const char* GetRaw() const
	//	{
	//		return this->c_str();
	//	}
	//};
	//end

	template <typename TChar, typename TAllocator>
	using TSTLStringStream = std::basic_stringstream<TChar, std::char_traits<TChar>, TAllocator>;

	using CStringStream = TSTLStringStream<char, THeapAllocator<char> >;

}