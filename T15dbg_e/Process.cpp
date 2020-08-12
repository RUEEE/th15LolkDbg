#include "pch.h"
#include "Process.h"
#include "DrawShape.h"
#include "imguiClass.h"
#include "handle.h"

float windowWidth=0;
float windowHeight=0;
float scale=0;
IDirect3DDevice9* device=NULL;
ID3DXFont* g_font = NULL;
HWND windowHwnd=NULL;
bool is_in_game=false;

LRESULT WINAPI MyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))//ImGui处理
		return true;

	//原处理函数
	LRESULT (WINAPI *original)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)=nullptr;
	*(DWORD*)(&original)=0x471F10;
	return original(hWnd, msg, wParam, lParam);
}

void Init(HWND hwnd,IDirect3DDevice9* d)
{
	static bool is_init = false;
	if (!is_init)
	{
		is_init = true;
		//----------------------------------------hwnd,d3d----------------------------------------
		windowHwnd = hwnd;
		RECT windowRect = { 0 };
		//GetWindowRect(windowHwnd,&windowRect);
		GetClientRect(windowHwnd,&windowRect);
		windowWidth=windowRect.right-windowRect.left;
		windowHeight=windowRect.bottom-windowRect.top;
		//windowWidth = *(int*)0x004E78C4;
		scale = windowWidth / 640.0f;
		device = d;
		D3DXCreateFont(device, 20 * scale, 10 * scale, 0, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"微软雅黑", &g_font);
		//----------------------------------------othersetting----------------------------------------
		BulletHandle::is_HitBox_visible=false;
		BulletHandle::is_SelectBox_visible=false;



		//----------------------------------------imgui----------------------------------------
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(windowHwnd);
		ImGui_ImplDX9_Init(device);
		ImGui::StyleColorsDark();
		auto fonts = ImGui::GetIO().Fonts;
		fonts->AddFontFromFileTTF("c:/windows/fonts/simhei.ttf",13.0f,NULL,fonts->GetGlyphRangesChineseSimplifiedCommon());
	}
}

void DrawPF()
{
	GameGeneral::GetKeyPress();
	is_in_game = *(DWORD*)(0x004e9bb8) != 0;


	static RECT rect = { 0, 0, 0, 0 };
	DWORD player = *(DWORD*)0x004e9bb8;
	if (player != 0)//player
	{
		float playerX = (*(float*)((DWORD)player + 0x618));
		float playerY = (*(float*)((DWORD)player + 0x61C));
		float printX = TransFromStageToScreen(playerX, playerY).x;
		float printY = TransFromStageToScreen(playerX, playerY).y;
		rect.top = printY - 100;
		rect.left = printX - 100;
		rect.bottom = printY + 100;
		rect.right = printX + 100;
		ShapeRectangle r(playerX, playerY, 25, 50);
		r.Draw();
		//g_font->DrawTextW(NULL, _T("辣鸡"), -1, &rect, DT_CENTER, D3DCOLOR_XRGB(0,255,0));
	}
	if (*(int*)0x004E9A80 != 0)//enemy
	{
		int pt = 0x004E9A80;
		DWORD ptr;
		pt = *(int*)(*(int*)0x004E9A80 + 0x180);
		while (pt != 0 && *(int*)(pt + 4) != 0)
		{
			pt = *(int*)(pt + 4);
			ptr = *(int*)pt;
			DWORD flag = *(DWORD*)(ptr + 0x526c);
			float x, y;
			x = *(float*)(ptr + 0x1250);
			y = *(float*)(ptr + 0x1254);
			ShapeRectangle rect(x, y, 20, 20);
			rect.Draw();
		}
	}
	BulletHandle::bullet_num=0;
	if (*(DWORD*)0x004e9a6c != 0)//bullet
	{
		DWORD pd = *(DWORD*)0x004e9a6c + 0x6C;
		if (*(DWORD*)pd != 0)
		{
			pd = *(DWORD*)pd;
			while (pd != 0)
			{
				float x = *(float*)(*(DWORD*)pd + 0xC38);
				float y = *(float*)(*(DWORD*)pd + 0xC3C);
				float h = *(float*)(*(DWORD*)pd + 0xC58) * 1.0f;
				ShapeRectangle rect(x, y, h, h);
				//rect.cBorder=D3DCOLOR_RGBA(255,255,0,255);
				//rect.cContent=D3DCOLOR_RGBA(0,25,0,150);
				//rect.fillType|=rect.fillContent;
				//rect.Draw();
				pd = *(DWORD*)(pd + 4);
				BulletHandle::bullet_num++;
			}
		}
	}
	

	//-----------------------------------------------------------------------------------------
	DWORD ori;
	device->GetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,&ori);
	device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	//to avoid the shit-like texture
	DrawImGui();
	device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, ori);
	//reset
}

void __fastcall InGameFrame()
{
	//original code
	(*(int*)0x004E7400)++;
	(*(int*)0x004E73FC)++;
}
