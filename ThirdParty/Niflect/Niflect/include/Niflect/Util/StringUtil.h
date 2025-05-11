#pragma once
#include "Niflect/Base/String.h"
#include "Niflect/Base/Array.h"
#include <stdarg.h>
#include <fstream>
#include <algorithm>
#include <cctype>

namespace NiflectUtil
{
	//static Niflect::CString FormatString(const char* format, ...)
	//{
	//	char szContent[10240] = "";
	//	va_list va_alist;
	//	va_start(va_alist, format);
	//	vsnprintf(szContent, 10240, format, va_alist);
	//	va_end(va_alist);
	//	return szContent;
	//}
	//static std::string FormatStdString(const char* format, ...)
	//{
	//	char szContent[10240] = "";
	//	va_list va_alist;
	//	va_start(va_alist, format);
	//	vsnprintf(szContent, 10240, format, va_alist);
	//	va_end(va_alist);
	//	return szContent;
	//}

	static Niflect::CString FormatString(const char* format, ...)
	{
		va_list args;

		// ��һ�λ�ȡ��ʽ������
		va_start(args, format);
		const int length = std::vsnprintf(nullptr, 0, format, args);
		va_end(args);

		if (length <= 0) return Niflect::CString();  // ������Ч��ʽ������

		// ׼���㹻��С�Ļ�����
		Niflect::CString result;
		result.resize(static_cast<size_t>(length));

		// ʵ��ִ�и�ʽ��
		va_start(args, format);
		std::vsnprintf(&result[0], result.size() + 1, format, args); // +1����ֹ�����ռ�
		va_end(args);

		return result;
	}
	static float StringToFloat(const Niflect::CString& str)
	{
        return static_cast<float>(atof(str.c_str()));
	}
	static bool StartsWith(const Niflect::CString& fullString, const Niflect::CString& start)
	{
		if (fullString.length() >= start.length())
		{
			return (0 == fullString.compare(0, start.length(), start));
		}
		return false;
	}
	static bool EndsWith(const Niflect::CString& fullString, const char& ending)
	{
		if (fullString.length() >= 1)
			return fullString.back() == ending;
		return false;
	}
	static bool EndsWith(const Niflect::CString& fullString, const Niflect::CString& ending)
	{
		if(fullString.length() >= ending.length())
			return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
		return false;
	}
	static Niflect::CString ReplaceFilePathExt(const Niflect::CString& filePath, const Niflect::CString& a, const Niflect::CString& b)
	{
		auto pos = filePath.find_last_of(a);
		if (pos != std::string::npos)
		{
			auto result = filePath.substr(0, pos - (a.length() - 1));
			result += b;
			return result;
		}
		return filePath;
	}
	static Niflect::CString RemoveFileExt(const Niflect::CString& filePath, const Niflect::CString& ext = ".")
	{
		auto pos = filePath.find_last_of(ext);
		if (pos != std::string::npos)
		{
			auto result = filePath.substr(0, pos);
			return result;
		}
		return filePath;
	}
	static bool GetFileExtSafe(const Niflect::CString& filePath, Niflect::CString& extracted, const Niflect::CString& ext = ".")
	{
		auto pos = filePath.find_last_of(ext);
		if (pos != std::string::npos && pos != filePath.length() - 1)
		{
			extracted = filePath.substr(pos);
			return true;
		}
		return false;
	}
	static Niflect::CString GetFileExt(const Niflect::CString& filePath, const Niflect::CString& ext = ".")
	{
		Niflect::CString extracted;
		GetFileExtSafe(filePath, extracted, ext);
		return extracted;
	}
	static Niflect::CString ConvertToSearchPath(const Niflect::CString& path)
	{
		if (path.back() != '/')
			return path + '/';
		return path;
	}
	static Niflect::CString ConcatPath(const Niflect::CString& a, const Niflect::CString& b)
	{
		constexpr const int JoinABWithSlash = 0;
		constexpr const int ATrailingSlash = 1;
		constexpr const int BLeadingSlash = 2;
		constexpr const int BothHaveSlash = 3;
		constexpr const int BothEmpty = 4;

		auto mode = JoinABWithSlash;
		if (!a.empty() && !b.empty())
		{
			if (a.back() == '/')
				mode = ATrailingSlash;
			if (b[0] == '/')
			{
				if (mode != ATrailingSlash)
					mode = BLeadingSlash;
				else
					mode = BothHaveSlash;
			}
		}
		else if (a.empty() && !b.empty())
		{
			return b;
		}
		else if (!a.empty() && b.empty())
		{
			return a;
		}
		else
		{
			mode = BothEmpty;
		}

		Niflect::CString result;
		switch (mode)
		{
		case JoinABWithSlash:
		{
			result = a + '/' + b;
			break;
		}
		case ATrailingSlash:
		case BLeadingSlash:
		{
			result = a + b;
			break;
		}
		case BothHaveSlash:
		{
			result = b.substr(1, b.length() - 1);
			result = a + result;
			break;
		}
		default:
			break;
		}
		return result;
	}
	static bool GetParentDirPathSafe(const Niflect::CString& fileOrDirPath, Niflect::CString& parentDirPath)
	{
		auto posSlash = fileOrDirPath.find_last_of('/');
		if (posSlash != std::string::npos)
		{
			parentDirPath = fileOrDirPath.substr(0, posSlash);
			return true;
		}
		return false;
	}
	static Niflect::CString GetParentDirPath(const Niflect::CString& fileOrDirPath)
	{
		Niflect::CString parentDirPath;
		if (GetParentDirPathSafe(fileOrDirPath, parentDirPath))
			return parentDirPath;
		return parentDirPath;
	}
	static Niflect::CString GetFileName(const Niflect::CString& filePath)
	{
		auto posSlash = filePath.find_last_of('/');
		if (posSlash != std::string::npos)
			return filePath.substr(posSlash + 1, filePath.length() - posSlash);
		return filePath;
	}
	static Niflect::CString GetDirName(const Niflect::CString& dirPath)
	{
		return GetFileName(dirPath);
	}
	static void WriteStringToFile(const Niflect::CString& data, const Niflect::CString& filePath)
	{
		std::ofstream ofs(filePath.c_str(), std::ios::binary);
		if (!ofs.is_open())
		{
			printf("%s\n", filePath.c_str());
			ASSERT(false);
		}
		ofs << data;
		ofs.close();
	}
	static Niflect::CString ReadStringFromFile(const Niflect::CString& filePath)
	{
		std::ifstream ifs(filePath.c_str(), std::ios::binary);
		char buffer[1024];
		Niflect::CString data;
		while (ifs.read(buffer, sizeof(buffer)))
			data.append(buffer, ifs.gcount());
		data.append(buffer, ifs.gcount());
		return data;
	}
	static Niflect::CString ConvertToUpperCase(Niflect::CString str)
	{
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::toupper(c); });
		return str;
	}
	static bool CompareLessHumanReadable(const Niflect::CString& a, const Niflect::CString& b)
	{
		Niflect::CString lowerA = a;
		Niflect::CString lowerB = b;
		std::transform(lowerA.begin(), lowerA.end(), lowerA.begin(), ::tolower);
		std::transform(lowerB.begin(), lowerB.end(), lowerB.begin(), ::tolower);
		return lowerA < lowerB;
	}
	static Niflect::TArray<Niflect::CString> Split(const Niflect::CString& str, char delimiter) {
		Niflect::TArray<Niflect::CString> tokens;  // ���ڴ洢�ָ������ַ���
		Niflect::CString token;                // ��ʱ�洢ÿ�����ַ���
		for (char ch : str) {             // ���������ַ����е�ÿ���ַ�
			if (ch == delimiter) {        // �����ǰ�ַ��Ƿָ���
				if (!token.empty()) {     // ������ʱ���ַ�����Ϊ��
					tokens.push_back(token); // ����ʱ���ַ�����ӵ����������
					token.clear();        // �����ʱ���ַ���
				}
			}
			else {
				token += ch;              // �����ǰ�ַ����Ƿָ�����������ӵ���ʱ���ַ�����
			}
		}
		if (!token.empty()) {             // ������һ����ʱ���ַ�����Ϊ��
			tokens.push_back(token);      // ������ӵ����������
		}
		return tokens;                    // ���طָ������ַ�������
	}
	static Niflect::TArray<Niflect::CString> Split(const Niflect::CString& str, const Niflect::CString& delimiter) {
		Niflect::TArray<Niflect::CString> tokens;  // ���ڴ洢�ָ������ַ���
		size_t start = 0;                 // ��ʼλ��
		size_t end = 0;                   // ����λ��

		while ((end = str.find(delimiter, start)) != std::string::npos) {
			// �ҵ��ָ���λ��
			tokens.push_back(str.substr(start, end - start)); // ��ȡ���ַ�������ӵ����������
			start = end + delimiter.length();                 // ���¿�ʼλ��
		}
		tokens.push_back(str.substr(start)); // ������һ�����ַ���

		return tokens;                       // ���طָ������ַ�������
	}
	static Niflect::TArray<Niflect::CString> SplitPath(const Niflect::CString& path)
	{
		return NiflectUtil::Split(path, '/');
	}
	static Niflect::CString CombineFromStrings(const Niflect::TArray<Niflect::CString>& vecPath, const Niflect::CString& delim)
	{
		Niflect::CString str;
		for (uint32 idx = 0; idx < vecPath.size(); ++idx)
		{
			str += vecPath[idx];
			if (idx != vecPath.size() - 1)
				str += delim;
		}
		return str;
	}
	static Niflect::CString CombineFromStrings(const Niflect::TArray<Niflect::CString>& vecPath, char delim)
	{
		Niflect::CString str(1, delim);
		return CombineFromStrings(vecPath, str);
	}
	static Niflect::CString Trim(const Niflect::CString& str)
	{
		auto is_space = [](unsigned char c) { return std::isspace(c); };

		// �ҵ���һ���ǿհ��ַ���λ��
		auto start = std::find_if_not(str.begin(), str.end(), is_space);
		// �ҵ����һ���ǿհ��ַ���λ��
		auto end = std::find_if_not(str.rbegin(), str.rend(), is_space).base();

		// �����޼�����ַ���
		return (start < end) ? Niflect::CString(start, end) : Niflect::CString();
	}
	static uint64 StableHash(const Niflect::CString& str) {
		constexpr uint64 FNV_OFFSET_BASIS = 0xcbf29ce484222325;
		constexpr uint64 FNV_PRIME = 0x100000001b3;
		uint64 hash = FNV_OFFSET_BASIS;
		for (char c : str) {
			hash ^= static_cast<uint64>(static_cast<uint8>(c));
			hash *= FNV_PRIME;
		}
		return hash;
	}
}