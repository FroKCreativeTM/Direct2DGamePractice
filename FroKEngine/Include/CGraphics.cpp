#include "CGraphics.h"
#include "Core/CGameError.h"

DEFINITION_SINGLE(CGraphics)

CGraphics::CGraphics() :
	m_hWnd(nullptr),
	m_nWidth(0),
	m_nHeight(0),
	m_bFullScreen(false)
{

}

CGraphics::~CGraphics()
{
	SAFE_RELEASE(m_D3D);
	SAFE_RELEASE(m_D3DDevice);
}

bool CGraphics::Init(HWND hWnd, int nWidth, int nHeight, bool isFullScreen)
{
	m_hWnd = hWnd;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_bFullScreen = isFullScreen;

	m_D3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_D3D == nullptr)
	{
		throw(CGameError(GAME_ERROR_TYPE::GET_FATAL, "Error initializing Direct3D"));
	}

	initD3DPP();

	if (m_bFullScreen)
	{
		// 만약 풀스크린이고, 어댑터가 호환된다면, 리프레시 속도를 
		// 호환되는 것으로 설정한다.
		if (isAdapterCompatible())
		{
			m_D3DPP.FullScreen_RefreshRateInHz = m_pMode.RefreshRate;
		}
		else
		{
			throw(CGameError(GAME_ERROR_TYPE::GET_FATAL,
				"The graphic device does not support the"\
				"specified resolution and/or format"));
		}

		// 그래픽 카드가 텍스처, 라이팅, 정점 셰이더를 지원하는지 확인한다.
		D3DCAPS9 caps;
		DWORD behavior;
		m_hResult = m_D3D->GetDeviceCaps(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL, &caps);

		// 디바이스가 텍스처, 라이팅을 지원하지 않거나 정점 셰이더 1.1을
		// 지원하지 않는다면, 소프트웨어 정점 처리로 전환한다.
		if ((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
			caps.VertexShaderVersion < D3DVS_VERSION(1, 1))
		{
			behavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		}
		else
		{
			behavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		}
	}

	m_hResult = m_D3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&m_D3DPP,
		&m_D3DDevice);

	return true;
}

void CGraphics::initD3DPP()
{
	try
	{
		// 구조체를 0으로 체운다.
		ZeroMemory(&m_D3DPP, sizeof(m_D3DPP));

		// 필요한 매개변수를 체운다.
		m_D3DPP.BackBufferWidth = m_nWidth;
		m_D3DPP.BackBufferHeight = m_nHeight;

		if (m_bFullScreen)
		{
			m_D3DPP.BackBufferFormat = D3DFMT_X8R8G8B8;	// 24비트 색상
		}
		else
		{
			m_D3DPP.BackBufferFormat = D3DFMT_UNKNOWN;	// 컴퓨터의 색상
		}

		m_D3DPP.BackBufferCount = 1;		// 백버퍼 카운터를 설정한다.
		m_D3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
		m_D3DPP.hDeviceWindow = m_hWnd;
		m_D3DPP.Windowed = (!m_bFullScreen);
		m_D3DPP.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	catch (...)
	{
		throw(CGameError(GAME_ERROR_TYPE::GET_FATAL, "Error initiating D3D presentation Parameter"));
	}
}

/// <summary>
/// 어댑터가 d3dpp에 지정된 백 버퍼의 높이와 폭, 리프레시 속도와 호환되는지 확인한다.
/// </summary>
/// <returns>
/// pmode 구조체가 체워져 있다면 true를 리턴한다.
/// </returns>
bool CGraphics::isAdapterCompatible()
{
	UINT modes = m_D3D->GetAdapterModeCount(D3DADAPTER_DEFAULT,
		m_D3DPP.BackBufferFormat);

	for (UINT i = 0; i < modes; ++i)
	{
		m_hResult = m_D3D->EnumAdapterModes(D3DADAPTER_DEFAULT,
			m_D3DPP.BackBufferFormat,
			i, &m_pMode);

		if (m_pMode.Height == m_D3DPP.BackBufferHeight &&
			m_pMode.Width == m_D3DPP.BackBufferWidth &&
			m_pMode.RefreshRate >= m_D3DPP.FullScreen_RefreshRateInHz)
		{
			return true;
		}
	}
	return false;
}

void CGraphics::Input(float fDeltaTime)
{
}

int CGraphics::Update(float fDeltaTime)
{
	return 0;
}

int CGraphics::LateUpdate(float fDeltaTime)
{
	return 0;
}

void CGraphics::Collision(float fDeltaTime)
{
}

HRESULT CGraphics::BeginScene(float fDeltaTime)
{
	m_hResult = E_FAIL;

	// 디바이스가 없다면 실패이다.
	if (!m_D3DDevice)
	{
		return m_hResult;
	}

	m_D3DDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 255, 0), 0.f, 0);
	m_hResult = m_D3DDevice->BeginScene();

	return m_hResult;
}

HRESULT CGraphics::EndScene(float fDeltaTime)
{
	m_hResult = E_FAIL;
	if (m_D3DDevice)
	{
		m_hResult = m_D3DDevice->EndScene();
	}
	return m_hResult;
}

HRESULT CGraphics::Render(float fDeltaTime)
{
	m_hResult = E_FAIL;

	m_hResult = m_D3DDevice->Present(nullptr, nullptr, nullptr, nullptr);

	return m_hResult;
}

HRESULT CGraphics::Reset()
{
	m_hResult = E_FAIL;
	initD3DPP();

	m_hResult = m_D3DDevice->Reset(&m_D3DPP);

	return m_hResult;
}
