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

	static HWND m_hWnd;		//�����ʿ��ϴ� ������ ���������� ���ϱ����� ��������� �������.
	static D3D_app* m_pInstance;			// �����ڿ��� �ν��Ͻ� �����͸� �����Ѵ�.

public:
	HACCEL m_hAccelTable;
	MSG m_msg;
	HINSTANCE m_hInstance;                                // ���� �ν��Ͻ��Դϴ�.
	WCHAR m_szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
	WCHAR m_szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.
	WNDCLASSEXW m_wcex;
	int  m_nCmdShow;
	
	UINT m_ClientWidth;
	UINT m_ClientHeight;

public:

	TimeSystem m_Timer;
	InputSystem m_Input;

public:
	// ������ ���� ���,����,���̱� �Ѵ�.
	virtual bool Initialize(UINT Width, UINT Height);
	virtual bool Run();
	void _update();
	virtual void Update(float deltaT) = 0; // ��� ���� Ŭ�������� ����
	virtual void Render() = 0; // ��� ���� Ŭ�������� ����

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

