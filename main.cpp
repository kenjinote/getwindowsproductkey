#define UNICODE
#include<windows.h>

TCHAR szClassName[]=TEXT("Window");

VOID GetProductKey(HWND hEdit)
{
	HKEY hKey;
	const TCHAR digits[]=TEXT("BCDFGHJKMPQRTVWXY2346789");
	TCHAR szProductKey[30];
	if(RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"),
		0,
		KEY_WOW64_64KEY|KEY_QUERY_VALUE,
		&hKey)==ERROR_SUCCESS)
	{
		DWORD dwBufSize=0;
		if(RegQueryValueEx(
			hKey,
			TEXT("DigitalProductId"),
			0,0,0,&dwBufSize)==ERROR_SUCCESS)
		{
			const LPBYTE lpBuf=(LPBYTE)GlobalAlloc(0,dwBufSize);
			if(RegQueryValueEx(
				hKey,
				TEXT("DigitalProductId"),
				0,0,(LPBYTE)lpBuf,&dwBufSize)==ERROR_SUCCESS)
			{
				szProductKey[29]=0;
				for(int i=28;i>=0;i--)
				{
					if((i+1)%6)
					{
						int x=0;
						for(int j=14;j>=0;j--)
						{
							x=(x<<8)+(lpBuf+0x34)[j];
							(lpBuf+0x34)[j]=x/24;
							x=x%24;
						}
						szProductKey[i]=digits[x];
					}
					else
					{
						szProductKey[i]=TEXT('-');
					}
				}
				SetWindowText(hEdit,szProductKey);
			}
			GlobalFree(lpBuf);
		}
		RegCloseKey(hKey);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	static HWND hEdit;
	switch(msg)
	{
	case WM_CREATE:
		hEdit = CreateWindow(
			TEXT("EDIT"),
			0,
			WS_VISIBLE | WS_CHILD | WS_HSCROLL | ES_READONLY,
			0,
			0,
			0,
			0,
			hWnd,
			0,
			((LPCREATESTRUCT)lParam)->hInstance,
			0
		);
		GetProductKey(hEdit);
		break;
	case WM_SETFOCUS:
		SendMessage(hEdit,EM_SETSEL,0,-1);
		SetFocus(hEdit);
		break;
	case WM_SIZE:
		MoveWindow(hEdit,0,0,LOWORD(lParam),HIWORD(lParam),TRUE);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd,msg,wParam,lParam);
	}
	return 0;
}

EXTERN_C void __cdecl WinMainCRTStartup()
{
	MSG msg;
	HINSTANCE hInstance=GetModuleHandle(0);
	WNDCLASS wndclass={
		CS_HREDRAW|CS_VREDRAW,
			WndProc,
			0,
			0,
			hInstance,
			0,
			LoadCursor(0,IDC_ARROW),
			(HBRUSH)(COLOR_WINDOW+1),
			0,
			szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd=CreateWindow(
		szClassName,
		TEXT("プロダクトキー取得"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
		);
	ShowWindow(hWnd,SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while(GetMessage(&msg,0,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	ExitProcess(msg.wParam);
}

#if _DEBUG
void main(){}
#endif
