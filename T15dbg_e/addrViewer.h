#pragma once
#include "pch.h"
#include "imguiClass.h"
#include <vector>
#include <string>
void AddrViewer(bool* p_open);


class Address
{
private:
	std::string valueStr;
	unsigned int ID;
	static unsigned int cnt;//warning copy/assign can't work correctly
public:
	static const char* typeStr[];
	int count;
	DWORD base;
	bool is_bad_addr;
	int bad_count;
	std::vector<DWORD> offs;
	std::vector<DWORD> offVal;
	enum Addr_Type{AD_BYTE=0,AD_WORD=1,AD_DWORD=2,AD_QWORD=3,AD_FLOAT=4,AD_DOUBLE=5,AD_STRING=6
	,AD_VECTOR2=7,AD_VECTOR3=8,AD_VECTOR4=9};
	std::string description;
	Addr_Type type;
	int typeAdd;//string's length and is_hex

	const char* GetDescriptionWider();
	void GetPointer();
	void GetValueUnSafe(void* val);
	void GetValueStr(char* str, int bufsz);
	const char* GetValueStrUnsafe(){return valueStr.c_str();}
	Address(int c);
	void SetBuffer(int i);
	void GetID(){cnt++; ID=cnt;}//to copy/assign
};
