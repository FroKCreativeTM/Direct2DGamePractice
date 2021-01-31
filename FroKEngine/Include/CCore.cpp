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
	// 뮤텍스를 이용해서 중복 인스턴스 실행을 막는다.
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

	// 윈도우 클래스 등록
	MyRegisterClass();

	// 해상도 설정
	m_tRS.nWidth = 1280;
	m_tRS.nHeight = 720;

	// 윈도우창 생성
	Create();

	// 렌더 타깃인 화면 DC를 만들어준다.
	m_hDC = GetDC(m_hWnd);

	// 타이머(FPS, 델타타임) 초기화
	if (!GET_SINGLE(CTimer)->Init(m_hWnd))
	{
		return false;
	}

	// 그래픽 디바이스 초기화
	if (!GET_SINGLE(CGraphics)->Init(m_hWnd, m_tRS.nWidth, m_tRS.nHeight, m_bFullScreen))
	{
		return false;
	}

	// 인풋 매니저(FPS, 델타타임) 초기화
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
		// 윈도우 메시지 큐를 처리하는 함수입니다.
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 윈도우 데드타임
		else
		{
			// 게임의 로직이 실행된다.
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
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
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
		 // 윈도우 크기를 조절해 클라이언트 영역이 width * height 가 되게 한다.
		RECT rc;
		GetClientRect(m_hWnd, &rc);

		MoveWindow(m_hWnd,
			0, 0,
			m_tRS.nWidth + (m_tRS.nWidth - rc.right),
			m_tRS.nHeight + (m_tRS.nHeight - rc.bottom)
		,TRUE);
	}

	/*
	// 실제 타이틀바나 윈도우를 포함한 크기를 구해준다.
	RECT rc = { 0,0,(LONG)m_tRS.nWidth, (LONG)m_tRS.nHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	// 위에서 구해준 크기로 윈도우 클라이언트 크기를 원하는 크기로 맞춰줘야 한다.
	SetWindowPos(m_hWnd, HWND_TOPMOST, 100, 100, rc.right - rc.left,
		rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);
	*/

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	return TRUE;
}

void CCore::Logic()
{ 
	// 타이머 갱신
	GET_SINGLE(CTimer)->Update();

	// 우리가 함수를 만들고 그 델타타임에 이것을 전달하면 된다.
	float fDeltaTime = GET_SINGLE(CTimer)->GetDeltaTime();

	// 게임 로직 실행
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
	// 렌더링이 성공적으로 된 뒤에만 렌더링한다.
	if (SUCCEEDED(GET_SINGLE(CGraphics)->BeginScene(fDeltaTime)))
	{
		// 그릴 씬 정보들을 여기에 적는다
		

		// 씬을 종료한다.
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
	// 메모리 누수를 체크하기 위한 설정
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// 메모리 릭이 있을 때만 쓴다.
	// 매개변수로 그 메모리의 블록 번호를 넣어주면,
	// 메모리 릭 부분을 바로 이동한다. (호출 스택을 잘 볼 수 있어야..)
	// _CrtSetBreakAlloc(313);

	// 컴파일 시간에 체크해서 이걸 동작시킬지 말지 결정한다.
#ifdef _DEBUG   
	// 콘솔창을 생성시켜주는 함수
	AllocConsole();
#endif
}

CCore::~CCore()
{
	// 서브 관리 클래스들을 전부 해제한다.
	DESTROY_SINGLE(CInput);
	DESTROY_SINGLE(CGraphics);
	DESTROY_SINGLE(CTimer);
}
