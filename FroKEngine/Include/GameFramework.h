#pragma once

// ������ �Ѱ� �� ����

// C/C++ runtime header
#include <Windows.h>
#include <list>
#include <vector>
#include <unordered_map>
#include <crtdbg.h>			// �޸� ���� üũ�� ���
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

// ���� ���꿡 �ʿ��� �����
#define PI		3.141592f
#define GRAVITY 9.8f

#include "Macro.h"
#include "Types.h"
#include "resource.h"