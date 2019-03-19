#include <iostream>
#include "ProcessHelper.h"
#include <Windows.h>
#include <Psapi.h>

int main()
{
	// Process Handle
	const wchar_t processName[] = L"hl.exe";
	HANDLE processHandle = ProcessHelper::getProcessHandleByName(processName);
	if (processHandle == INVALID_HANDLE_VALUE)
	{
		std::cout << "A process handle has not been found" << std::endl;
		return 1;
	}

	std::cout << "A process handle has been found!" << std::endl;

	// Module handle
	const wchar_t moduleName[] = L"c:\\program files (x86)\\counter strike 1.6 magesy\\cstrike\\cl_dlls\\client.dll";
	HMODULE moduleHandle = ProcessHelper::getProcessModuleHandleByName(processHandle, moduleName);
	if (moduleHandle == (HMODULE) INVALID_HANDLE_VALUE)
	{
		std::cout << "A module handle has not been found" << std::endl;
		return 1;
	}

	std::cout << "A module handle has been found!" << std::endl;

	// Module information
	MODULEINFO moduleInformation;
	if (GetModuleInformation(processHandle, moduleHandle, &moduleInformation, sizeof(moduleInformation)) == 0)
	{
		std::cout << "Couldn't get module information" << std::endl;
		return 1;
	}

	std::cout << "Module information has been successfully got!" << std::endl;
	std::cout << "A load address of the module is 0x" << std::hex << moduleInformation.lpBaseOfDll << std::endl;
	std::cout << "A size of a space the module has occupied is " << moduleInformation.SizeOfImage << std::endl;
	std::cout << "An entry point of the module is 0x" << std::hex << moduleInformation.EntryPoint << std::endl;

	// Read process memory
	unsigned long playerHealthOffsetFromClientDll = 0xF4B04;
	// TODO: hmm
	uintptr_t playerHealthPtr = (uintptr_t)((char *)moduleInformation.lpBaseOfDll + playerHealthOffsetFromClientDll);

	std::cout << "Player health at 0x" << std::hex << playerHealthPtr << std::endl;
	int playerHealth = 0;
	if (ReadProcessMemory(processHandle, (void *)playerHealthPtr, &playerHealth, sizeof(playerHealth), 0) == 0)
	{
		std::cout << "Couldn't read process memory" << std::endl;
		return 1;
	}

	std::cout << "Player's health is " << std::dec << playerHealth << std::endl;

	return 0;
	// uintptr_t playerHealthPointsPtr = (char *)
	// ReadProcessMemory(processHandle, )
	// Write process memory
}
