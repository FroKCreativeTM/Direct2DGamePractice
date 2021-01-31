#pragma once

#include "GameFramework.h"

class CCore
{
public:
	static CCore* GetInst();
	static void DestroyInst();
	static bool AnotherInstance();

public:
	bool Init(HINSTANCE hInstance, bool isFullScreen);
	int Run();
	void HandleLostGraphicsDevice();

public:
	// 해상도 반환
	RESOLUTION GetResolution() const
	{
		return m_tRS;
	}

	// 윈도우 핸들 반환
	HWND GetWindowHandle() const
	{
		return m_hWnd;
	}

	// 윈도우 종료 함수
	void DestroyGame()
	{
		DestroyWindow(m_hWnd);
	}

	// 윈도우 인스턴스를 반환한다.
	HINSTANCE GetWindowInstance() const
	{
		return m_hInst;
	}

public:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	// 싱글톤! 
	// static이라 결국은 전역변수화되니 이 점 주의!(멀티스레드 상에선 특히나)
	// 메모리 생성 시점! 프로그램 시작할 때
private:
	static CCore*	m_pInstance;
	static bool		m_bLoop;

	// API 관련 멤버변수
private:
	HINSTANCE		m_hInst;
	HWND			m_hWnd;
	RESOLUTION		m_tRS;
	HDC				m_hDC;
	HRESULT			m_hResult;

	bool			m_bFullScreen;

	// 외부 공개할 필요 없는 메소드들
private:
	ATOM MyRegisterClass();
	BOOL Create();

private:
	void Logic();
	void Input(float fDeltaTime);
	int Update(float fDeltaTime);
	int LateUpdate(float fDeltaTime);
	void Collision(float fDeltaTime);
	void Render(float fDeltaTime);

	void ReleaseAll();
	void ResetAll();

private:
	CCore();
	~CCore();
};

