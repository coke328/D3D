#include "render_square.h"



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	render_square App(hInstance);  // �����ڿ��� ������,������ �̸��� �ٲ۴�
	if (!App.Initialize(1600, 900))
		return -1;

	return App.Run();
}