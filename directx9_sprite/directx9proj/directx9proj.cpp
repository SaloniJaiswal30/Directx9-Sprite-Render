// DXGIConsoleApplication.cpp : Defines the entry point for the console application.
//

//#include "DuplicationManager.h"
#include <time.h>
#include<windows.h>
#include <cstdio>
#include "DuplicationManager.h"

FILE* log_file;
char file_name[MAX_PATH];
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
MSG  msg;
HWND hwnd;
WNDCLASSW wc;
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)//main function(handle,null,parameter passed,display of window)
{

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;;				//whenever there is a movement or size adjustment of the height or width of the window, the entire window is redrawn
	wc.cbClsExtra = 0;									//no per-class extra data 
	wc.cbWndExtra = 0;									//no per-window extra data
	wc.lpszClassName = L"Window";						//L is for wide string used in unicode(class name)
	wc.hInstance = hInstance;							//handle to the application instance
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;							//class menu
	wc.lpfnWndProc = WndProc;							//pointer to application called window procedure
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			//loading curor and bydefault it is arroew
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);			//class icon and IDI_APPLICATION is a value for a default application icon
	RegisterClassW(&wc);

	//window creation
	RECT WindowRect = { 0, 0, 1024, 768 };
	AdjustWindowRect(&WindowRect, WS_OVERLAPPEDWINDOW, FALSE);
	hwnd = CreateWindowW(wc.lpszClassName, L"DXGI desktop duplication sample",
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW,
		10, 10,
		WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top,
		nullptr, nullptr, hInstance, nullptr);
	//hwnd = CreateWindowW(wc.lpszClassName, L" Window", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 10, 10, 500, 500, NULL, NULL, hInstance, NULL);

	//make window visible
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	//SetForegroundWindow(hwnd); //set our window on top
	//SetFocus(hwnd); //set the focus on our window
	fopen_s(&log_file, "logY.txt", "w");

	DUPLICATIONMANAGER DuplMgr;
	DUPL_RETURN Ret;
	UINT Output = 0;
	Ret = DuplMgr.InitDupl(log_file, Output, hwnd);
	if (Ret != DUPL_RETURN_SUCCESS)
	{
		fprintf_s(log_file, "Duplication Manager couldn't be initialized.");
		return 0;
	}
	if (FAILED(DuplMgr.GetFrame(hwnd))) {
		int c = 484 + 43;
	}
	//checks for the users input or msg
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (FAILED(DuplMgr.render())) {
			int c = 484 + 43;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		//Tells os to call window procedure
		
		


	}

	fclose(log_file);

	

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {

	case WM_DESTROY:
		PostQuitMessage(0);								//sends the WM_QUIT message to the message queue
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}
