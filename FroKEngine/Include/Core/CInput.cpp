#include "CInput.h"
#include "CGameError.h"

DEFINITION_SINGLE(CInput);

CInput::CInput() :
	m_pCreateKey(nullptr)
{
	m_tMousePos.x = 0.f;
	m_tMousePos.y = 0.f;
	m_tMouseRawPos.x = 0.f;
	m_tMouseRawPos.y = 0.f;

	m_bMouseLButton = false;
	m_bMouseMButton = false;
	m_bMouseRButton = false;
	m_bMouseX1Button = false;
	m_bMouseX2Button = false;
}

CInput::~CInput()
{
}

bool CInput::Init(HWND hWnd, bool isCaptured)
{
	try
	{
		m_hWnd = hWnd;
		m_bMouseCaptured = isCaptured;

		m_Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		m_Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
		m_Rid[0].dwFlags = RIDEV_INPUTSINK;
		m_Rid[0].hwndTarget = hWnd;
		RegisterRawInputDevices(m_Rid, 1, sizeof(m_Rid[0]));

		if (m_bMouseCaptured)
		{
			SetCapture(hWnd);
		}

		ZeroMemory(m_tControllers, sizeof(CONTROLLER_STATE) * MAX_CONTROLLERS);
		CheckController();

		AddKey('W', "MoveFront");
		AddKey('S', "MoveBack");
		AddKey('D', "MoveRight");
		AddKey('A', "MoveLeft");
		AddKey(VK_CONTROL, "Fire");
		AddKey(VK_SPACE, "Jump");
		AddKey(VK_LBUTTON, "MouseLButton");
		AddKey(VK_RBUTTON, "MouseRButton");
		AddKey(VK_MENU, "AltKey");
		AddKey(VK_RETURN, "EnterKey");
		AddKey(VK_ESCAPE, "Esc");
		AddKey(VK_OEM_3, "~"); // ~key
 
	}
	catch (...)
	{
		throw(CGameError(GAME_ERROR_TYPE::GET_FATAL,
			"Error initializing input system"));
	}
	return true;
}

void CInput::Update(float fDeltaTime)
{
	unordered_map<string, PKEYINFO>::iterator iter;
	unordered_map<string, PKEYINFO>::iterator iterEnd = m_mapKey.end();

	for (iter = m_mapKey.begin(); iter != iterEnd; ++iter)
	{
		int nPushCount = 0;
		for (size_t i = 0; i < iter->second->vecKey.size(); ++i)
		{
			// 체크하려는 키가 눌렸다면
			if (GetAsyncKeyState(iter->second->vecKey[i]) & 0x8000)
			{
				nPushCount++;
			}
		}

		// 누른 경우
		if (nPushCount == iter->second->vecKey.size())
		{
			// 처음 누른 경우
			if (!iter->second->bDown && !iter->second->bPress)
			{
				iter->second->bDown = true;
				iter->second->bPress = true;
			}
			// 계속 누른 경우
			else if (iter->second->bDown)
			{
				iter->second->bDown = false;
			}
		}
		// 안 누름
		else
		{
			// 누르다가 뗀 경우
			if (iter->second->bDown || iter->second->bPress)
			{
				iter->second->bUp = true;
				iter->second->bDown = false;
				iter->second->bPress = false;
			}
			else if (iter->second->bUp)
			{
				iter->second->bUp = false;
			}
		}
	}
}

bool CInput::KeyDown(const string& strKey) const
{
	PKEYINFO pInfo = FindKey(strKey);
	if (!pInfo)
		return false;
	return pInfo->bDown;
}

bool CInput::KeyPress(const string& strKey) const
{
	PKEYINFO pInfo = FindKey(strKey);
	if (!pInfo)
		return false;
	return pInfo->bPress;
}

bool CInput::KeyUp(const string& strKey) const
{
	PKEYINFO pInfo = FindKey(strKey);
	if (!pInfo)
		return false;
	return pInfo->bUp;
}

void CInput::MouseIn(LPARAM lParam)
{
	m_tMousePos.x = (float)GET_X_LPARAM(lParam);
	m_tMousePos.y = (float)GET_Y_LPARAM(lParam);
}

// 처리되지 않은 마우스 데이터를 읽어 mouseRaw 구조체들에 저장한다.
// 이런 륀은 고정밀 마우스(DPI 사용하는)와 연동된다.
void CInput::MouseRawin(LPARAM lParam)
{
	UINT dwSize = 40;
	static BYTE lpb[40];
	GetRawInputData((HRAWINPUT)lParam, RID_INPUT,
		lpb, &dwSize, sizeof(RAWINPUTHEADER));

	RAWINPUT* raw = (RAWINPUT*)lpb;

	if (raw->header.dwSize == RIM_TYPEMOUSE)
	{
		m_tMouseRawPos.x = (float)raw->data.mouse.lLastX;
		m_tMouseRawPos.y = (float)raw->data.mouse.lLastY;
	}
}

void CInput::SetMouseLButton(bool b)
{
	m_bMouseLButton = b;
}

void CInput::SetMouseMButton(bool b)
{
	m_bMouseMButton = b;
}

void CInput::SetMouseRButton(bool b)
{
	m_bMouseRButton = b;
}

void CInput::SetMouseXButton(WPARAM wParam)
{
	m_bMouseX1Button = (wParam & MK_XBUTTON1) ? true : false;
	m_bMouseX2Button = (wParam & MK_XBUTTON2) ? true : false;
}

void CInput::mouseWheelIn(WPARAM wParam)
{
	m_nMouseWheel = GET_WHEEL_DELTA_WPARAM(wParam);
}

POSITION CInput::GetMousePos() const
{
	return m_tMousePos;
}

POSITION CInput::GetMouseRawPos() const
{
	return m_tMouseRawPos;
}

bool CInput::GetMouseLButton() const
{
	return m_bMouseLButton;
}

bool CInput::GetMouseMButton() const
{
	return m_bMouseMButton;
}

bool CInput::GetMouseRButton() const
{
	return m_bMouseRButton;
}

bool CInput::GetMouseX1Button() const
{
	return m_bMouseX1Button;
}

bool CInput::GetMouseX2Button() const
{
	return m_bMouseX2Button;
}

void CInput::ReadControllers()
{
	DWORD result;
	for (DWORD i = 0; i < MAX_CONTROLLERS; i++)
	{
		if (m_tControllers[i].connected)
		{
			result = XInputGetState(i, &m_tControllers[i].state);
			if (result == ERROR_DEVICE_NOT_CONNECTED)
			{
				m_tControllers[i].connected = false;
			}
		}
	}
}

void CInput::CheckController()
{
	DWORD result;
	for (DWORD i = 0; i < MAX_CONTROLLERS; i++)
	{
		result = XInputGetState(i, &m_tControllers[i].state);
		if (result == ERROR_SUCCESS) 
		{
			m_tControllers[i].connected = true;
		}
		else 
		{
			m_tControllers[i].connected = false;
		}
	}
}

// 지정된 컨트롤러의 상태를 반환한다.
const PCONTROLLER_STATE CInput::GetControllerState(UINT n)
{
	if (n > MAX_CONTROLLERS - 1)
	{
		n = MAX_CONTROLLERS - 1;
	}
	return &m_tControllers[n];
}

const WORD CInput::GetGamepadButton(UINT n)
{
	if (n > MAX_CONTROLLERS)
	{
		n = MAX_CONTROLLERS;
	}
	return m_tControllers[n].state.Gamepad.wButtons;
}

bool CInput::GetGamepadDPadUp(UINT n)
{
	return (GetGamepadButton(n) & GAMEPAD_DPAD_UP);
}

bool CInput::GetGamepadDPadDown(UINT n)
{
	return (GetGamepadButton(n) & GAMEPAD_DPAD_DOWN);
}

bool CInput::GetGamepadDPadLeft(UINT n)
{
	return (GetGamepadButton(n) & GAMEPAD_DPAD_LEFT);
}

bool CInput::GetGamepadDPadRight(UINT n)
{
	return (GetGamepadButton(n) & GAMEPAD_DPAD_RIGHT);
}

bool CInput::GetGamepadStart(UINT n)
{
	return (GetGamepadButton(n) & GAMEPAD_START_BUTTON);
}

bool CInput::GetGamepadBack(UINT n)
{
	return (GetGamepadButton(n) & GAMEPAD_BACK_BUTTON);
}

bool CInput::GetGamepadLeftThumb(UINT n)
{
	return (GetGamepadButton(n) & GAMEPAD_LEFT_THUMB);
}

bool CInput::GetGamepadRightThumb(UINT n)
{
	return (GetGamepadButton(n) & GAMEPAD_RIGHT_THUMB);
}

bool CInput::GetGamepadLeftShoulder(UINT n)
{
	return (GetGamepadButton(n) & GAMEPAD_LEFT_SHOULDER);
}

bool CInput::GetGamepadRightShoulder(UINT n)
{
	return (GetGamepadButton(n) & GAMEPAD_RIGHT_SHOULDER);
}

bool CInput::GetGamepadA(UINT n)
{
	return (GetGamepadButton(n) & GAMEPAD_A);
}

bool CInput::GetGamepadB(UINT n)
{
	return (GetGamepadButton(n) & GAMEPAD_B);
}

bool CInput::GetGamepadX(UINT n)
{
	return (GetGamepadButton(n) & GAMEPAD_X);
}

bool CInput::GetGamepadY(UINT n)
{
	return (GetGamepadButton(n) & GAMEPAD_Y);
}

BYTE CInput::GetGamepadLeftTrigger(UINT n)
{
	BYTE value = GetGamepadLeftTriggerUndead(n);
	
	// if > dead zone
	if (value > m_fTriggerDeadzone) 
	{    
		//adjust magnitude relative to the end of the dead zone
		value = (value - m_fTriggerDeadzone) * 255 / (255 - m_fTriggerDeadzone);
	}
	// else, < dead zone
	else 
	{
		value = 0;
	}

	return value;
}

BYTE CInput::GetGamepadRightTrigger(UINT n)
{
	BYTE value = GetGamepadRightTriggerUndead(n);
	
	// 데드존 범위 밖이라면 
	if (value > m_fTriggerDeadzone)
	{
		// 비율을 데드존에 맞춰서 적용한다.
		value = (value - m_fTriggerDeadzone) * 255 / (255 - m_fTriggerDeadzone);
	}
	else 
	{
		value = 0;
	}

	return value;
}

SHORT CInput::GetGamepadThumbLX(UINT n)
{
	int x = GetGamepadThumbLXUndead(n);

	// x 가 데드존 범위 밖이라면
	if (x > m_fThumbstickDeadzone) 
	{
		// 데드존 범위(0 ~ 32767) 안으로 normalize 시킨다.
		x = (x - m_fThumbstickDeadzone) * 32767 / (32767 - m_fThumbstickDeadzone);
	}
	// -x 가 데드존 범위 밖이라면
	else if (x < -m_fThumbstickDeadzone) 
	{
		// 데드존 범위(-32767 ~ 0) 안으로 normalize 시킨다.
		x = (x + m_fThumbstickDeadzone) * 32767 / (32767 - m_fThumbstickDeadzone);
	}
	else
	{        
		x = 0;  // return 0
	}
	return static_cast<SHORT>(x);
}

SHORT CInput::GetGamepadThumbLY(UINT n)
{
	int y = GetGamepadThumbLYUndead(n);

	if (y > m_fThumbstickDeadzone)
	{
		// 데드존 범위(0 ~ 32767) 안으로 normalize 시킨다.
		y = (y - m_fThumbstickDeadzone) * 32767 / (32767 - m_fThumbstickDeadzone);
	}
	else if (y < -m_fThumbstickDeadzone) 
	{
		// 데드존 범위(-32767 ~ 0) 안으로 normalize 시킨다.
		y = (y + m_fThumbstickDeadzone) * 32767 / (32767 - m_fThumbstickDeadzone);
	}
	else 
	{
		y = 0;  // return 0
	}

	return static_cast<SHORT>(y);
}

SHORT CInput::GetGamepadThumbRX(UINT n)
{
	int x = GetGamepadThumbRXUndead(n);

	// +x가 데드존 바깥에 있다면
	if (x > m_fThumbstickDeadzone) 
	{
		// 데드존 범위(0 ~ 32767) 안으로 normalize 시킨다.
		x = (x - m_fThumbstickDeadzone) * 32767 / (32767 - m_fThumbstickDeadzone);
	}
	// -x가 데드존 바깥에 있다면
	else if (x < -m_fThumbstickDeadzone) 
	{
		// 데드존 범위(-32767 ~ 0) 안으로 normalize 시킨다.
		x = (x + m_fThumbstickDeadzone) * 32767 / (32767 - m_fThumbstickDeadzone);
	}
	// 데드 존 범위 안이라면
	else
	{
		x = 0;  // return 0
	}

	return static_cast<SHORT>(x);
}

SHORT CInput::GetGamepadThumbRY(UINT n)
{
	int y = GetGamepadThumbRYUndead(n);

	if (y > m_fThumbstickDeadzone) 
	{
		// 데드존 범위(0 ~ 32767) 안으로 normalize 시킨다.
		y = (y - m_fThumbstickDeadzone) * 32767 / (32767 - m_fThumbstickDeadzone);
	}
	else if (y < -m_fThumbstickDeadzone) 
	{
		// 데드존 범위(-32767 ~ 0) 안으로 normalize 시킨다.
		y = (y + m_fThumbstickDeadzone) * 32767 / (32767 - m_fThumbstickDeadzone);
	}
	else 
	{// else, y inside dead zone
		y = 0;  // return 0
	}

	return static_cast<SHORT>(y);
}

void CInput::SetThumbstickDeadzone(short dz)
{
	m_fThumbstickDeadzone = abs(dz);
}

void CInput::SetTriggerDeadzone(BYTE dz)
{
	m_fTriggerDeadzone = dz;
}

short CInput::GetThumbstickDeadzone()
{
	return m_fThumbstickDeadzone;
}

BYTE CInput::GetTriggerDeadzone()
{
	return static_cast<BYTE>(m_fTriggerDeadzone);
}

void CInput::GamepadVibrateLeft(UINT n, WORD speed, float fSec)
{
	if (n > MAX_CONTROLLERS - 1)
		n = MAX_CONTROLLERS - 1;
	m_tControllers[n].vibration.wLeftMotorSpeed = speed;
	m_tControllers[n].fVibrateTimeLeft = fSec;
}

void CInput::GamepadVibrateRight(UINT n, WORD speed, float fSec)
{
	if (n > MAX_CONTROLLERS - 1)
		n = MAX_CONTROLLERS - 1;
	m_tControllers[n].vibration.wRightMotorSpeed = speed;
	m_tControllers[n].fVibrateTimeRight = fSec;
}

void CInput::VibrateControllers(float fDeltaTime)
{
	for (int i = 0; i < MAX_CONTROLLERS; i++)
	{
		if (m_tControllers[i].connected)
		{
			m_tControllers[i].fVibrateTimeLeft -= fDeltaTime;
			if (m_tControllers[i].fVibrateTimeLeft < 0)
			{
				m_tControllers[i].fVibrateTimeLeft = 0;
				m_tControllers[i].vibration.wLeftMotorSpeed = 0;
			}
			m_tControllers[i].fVibrateTimeRight -= fDeltaTime;
			if (m_tControllers[i].fVibrateTimeRight < 0)
			{
				m_tControllers[i].fVibrateTimeRight = 0;
				m_tControllers[i].vibration.wRightMotorSpeed = 0;
			}
			XInputSetState(i, &m_tControllers[i].vibration);
		}
	}
}

BYTE CInput::GetGamepadLeftTriggerUndead(UINT n)
{
	if (n > MAX_CONTROLLERS - 1)
		n = MAX_CONTROLLERS - 1;
	return m_tControllers[n].state.Gamepad.bLeftTrigger;
}

BYTE CInput::GetGamepadRightTriggerUndead(UINT n)
{
	if (n > MAX_CONTROLLERS - 1)
		n = MAX_CONTROLLERS - 1;
	return m_tControllers[n].state.Gamepad.bRightTrigger;
}

SHORT CInput::GetGamepadThumbLXUndead(UINT n)
{
	if (n > MAX_CONTROLLERS - 1)   // if invalid controller number
		n = MAX_CONTROLLERS - 1;    // force valid
	return m_tControllers[n].state.Gamepad.sThumbLX;
}

SHORT CInput::GetGamepadThumbLYUndead(UINT n)
{
	if (n > MAX_CONTROLLERS - 1)   // if invalid controller number
		n = MAX_CONTROLLERS - 1;    // force valid
	return m_tControllers[n].state.Gamepad.sThumbLY;
}

SHORT CInput::GetGamepadThumbRXUndead(UINT n)
{
	if (n > MAX_CONTROLLERS - 1)   // if invalid controller number
		n = MAX_CONTROLLERS - 1;    // force valid
	return m_tControllers[n].state.Gamepad.sThumbRX;
}

SHORT CInput::GetGamepadThumbRYUndead(UINT n)
{
	if (n > MAX_CONTROLLERS - 1)   // if invalid controller number
		n = MAX_CONTROLLERS - 1;    // force valid
	return m_tControllers[n].state.Gamepad.sThumbRY;
}

POSITION CInput::GetMouseClientPos() const
{
	return POSITION(0.f, 0.f);
}

POSITION CInput::GetMouseWorldPos() const
{
	return POSITION(0.f, 0.f);
}

POSITION CInput::GetMouseMove() const
{
	return POSITION(0.f, 0.f);
}

PKEYINFO CInput::FindKey(const string& strKey) const
{
	unordered_map<string, PKEYINFO>::const_iterator iter
		= m_mapKey.find(strKey);

	if (iter == m_mapKey.end())
	{
		return nullptr;
	}
	return iter->second;
}
