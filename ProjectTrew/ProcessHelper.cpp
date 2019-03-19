#include "ProcessHelper.h"
#include <iostream>
#include <TlHelp32.h>
#include <Psapi.h>
#include <Psapi.h>

ProcessHelper::ProcessHelper()
{
}


ProcessHelper::~ProcessHelper()
{
}

/**
 * This function returns a handle (HANDLE) for the specified process name.
 * If there is no process with the provided name, the function will return
 * INVALID_HANDLE_VALUE.
 * 
 * TODO: Should I use memory allocation (malloc / new) for MODULE \ HMODULE
 * TODO: types?
 */
HANDLE ProcessHelper::getProcessHandleByName(const wchar_t *processName)
{
	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
	{
		std::cout << "Couldn't get a list of processes" << std::endl;
		return INVALID_HANDLE_VALUE;
	}

	PROCESSENTRY32 processInformationStruct;
	PROCESSENTRY32 *processInformationStructPtr = &processInformationStruct;

	/**
	 * We must do it because of the documentation
	 * https://docs.microsoft.com/ru-ru/windows/desktop/api/tlhelp32/nf-tlhelp32-process32first
	 */
	processInformationStruct.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(processesSnapshot, processInformationStructPtr))
	{
		std::cout << "Couldn't iterate on a snapshot of processes" << std::endl;
		return INVALID_HANDLE_VALUE;
	}

	do
	{
		if (wcscmp(processName, processInformationStruct.szExeFile) == 0)
		{
			// PROCESS_VM_READ | PROCESS_VM_WRITE
			HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processInformationStruct.th32ProcessID);
			if (processHandle == NULL)
			{
				std::cout << "Process has been found but I couldn't open it. Its process id is " << processInformationStruct.th32ProcessID << std::endl;
				return INVALID_HANDLE_VALUE;
			}

			return processHandle;
		}
	} while (Process32Next(processesSnapshot, processInformationStructPtr));

	std::cout << "Process has not been found" << std::endl;
	return INVALID_HANDLE_VALUE;
}

/**
 * This function returns a handle (HMODULE) for the specified module name in
 * the specified process.
 * 
 * If there is no module module with the provided name, the function will return
 * (HMODULE) INVALID_HANDLE_VALUE;
 * 
 * TODO: Should I use memory allocation (malloc / new) for MODULE \ HMODULE
 * TODO: types?
 */
HMODULE ProcessHelper::getProcessModuleHandleByName(HANDLE processHandle, const wchar_t* moduleName)
{
	HMODULE modulesHandles[1024];
	unsigned long countOfBytesToStoreModulesHandles;

	HMODULE errReturnValue = (HMODULE)INVALID_HANDLE_VALUE;

	if (EnumProcessModulesEx(processHandle, modulesHandles, sizeof(modulesHandles), &countOfBytesToStoreModulesHandles, LIST_MODULES_ALL) == FALSE)
	{
		std::cout << "Couldn't get modules handles of the process" << std::endl;
		return errReturnValue;
	}

	unsigned long modulesCount = countOfBytesToStoreModulesHandles / sizeof(HMODULE);
	for (unsigned long i = 0; i < modulesCount; i++)
	{
		HMODULE currentModuleHandle = modulesHandles[i];
		wchar_t currentModuleName[MAX_PATH];

		if (GetModuleFileNameEx(processHandle, currentModuleHandle, currentModuleName, MAX_PATH) == NULL)
		{
			std::cout << "Couldn't get the name of a module" << std::endl;
			return errReturnValue;
		}

		/**
		 * TODO: Leave only the last part of the module path
		 * TODO: Example> C:\one two three\client.dll to client.dll
		 */

		if (wcscmp(moduleName, currentModuleName) == 0)
		{
			return currentModuleHandle;
		}
	}

	std::cout << "The specified module has not been found" << std::endl;
	return errReturnValue;
}
