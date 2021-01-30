#pragma once

// 게임을 총괄 및 관리

// C/C++ runtime header
#include <Windows.h>
#include <list>
#include <vector>
#include <unordered_map>
#include <crtdbg.h>			// 메모리 누수 체크용 헤더
#include <typeinfo>
#include <string>
#include <functional>
#include <conio.h>
#include <exception>

using namespace std;

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "winmm.lib")

// 수학 연산에 필요한 상수들
#define PI		3.141592f
#define GRAVITY 9.8f

#include "Macro.h"
#include "Types.h"
#include "resource.h"