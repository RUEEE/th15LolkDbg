#include "pch.h"
#include "hook.h"
#include "DrawShape.h"
#include "Process.h"
void* endSceneAddr = NULL;
void* createDeviceAddr = NULL;
BYTE endSceneOldCode[sizeof(JmpCode)];
BYTE createDeviceOldCode[sizeof(JmpCode)];

extern HANDLE current_process;
extern HWND windowHwnd;
extern ID3DXFont* g_font;
extern float windowWidth;
extern float scale;
extern IDirect3DDevice9* device;
// dllmain.cpp : 定义 DLL 应用程序的入口点。

HRESULT STDMETHODCALLTYPE MyEndScene(IDirect3DDevice9* thiz)
{
	device=thiz;
	unhook(endSceneAddr, endSceneOldCode);//unhook

	DrawPF();
	
	//rehook
	HRESULT hr = thiz->EndScene();
	hook(endSceneAddr, MyEndScene, endSceneOldCode);
	
	return hr;
}


HRESULT STDMETHODCALLTYPE MyCreateDevice
(IDirect3D9* thiz, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, 
DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, 
IDirect3DDevice9** ppReturnedDeviceInterface)
{
	//SetWindowText(hFocusWindow,L"shit");
	char title[MAX_PATH];
	GetWindowTextA(hFocusWindow,title, MAX_PATH);
	if (strcmp(title, (char*)0x004CE644) == 0)//确实是该窗口
	{
		SetWindowText(hFocusWindow,L"草泥马");
		unhook(createDeviceAddr, createDeviceOldCode);
		HRESULT hr = thiz->CreateDevice
		(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

		

		Init(hFocusWindow,*ppReturnedDeviceInterface);
		return hr;
		//关闭hook
	}else{
		unhook(createDeviceAddr, createDeviceOldCode);
		HRESULT hr = thiz->CreateDevice
		(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
		hook(createDeviceAddr, MyCreateDevice, createDeviceOldCode);
		return hr;
	}
}




DWORD WINAPI initHookThread(LPVOID dllMainThread)
{
	// 等待DllMain（LoadLibrary线程）结束
	//WaitForSingleObject(dllMainThread, INFINITE);
	//CloseHandle(dllMainThread);
	// 创建一个窗口用于初始化D3D
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = DefWindowProc;
	wc.lpszClassName = _T("DummyWindow");
	if (RegisterClassEx(&wc) == 0)
	{
		MessageBox(NULL, _T("注册窗口类失败"), _T(""), MB_OK);
		return 0;
	}

	HWND hwnd = CreateWindowEx(0, wc.lpszClassName, _T(""), WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, NULL, NULL, wc.hInstance, NULL);
	if (hwnd == NULL)
	{
		MessageBox(NULL, _T("创建窗口失败"), _T(""), MB_OK);
		return 0;
	}

	// 初始化D3D

	IDirect3D9* d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d9 == NULL)
	{
		MessageBox(NULL, _T("创建D3D失败"), _T(""), MB_OK);
		DestroyWindow(hwnd);
		return 0;
	}

	D3DPRESENT_PARAMETERS pp = {};
	pp.Windowed = TRUE;
	pp.SwapEffect = D3DSWAPEFFECT_COPY;

	IDirect3DDevice9* device;
	if (FAILED(d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &device)))
	{
		MessageBox(NULL, _T("创建设备失败"), _T(""), MB_OK);
		d3d9->Release();
		DestroyWindow(hwnd);
		return 0;
	}

	// hook EndScene
	endSceneAddr = (*(void***)device)[42]; //EndScene是IDirect3DDevice9第43个函数
	hook(endSceneAddr, MyEndScene, endSceneOldCode);
	//>>>>>>>>>>>>>>
	createDeviceAddr=(*(void***)d3d9)[16]; //CreateDevice是IDirect3D9第17个函数
	hook(createDeviceAddr, MyCreateDevice, createDeviceOldCode);
	//>>>>>>>>>>>>>>
	*(DWORD*)(0x00472F94)=(DWORD)MyWndProc;//hook Message
	//>>>>>>>>>>>>>>
	DWORD oldProtect,oldProtect2;
	VirtualProtect((LPVOID)0x00430120,0x100,PAGE_EXECUTE_READWRITE,&oldProtect);
	*(BYTE*)(0x0043D120)=0xB8;
	*(DWORD*)(0x0043D121)=(DWORD)InGameFrame;
	*(DWORD*)(0x0043D125)=0x9090D0FF;
	*(DWORD*)(0x0043D128)=0x90909090;//hook frame increase
	VirtualProtect((LPVOID)0x00430120, 0x100, oldProtect, &oldProtect2);

	// 释放
	d3d9->Release();
	device->Release();
	DestroyWindow(hwnd);
	return 0;
}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		// 取当前线程句柄
		HANDLE curThread;
		if (!DuplicateHandle(current_process=GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &curThread, SYNCHRONIZE, FALSE, 0))
			return FALSE;
		// DllMain中不能使用COM组件，所以要在另一个线程初始化
		CloseHandle(CreateThread(NULL, 0, initHookThread, curThread, 0, NULL));
		break;

	case DLL_PROCESS_DETACH:
		if (endSceneAddr != NULL)
			unhook(endSceneAddr, endSceneOldCode);
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}