#pragma once

#include "../GameFramework.h"

// for high-definition mouse
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC      ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE     ((USHORT) 0x02)
#endif
//--------------------------

typedef struct _tagKeyInfo
{
	string			strName;
	// unsigned long
	// 정수나 char 타입이면 push
	vector<DWORD>	vecKey;
	bool			bDown;
	bool			bPress;
	bool			bUp;

	_tagKeyInfo() :
		bDown(false),
		bPress(false),
		bUp(false)
	{}
}KEYINFO, * PKEYINFO;

typedef struct _tagControllerState
{
	XINPUT_STATE		state;
	XINPUT_VIBRATION	vibration;
	float				fVibrateTimeLeft;
	float				fVibrateTimeRight;
	bool				connected;
}CONTROLLER_STATE, *PCONTROLLER_STATE;

const short GAMEPAD_THUMBSTICK_DEADZONE = (short)(0.20f * 0X7FFF);    
const short GAMEPAD_TRIGGER_DEADZONE = 20;                     
const DWORD MAX_CONTROLLERS = 4;                               

// Bit corresponding to gamepad button in state.Gamepad.wButtons
const DWORD GAMEPAD_DPAD_UP = 0x0001;
const DWORD GAMEPAD_DPAD_DOWN = 0x0002;
const DWORD GAMEPAD_DPAD_LEFT = 0x0004;
const DWORD GAMEPAD_DPAD_RIGHT = 0x0008;
const DWORD GAMEPAD_START_BUTTON = 0x0010;
const DWORD GAMEPAD_BACK_BUTTON = 0x0020;
const DWORD GAMEPAD_LEFT_THUMB = 0x0040;
const DWORD GAMEPAD_RIGHT_THUMB = 0x0080;
const DWORD GAMEPAD_LEFT_SHOULDER = 0x0100;
const DWORD GAMEPAD_RIGHT_SHOULDER = 0x0200;
const DWORD GAMEPAD_A = 0x1000;
const DWORD GAMEPAD_B = 0x2000;
const DWORD GAMEPAD_X = 0x4000;
const DWORD GAMEPAD_Y = 0x8000;

class CInput
{
public:
	// 초기화와 키보드
	bool Init(HWND hWnd, bool isCaptured);
	void Update(float fDeltaTime);
	bool KeyDown(const string& strKey) const;
	bool KeyPress(const string& strKey) const;
	bool KeyUp(const string& strKey) const;

public : 
	// 마우스
	void MouseIn(LPARAM lParam);
	void MouseRawin(LPARAM lParam);

	void SetMouseLButton(bool b);
	void SetMouseMButton(bool b);
	void SetMouseRButton(bool b);
	void SetMouseXButton(WPARAM wParam);
	void mouseWheelIn(WPARAM wParam);

	POSITION GetMousePos()const;
	POSITION GetMouseRawPos() const;
	bool GetMouseLButton() const;
	bool GetMouseMButton() const;
	bool GetMouseRButton() const;
	bool GetMouseX1Button() const;
	bool GetMouseX2Button() const;

public : 
	void ReadControllers();
	void CheckController();
	const PCONTROLLER_STATE GetControllerState(UINT n);
	const WORD GetGamepadButton(UINT n);

	bool GetGamepadDPadUp(UINT n);
	bool GetGamepadDPadDown(UINT n);
	bool GetGamepadDPadLeft(UINT n);
	bool GetGamepadDPadRight(UINT n);
	bool GetGamepadStart(UINT n);
	bool GetGamepadBack(UINT n);
	bool GetGamepadLeftThumb(UINT n);
	bool GetGamepadRightThumb(UINT n);
	bool GetGamepadLeftShoulder(UINT n);
	bool GetGamepadRightShoulder(UINT n);
	bool GetGamepadA(UINT n);
	bool GetGamepadB(UINT n);
	bool GetGamepadX(UINT n);
	bool GetGamepadY(UINT n);

	BYTE GetGamepadLeftTrigger(UINT n);
	BYTE GetGamepadRightTrigger(UINT n);
	SHORT GetGamepadThumbLX(UINT n);
	SHORT GetGamepadThumbLY(UINT n);
	SHORT GetGamepadThumbRX(UINT n);
	SHORT GetGamepadThumbRY(UINT n);
	BYTE GetGamepadLeftTriggerUndead(UINT n);
	BYTE GetGamepadRightTriggerUndead(UINT n);
	SHORT GetGamepadThumbLXUndead(UINT n);
	SHORT GetGamepadThumbLYUndead(UINT n);
	SHORT GetGamepadThumbRXUndead(UINT n);
	SHORT GetGamepadThumbRYUndead(UINT n);

	void SetThumbstickDeadzone(short dz);
	void SetTriggerDeadzone(BYTE dz);
	short GetThumbstickDeadzone();
	BYTE GetTriggerDeadzone();

	void GamepadVibrateLeft(UINT n, WORD speed, float fSec);
	void GamepadVibrateRight(UINT n, WORD speed, float fSec);
	void VibrateControllers(float fDeltaTime);


public:
	template <typename T>
	bool AddKey(const T& data)
	{
		// 처음에 NULL로 되어있다.
		if (!m_pCreateKey)
		{
			m_pCreateKey = new KEYINFO;
		}

		// 템플릿 식별 번호를 만들어서 저장한다.
		const char* pTType = typeid(T).name();

		// 정수나 char인 경우
		if (strcmp(pTType, "char") == 0 ||
			strcmp(pTType, "int") == 0)
		{
			m_pCreateKey->vecKey.push_back((DWORD)data);
		}
		// map의 key값이다.
		else
		{
			m_pCreateKey->strName = data;
			m_mapKey.insert(make_pair(m_pCreateKey->strName, m_pCreateKey));
		}

		return true;
	}

	// template ... Types
	// 가변 템플릿
	// 무조건 재귀함수를 써야...
	template <typename T, typename ... Types>
	bool AddKey(const T& data, const Types& ...arg)
	{
		// 처음에 NULL로 되어있다.
		if (!m_pCreateKey)
		{
			m_pCreateKey = new KEYINFO;
		}

		// 템플릿 식별 번호를 만들어서 저장한다.
		const char* pTType = typeid(T).name();

		// 정수나 char인 경우
		if (strcmp(pTType, "char") == 0 ||
			strcmp(pTType, "int") == 0)
		{
			m_pCreateKey->vecKey.push_back((DWORD)data);
		}
		// map의 key값이다.
		else
		{
			m_pCreateKey->strName = data;
			m_mapKey.insert(make_pair(m_pCreateKey->strName, m_pCreateKey));
		}

		// 재귀로 돌린다.
		AddKey(arg...);

		// 재귀용 변수를 다시 초기화
		if (m_pCreateKey)
		{
			m_pCreateKey = nullptr;
		}

		return true;
	}

	POSITION  GetMouseClientPos() const;
	POSITION  GetMouseWorldPos() const;
	POSITION  GetMouseMove() const;	// 얼마나 마우스가 움직였는가.

private:
	PKEYINFO FindKey(const string& strKey) const;

private:
	HWND		m_hWnd;
	// 이 키를 누르면 호출되는 함수 목록을 매칭할 것이다.
	unordered_map<string, PKEYINFO>	m_mapKey;
	// 재귀 함수 돌리기용
	PKEYINFO	m_pCreateKey;

private:
	// 마우스에 대한 정보
	RAWINPUTDEVICE		m_Rid[1];			// 고정밀 마우스용
	POSITION			m_tMousePos;		// 화면상 마우스 위치
	POSITION			m_tMouseRawPos;		// 고정밀 마우스 데이터
	int					m_nMouseWheel;		// mouse wheel movement
	bool				m_bMouseCaptured;	// 마우스가 캡쳐된 경우
	bool				m_bMouseLButton;
	bool				m_bMouseMButton;
	bool				m_bMouseRButton;
	bool				m_bMouseX1Button;
	bool				m_bMouseX2Button;

private : 
	// xinput을 이용한 게임 패드
	CONTROLLER_STATE	m_tControllers[MAX_CONTROLLERS];
	short				m_fThumbstickDeadzone;
	short				m_fTriggerDeadzone;


	DECLARE_SINGLE(CInput)
};

