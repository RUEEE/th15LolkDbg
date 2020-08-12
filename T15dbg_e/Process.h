#pragma once
#include "pch.h"
void Init(HWND hwnd, IDirect3DDevice9* d);
void DrawPF();
void DrawImGui();
LRESULT WINAPI MyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void __fastcall InGameFrame();