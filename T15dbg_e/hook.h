#pragma once
#include "pch.h"

#pragma pack(push)
#pragma pack(1)
struct JmpCode
{
private:
	const BYTE jmp;
	DWORD address;

public:
	JmpCode(DWORD srcAddr, DWORD dstAddr)
		: jmp(0xE9)
	{
		setAddress(srcAddr, dstAddr);
	}

	void setAddress(DWORD srcAddr, DWORD dstAddr)
	{
		address = dstAddr - srcAddr - sizeof(JmpCode);
	}
};
void unhook(void* originalFunction, BYTE* oldCode);
void hook(void* originalFunction, void* hookFunction, BYTE* oldCode);