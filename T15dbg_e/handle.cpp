#include "pch.h"
#include "handle.h"

//general
unsigned int* GameGeneral::score									= (unsigned int*)0x004E740C;
unsigned int* GameGeneral::hiScore									= (unsigned int*)0x004E75BC;
unsigned int* GameGeneral::life										= (unsigned int*)0x004E7450;
unsigned int* GameGeneral::bomb										= (unsigned int*)0x004E745C;
unsigned int* GameGeneral::power									= (unsigned int*)0x004E7440;
unsigned int* GameGeneral::mvalue									= (unsigned int*)0x004E7434;
unsigned int* GameGeneral::graze									= (unsigned int*)0x004E741C;
unsigned int* GameGeneral::graze_present							= (unsigned int*)0x004E7420;
unsigned int* GameGeneral::retry_count_total						= (unsigned int*)0x004E7594;
unsigned int* GameGeneral::retry_count_now							= (unsigned int*)0x004E75B8;
unsigned int* GameGeneral::death_count								= (unsigned int*)0x004E742C;
unsigned int* GameGeneral::credit_used							    = (unsigned int*)0x004E7414;
unsigned int* GameGeneral::difficulty								= (unsigned int*)0x004E7410;
unsigned int* GameGeneral::playerType								= (unsigned int*)0x004E7404;
unsigned int* GameGeneral::frame_passed								= (unsigned int*)0x004E73FC;
float* GameGeneral::gameSpeed										= (float*)0x004E73E8;
DWORD* GameGeneral::keyPressed										= (DWORD*)(0x004E6F28);
DWORD* GameGeneral::keyPressedG										= (DWORD*)(0x004E6D10);

bool GameGeneral::is_pressed_Z										= false;
bool GameGeneral::is_pressed_X										= false;
bool GameGeneral::is_pressed_C										= false;
bool GameGeneral::is_pressed_Shift									= false;
bool GameGeneral::is_pressed_Up										= false;
bool GameGeneral::is_pressed_Down									= false;
bool GameGeneral::is_pressed_Left									= false;
bool GameGeneral::is_pressed_Right									= false;


bool GameGeneral::is_pressed_GZ										= false;
bool GameGeneral::is_pressed_GX										= false;
bool GameGeneral::is_pressed_GC										= false;
bool GameGeneral::is_pressed_GShift									= false;
bool GameGeneral::is_pressed_GUp									= false;
bool GameGeneral::is_pressed_GDown									= false;
bool GameGeneral::is_pressed_GLeft									= false;
bool GameGeneral::is_pressed_GRight									= false;
//bullet
bool BulletHandle::is_HitBox_visible								= false;
bool BulletHandle::is_SelectBox_visible								= true;
int BulletHandle::bullet_num										= 0;

//player
bool PlayerHandle::is_player_accessible					        	= false;
float PlayerHandle::player_x										= 0;
float PlayerHandle::player_y										= 0;
float PlayerHandle::hitbox											= 0;
int PlayerHandle::shoot_rank										= 0;


void GameGeneral::GetKeyPress()
{
GameGeneral::is_pressed_Z		= *GameGeneral::keyPressed & 0x1;
GameGeneral::is_pressed_X		= *GameGeneral::keyPressed & 0x2;
GameGeneral::is_pressed_C		= *GameGeneral::keyPressed & 0x800;
GameGeneral::is_pressed_Shift	= *GameGeneral::keyPressed & 0x8;
GameGeneral::is_pressed_Up		= *GameGeneral::keyPressed & 0x10;
GameGeneral::is_pressed_Down	= *GameGeneral::keyPressed & 0x20;
GameGeneral::is_pressed_Left	= *GameGeneral::keyPressed & 0x40;
GameGeneral::is_pressed_Right	= *GameGeneral::keyPressed & 0x80;

GameGeneral::is_pressed_GZ		= *GameGeneral::keyPressedG & 0x1;
GameGeneral::is_pressed_GX		= *GameGeneral::keyPressedG & 0x2;
GameGeneral::is_pressed_GC		= *GameGeneral::keyPressedG & 0x800;
GameGeneral::is_pressed_GShift	= *GameGeneral::keyPressedG & 0x8;
GameGeneral::is_pressed_GUp		= *GameGeneral::keyPressedG & 0x10;
GameGeneral::is_pressed_GDown	= *GameGeneral::keyPressedG & 0x20;
GameGeneral::is_pressed_GLeft	= *GameGeneral::keyPressedG & 0x40;
GameGeneral::is_pressed_GRight	= *GameGeneral::keyPressedG & 0x80;
}

void GameGeneral::PowerChanged()
{
	__asm
	{
		mov ecx, DWORD_PTR[0x004E9BB8]
		mov eax,0x004567B0
		call eax
	}
}

void GameGeneral::LifeChanged()
{
	__asm
	{
		push DWORD_PTR[0x004E7454]
		push DWORD_PTR[0x004E7450]
		mov ecx, DWORD_PTR[0x004E9A8C]
		mov eax, 0x0043A850
		call eax
	}
}

void GameGeneral::BombChanged()
{
	__asm
	{
		push DWORD_PTR[0x004E7460]
		push DWORD_PTR[0x004E745C]
		mov ecx, DWORD_PTR[0x004E9A8C]
		mov eax, 0x0043A960
		call eax
	}
}