#pragma once

#include <Windows.h>

class ProcessHelper
{
public:
	ProcessHelper();
	~ProcessHelper();

	static HANDLE getProcessHandleByName(const wchar_t *processName);

	static HMODULE getProcessModuleHandleByName(HANDLE processHandle, const wchar_t *moduleName);
};

