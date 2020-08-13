#pragma once
#include "pch.h"
#include <list>
#include <vector>
#include "DrawShape.h"


#define GET_VAL_FINAL_SAFE(T,...) (GetPtFinalSafe(__VA_ARGS__)!=nullptr?(*(T*)GetPtFinalSafe(__VA_ARGS__)) : (T)0)
#define GET_VAL_FINAL_UNSAFE(T,...) (GetPtFinalUnSafe(__VA_ARGS__)!=nullptr?(*(T*)GetPtFinalUnSafe(__VA_ARGS__)) : (T)0)
//only to get something type such as int,float,double,...
extern HANDLE current_process;

template<typename Off,typename... Offs>
void* GetPtFinalSafe(DWORD base,Off offs1, Offs... offs)
{
	void* ptr;
	SIZE_T g;
	if (!(ReadProcessMemory(current_process,(LPCVOID)base,&ptr,0x4,&g)))
	{
		//IsBadReadPtr((void*)base, 0x4) || IsBadReadPtr(ptr = (void*)(*(DWORD*)base + (DWORD)offs1), 0x4)
		return nullptr;
	}
	ptr=(void*)((DWORD)ptr + offs1);

	if constexpr (sizeof...(offs) > 0) {
		ptr = GetPtFinalSafe((DWORD)ptr,offs...);
	}
	return ptr;
}

template<typename Off, typename... Offs>
void* GetPtFinalUnSafe(DWORD base, Off offs1, Offs... offs)
{
	void* ptr;
	ptr = (void*)(*(DWORD*)base + (DWORD)offs1);
	if constexpr (sizeof...(offs) > 0)
		ptr = GetPtFinalUnSafe((DWORD)ptr, offs...);
	return ptr;
}

bool GetPtFinalSafe(DWORD base,const std::vector<DWORD>& offs,std::vector<DWORD>& ptrs);



class BulletHandle
{
public:
	static bool is_HitBox_visible;
	static bool is_SelectBox_visible;
	static int bullet_num;

	ShapeRectangle rectHitBox;
	ShapeRectangle rectSelectBox;
	bool is_Selected;
};

class PlayerHandle
{
public:
	static bool is_player_accessible;
	static float player_x;
	static float player_y;
	static float hitbox;
	static int shoot_rank;
	

};

class GameGeneral
{
public:
	static DWORD playerBase;

	static DWORD bulletBase;
	static DWORD bulletOffset1;

	static DWORD enemyBase;
	static DWORD enemyOffset1;
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	static unsigned int* score;
	static unsigned int* hiScore;
	static unsigned int* life;
	static unsigned int* bomb;
	static unsigned int* power;
	static unsigned int* mvalue;
	static unsigned int* graze;
	static unsigned int* graze_present;
	static unsigned int* retry_count_total;
	static unsigned int* retry_count_now;
	static unsigned int* death_count;
	static unsigned int* credit_used;
	static unsigned int* difficulty;
	static unsigned int* playerType;
	static unsigned int* frame_passed;
	static float* gameSpeed;
	static DWORD* keyPressed;
	static DWORD* keyPressedG;
	//>>>>>>>>>>>>>
	static bool is_pressed_Z;
	static bool is_pressed_X;
	static bool is_pressed_C;
	static bool is_pressed_Shift;
	static bool is_pressed_Up;
	static bool is_pressed_Down;
	static bool is_pressed_Left;
	static bool is_pressed_Right;

	static bool is_pressed_GZ;
	static bool is_pressed_GX;
	static bool is_pressed_GC;
	static bool is_pressed_GShift;
	static bool is_pressed_GUp;
	static bool is_pressed_GDown;
	static bool is_pressed_GLeft;
	static bool is_pressed_GRight;

	static void GetKeyPress();
	static void PowerChanged();
	static void LifeChanged();
	static void BombChanged();
};