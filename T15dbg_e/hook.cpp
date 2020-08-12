#include "pch.h"
#include "hook.h"

void hook(void* originalFunction, void* hookFunction, BYTE* oldCode)
{
	JmpCode code((uintptr_t)originalFunction, (uintptr_t)hookFunction);
	DWORD oldProtect, oldProtect2;
	VirtualProtect(originalFunction, sizeof(code), PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(oldCode, originalFunction, sizeof(code));
	memcpy(originalFunction, &code, sizeof(code));
	VirtualProtect(originalFunction, sizeof(code), oldProtect, &oldProtect2);
}

void unhook(void* originalFunction, BYTE* oldCode)
{
	DWORD oldProtect, oldProtect2;
	VirtualProtect(originalFunction, sizeof(JmpCode), PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(originalFunction, oldCode, sizeof(JmpCode));
	VirtualProtect(originalFunction, sizeof(JmpCode), oldProtect, &oldProtect2);
}
