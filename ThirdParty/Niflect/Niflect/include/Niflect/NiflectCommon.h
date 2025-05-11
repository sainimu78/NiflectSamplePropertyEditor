#pragma once
#include "Niflect/NiflectDevelopmentMacro.h"//���Ѽ��� PrecompileHeader, �����������ʱ���ܽ������ǩ����������ͷ�ļ������� PrecompileHeader �е��¿�����δ����ı������
#include <cstddef>//NULL
#define ASSERTION_ABORT
#ifdef ASSERTION_ABORT
#include <cassert>//assert
#else
#include <cstdio>//printf, fflush
#endif

enum { INDEX_NONE = -1 };

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef long long int64;
typedef unsigned int uint32;
typedef unsigned long long uint64;

namespace Niflect
{
    enum { INVALID_HASH = 0 };
    typedef uint64 HashInt;
}

#ifdef ASSERTION_ABORT
#ifndef NDEBUG
    #define ASSERT(b)\
        do { \
            bool cond = b;\
            if (!cond)\
            {\
                assert(false);\
            }\
        } while(0)
#else
    #define ASSERT(b) (void)0
#endif
#else
#if defined(_MSC_VER)
#define DEBUG_BREAK() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
#define DEBUG_BREAK() __builtin_trap()
#else
#define DEBUG_BREAK() (void)0  // ����ƽ̨�޲���
#endif

#ifndef NDEBUG
    #define ASSERT(b)\
        do { \
            bool cond = b;\
            if (!cond)\
            {\
                printf("Assertion failed: %s, %d\n", __FILE__, __LINE__);\
                fflush(stdout);\
                DEBUG_BREAK();\
            }\
        } while(0)
#else
    #define ASSERT(b) (void)0
#endif
#endif

#ifdef NIFLECT_API
#else
    #ifdef WIN32
        #ifdef NIFLECT_EXPORTS
            #define NIFLECT_API __declspec(dllexport)
        #else
            #define NIFLECT_API __declspec(dllimport)
        #endif
    #else
        #ifdef NIFLECT_EXPORTS
		    #define NIFLECT_API __attribute__((visibility("default")))
	    #else
		    #define NIFLECT_API 
	    #endif
    #endif
#endif

//Ϊʵ�ֶ�̬���͵�ʵ����, ͨ�� alignof �ɱ�����ȷ��ע�����͵Ķ����С
//�ֽ�Ϊ������ʵ��, ���ڷ��ֿ����� VS2022 + v140(Qt5.8) �������޷��Գ�����ʹ�� alignof, �ݲ�����
//��ע, �˻�����δ���� _MSVC_TOOLSET_VERSION, _MSC_VER Ϊ 1942, �ޱ�׼���������Ƿ�Ϊ v140
//�������˽��ƽ̨����, �������� alignof ֧�����, ��ͨ�û�ȡ�������ٿ��Ǹ�Ϊ��ʽ֧��
//�������ʵ��, ���ú꼴��. ����ʾ�޷�ʵ����������Ĵ�����Ӧ����
//#define DEVMACRO_GENERATED_TYPE_ALIGNMENT