#ifndef DEFINITIONS_H
#define DEFINITIONS_H
#include <Windows.h>
#include <cstdint>
#if ROGUE_DEBUG
#include <cstdlib>
#include <crtdbg.h>
#include <cstdio>
#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#define INTERNAL static
#define GLOBAL static
#define LOCAL_PERSIST static

#define ASSERT(exp) {if(!exp){int* a = 0; *a = 0;}}

#define ARRAY_SIZE(arr) sizeof(arr) / sizeof(arr[0])

#if ROGUE_DEBUG
#define DEBUG_LOG(format, ...)\
{\
    char buf[512] = {};\
    sprintf(buf, format, __VA_ARGS__);\
    OutputDebugString(buf);\
}
#define LEAK_CHECKS()\
_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);\
_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);\
_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);\
_CrtSetBreakAlloc(-1);
#else
#define DEBUG_LOG(format, ...)
#define LEAK_CHECKS()
#endif


#endif//DEFINITIONS_H
