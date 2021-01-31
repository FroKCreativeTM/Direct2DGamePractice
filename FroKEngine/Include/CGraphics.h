#pragma once

#include "GameFramework.h"

class CGraphics
{
public : 
	HRESULT GetDeviceState()
	{
		m_hResult = E_FAIL;
		
		if (m_D3DDevice == nullptr)
		{
			return m_hResult;
		}
		m_hResult = m_D3DDevice->TestCooperativeLevel();

		return  m_hResult;
	}

public : 
	bool Init(HWND hWnd, int nWidth, int nHeight, bool isFullScreen);

public:
	void Input(float fDeltaTime);
	int Update(float fDeltaTime);
	int LateUpdate(float fDeltaTime);
	void Collision(float fDeltaTime);
	HRESULT BeginScene(float fDeltaTime);
	HRESULT EndScene(float fDeltaTime);
	HRESULT Render(float fDeltaTime);

	HRESULT Reset();

private : 
	void initD3DPP();
	bool isAdapterCompatible();

private : 
	LPDIRECT3D9				m_D3D;
	LPDIRECT3DDEVICE9		m_D3DDevice;
	D3DPRESENT_PARAMETERS	m_D3DPP;
	D3DDISPLAYMODE			m_pMode;

	HRESULT					m_hResult;
	HWND					m_hWnd;
	int						m_nWidth;
	int						m_nHeight;
	bool					m_bFullScreen;

	DECLARE_SINGLE(CGraphics)
};

