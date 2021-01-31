#include "CCore.h"


INT APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	// 싱글톤 객체 초기화
	if (!CCore::GetInst()->Init(hInstance, true))
	{
		CCore::DestroyInst();
		return 0;
	}

	// 만약 이미 실행중인 같은 인스턴스가 있다면 종료시킨다.
	if (CCore::AnotherInstance())
	{
		CCore::DestroyInst();
		return 0;
	}

	// return value
	int nRev = CCore::GetInst()->Run();
	CCore::DestroyInst();

	return nRev;
}