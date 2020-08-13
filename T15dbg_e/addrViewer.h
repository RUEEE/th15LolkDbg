#pragma once
#include "pch.h"
#include "imguiClass.h"
#include <vector>
#include <string>
void AddrViewer(bool* p_open);


class Address
{
private:
	std::string descriptionTotal;
	std::string valueStr;
public:
	static const char* typeStr[];
	int count;
	DWORD base;
	bool is_bad_addr;
	int bad_count;
	std::vector<DWORD> offs;
	std::vector<DWORD> offVal;
	enum Addr_Type{AD_BYTE=0,AD_WORD,AD_DWORD,AD_QWORD,AD_FLOAT,AD_DOUBLE,AD_STRING
	,AD_VECTOR2,AD_VECTOR3,AD_VECTOR4};
	std::string description;
	Addr_Type type;
	int typeAdd;//string's length and is_hex


	void GetPointer();
	void GetValueUnSafe(void* val);
	void GetValueStr(char* str, int bufsz);
	const char* GetValueStrUnsafe(){return valueStr.c_str();}
	Address(int c);
	void SetBuffer(int i);
	const char* GetDescription();
	const char* GetDescriptionUnsafe(){return descriptionTotal.c_str(); }
};
