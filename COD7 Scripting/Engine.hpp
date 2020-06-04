#pragma once
#include <stdarg.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <string>
using PrintError_t = void(__cdecl*)(int arg0, char* formatStr, ...);
PrintError_t PrintError; 
std::stringstream g_errors;
void LogPrintMsg(int outputChannel, char* formatStr, ...)
{
	va_list va;
	va_start(va, formatStr);
	char errorBuf[0x1000u] = {};
	_vsnprintf_s(errorBuf, 0x1000u, formatStr, va);
	g_errors << errorBuf << '\n';
	va_end(va);
	//PrintError(outputChannel, formatStr, va);
}
