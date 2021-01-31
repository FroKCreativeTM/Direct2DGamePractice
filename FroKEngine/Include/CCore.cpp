#include "CCore.h"
#include "CGraphics.h"
#include "Core/CTimer.h"
#include "Core/CGameError.h"
#include "Core/CInput.h"

CCore* CCore::m_pInstance = nullptr;
bool   CCore::m_bLoop = true;

CCore* CCore::GetInst()
{
	if (!m_pInstance)
	{
		m_pInstance = new CCore;
	}
	return m_pInstance;
}

void CCore::DestroyInst()
{
	SAFE_DELETE(m_pInstance);
}

bool CCore::AnotherInstance()
{
	// ���ؽ��� �̿��ؼ� �ߺ� �ν��Ͻ� ������ ���´�.
	HANDLE hOurMutex;

	hOurMutex = CreateMutex(nullptr, true, L"Already Created Program");

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return true;
	}
	return false;
}

bool CCore::Init(HINSTANCE hInstance, bool isFullScreen)
{
	m_hInst = hInstance;
	m_bFullScreen = isFullScreen;

	// ������ Ŭ���� ���
	MyRegisterClass();

	// �ػ� ����
	m_tRS.nWidth = 1280;
	m_tRS.nHeight = 720;

	// ������â ����
	Create();

	// ���� Ÿ���� ȭ�� DC�� ������ش�.
	m_hDC = GetDC(m_hWnd);

	// Ÿ�̸�(FPS, ��ŸŸ��) �ʱ�ȭ
	if (!GET_SINGLE(CTimer)->Init(m_hWnd))
	{
		return false;
	}

	// �׷��� ����̽� �ʱ�ȭ
	if (!GET_SINGLE(CGraphics)->Init(m_hWnd, m_tRS.nWidth, m_tRS.nHeight, m_bFullScreen))
	{
		return false;
	}

	// ��ǲ �Ŵ���(FPS, ��ŸŸ��) �ʱ�ȭ
	if (!GET_SINGLE(CInput)->Init(m_hWnd, false))
	{
		return false;
	}

	return true;
}

int CCore::Run()
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	while (m_bLoop)
	{
		// ������ �޽��� ť�� ó���ϴ� �Լ��Դϴ�.
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// ������ ����Ÿ��
		else
		{
			// ������ ������ ����ȴ�.
			Logic();
		}
	}

	return (int)msg.wParam;
}

void CCore::HandleLostGraphicsDevice()
{
	m_hResult = GET_SINGLE(CGraphics)->GetDeviceState();

	if (FAILED(m_hResult))
	{
		if (m_hResult == D3DERR_DEVICELOST)
		{
			Sleep(100);
			return;
		}
		else if (m_hResult == D3DERR_DEVICENOTRESET)
		{
			ReleaseAll();
			m_hResult = GET_SINGLE(CGraphics)->Reset();
			if (FAILED(m_hResult))
			{
				return;
			}
			ResetAll();
		}
		else
		{
			return;
		}
	}
}

LRESULT CCore::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		m_bLoop = false;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

ATOM CCore::MyRegisterClass()
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = CCore::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hInst;
	wcex.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"FroK's Engine";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

	return RegisterClassExW(&wcex);
}

BOOL CCore::Create()
{
	DWORD style;
	if (m_bFullScreen)
	{
		style = WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP;
	}
	else
	{
		style = WS_OVERLAPPEDWINDOW;
	}

	m_hWnd = CreateWindowW(
		L"FroK's Engine",
		L"FroK's Engine",
		style,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
		nullptr, nullptr, m_hInst, nullptr);

	if (!m_hWnd)
	{
		return FALSE;
	}

	if (!m_bFullScreen)
	{
		 // ������ ũ�⸦ ������ Ŭ���̾�Ʈ ������ width * height �� �ǰ� �Ѵ�.
		RECT rc;
		GetClientRect(m_hWnd, &rc);

		MoveWindow(m_hWnd,
			0, 0,
			m_tRS.nWidth + (m_tRS.nWidth - rc.right),
			m_tRS.nHeight + (m_tRS.nHeight - rc.bottom)
		,TRUE);
	}

	/*
	// ���� Ÿ��Ʋ�ٳ� �����츦 ������ ũ�⸦ �����ش�.
	RECT rc = { 0,0,(LONG)m_tRS.nWidth, (LONG)m_tRS.nHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	// ������ ������ ũ��� ������ Ŭ���̾�Ʈ ũ�⸦ ���ϴ� ũ��� ������� �Ѵ�.
	SetWindowPos(m_hWnd, HWND_TOPMOST, 100, 100, rc.right - rc.left,
		rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);
	*/

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	return TRUE;
}

void CCore::Logic()
{ 
	// Ÿ�̸� ����
	GET_SINGLE(CTimer)->Update();

	// �츮�� �Լ��� ����� �� ��ŸŸ�ӿ� �̰��� �����ϸ� �ȴ�.
	float fDeltaTime = GET_SINGLE(CTimer)->GetDeltaTime();

	// ���� ���� ����
	Input(fDeltaTime);
	Update(fDeltaTime);
	LateUpdate(fDeltaTime);
	Collision(fDeltaTime);
	Render(fDeltaTime);
}

void CCore::Input(float fDeltaTime)
{
	GET_SINGLE(CInput)->Update(fDeltaTime);
}

int CCore::Update(float fDeltaTime)
{
	return 0;
}

int CCore::LateUpdate(float fDeltaTime)
{
	return 0;
}

void CCore::Collision(float fDeltaTime)
{
}

void CCore::Render(float fDeltaTime)
{
	// �������� ���������� �� �ڿ��� �������Ѵ�.
	if (SUCCEEDED(GET_SINGLE(CGraphics)->BeginScene(fDeltaTime)))
	{
		// �׸� �� �������� ���⿡ ���´�
		

		// ���� �����Ѵ�.
		GET_SINGLE(CGraphics)->EndScene(fDeltaTime);
	}

	HandleLostGraphicsDevice();
	if (GET_SINGLE(CGraphics)->Render(fDeltaTime) == E_FAIL)
	{
		DestroyGame();
	}	
}

void CCore::ReleaseAll()
{
}

void CCore::ResetAll()
{
	GET_SINGLE(CGraphics)->Reset();
}

CCore::CCore()
{
	// �޸� ������ üũ�ϱ� ���� ����
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// �޸� ���� ���� ���� ����.
	// �Ű������� �� �޸��� ��� ��ȣ�� �־��ָ�,
	// �޸� �� �κ��� �ٷ� �̵��Ѵ�. (ȣ�� ������ �� �� �� �־��..)
	// _CrtSetBreakAlloc(313);

	// ������ �ð��� üũ�ؼ� �̰� ���۽�ų�� ���� �����Ѵ�.
#ifdef _DEBUG   
	// �ܼ�â�� ���������ִ� �Լ�
	AllocConsole();
#endif
}

CCore::~CCore()
{
	// ���� ���� Ŭ�������� ���� �����Ѵ�.
	DESTROY_SINGLE(CInput);
	DESTROY_SINGLE(CGraphics);
	DESTROY_SINGLE(CTimer);
}
