#pragma once
#include "pch.h"
#include <list>
#include <vector>
#include "DrawShape.h"

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