#pragma once

#include "GameFramework.h"

class CCore
{
public:
	static CCore* GetInst();
	static void DestroyInst();

public:
	bool Init(HINSTANCE hInstance);
	int Run();

public:
	// �ػ� ��ȯ
	RESOLUTION GetResolution() const
	{
		return m_tRS;
	}

	// ������ �ڵ� ��ȯ
	HWND GetWindowHandle() const
	{
		return m_hWnd;
	}

	// ������ ���� �Լ�
	void DestroyGame()
	{
		DestroyWindow(m_hWnd);
	}

	// ������ �ν��Ͻ��� ��ȯ�Ѵ�.
	HINSTANCE GetWindowInstance() const
	{
		return m_hInst;
	}

public:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	// �̱���! 
	// static�̶� �ᱹ�� ��������ȭ�Ǵ� �� �� ����!(��Ƽ������ �󿡼� Ư����)
	// �޸� ���� ����! ���α׷� ������ ��
private:
	static CCore* m_pInstance;
	static bool		m_bLoop;

	// API ���� �������
private:
	HINSTANCE		m_hInst;
	HWND			m_hWnd;
	RESOLUTION		m_tRS;
	HDC				m_hDC;

	// �ܺ� ������ �ʿ� ���� �޼ҵ��
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

private:
	CCore();
	~CCore();
};
