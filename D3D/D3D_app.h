#pragma once
#include "framework.h"
#include "TimeSystem.h"
#include "InputSystem.h"

#define MAX_LOADSTRING 100

class D3D_app
{
public:
	D3D_app(HINSTANCE hInstance);
	virtual ~D3D_app();

	static HWND m_hWnd;		//자주필요하니 포인터 간접접근을 피하기위해 정적멤버로 만들었다.
	static D3D_app* m_pInstance;			// 생성자에서 인스턴스 포인터를 보관한다.

public:
	HACCEL m_hAccelTable;
	MSG m_msg;
	HINSTANCE m_hInstance;                                // 현재 인스턴스입니다.
	WCHAR m_szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
	WCHAR m_szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
	WNDCLASSEXW m_wcex;
	int  m_nCmdShow;
	
	UINT m_ClientWidth;
	UINT m_ClientHeight;

public:

	TimeSystem m_Timer;
	InputSystem m_Input;

public:
	// 윈도우 정보 등록,생성,보이기 한다.
	virtual bool Initialize(UINT Width, UINT Height);
	virtual bool Run();
	void _update();
	virtual void Update(float deltaT) = 0; // 상속 받은 클래스에서 구현
	virtual void Render() = 0; // 상속 받은 클래스에서 구현

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

