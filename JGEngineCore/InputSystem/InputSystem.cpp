#include"InputSystem.h"
#include"JGPressManager.h"
#include"../EngineStatics/JGLog.h"
using namespace std;
InputSystem::InputSystem()
{
	m_PressManager = make_unique<JGPressManager>();
}
InputSystem::~InputSystem()
{
	if (m_Mouse)
	{
		m_Mouse->Unacquire();
		m_Mouse->Release();
		m_Mouse = nullptr;
	}
	if (m_KeyBoard)
	{
		m_KeyBoard->Unacquire();
		m_KeyBoard->Release();
		m_Mouse = nullptr;
	}
	if (m_DirectInput)
	{
		m_DirectInput->Release();
		m_DirectInput = nullptr;
	}
}
bool InputSystem::CreateInputDevice(HINSTANCE hinst, HWND hWnd, int width, int height)
{
	m_hWnd = hWnd;
	m_ScreenWidth = (float)width;
	m_ScreenHeight = (float)height;

	HRESULT result = S_OK;

	/** 다이렉트 장치 */
	// 다이렉트 인풋의 인터페이스를 초기화
	result = DirectInput8Create(hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_DirectInput, nullptr);
	if (FAILED(result))
	{
		JGLog::Write(ELogLevel::Error, TT("Create DirectInput Failed."));
		return false;
	}
	/* 키보드 장치 */
	result = m_DirectInput->CreateDevice(GUID_SysKeyboard, &m_KeyBoard, nullptr);
	if (FAILED(result))
	{
		JGLog::Write(ELogLevel::Error, TT("Create Keyboard Device Failed."));
		return false;
	}
	result = m_KeyBoard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		JGLog::Write(ELogLevel::Error, TT("Create Keyboard SetDataFormat Failed."));
		return false;
	}
	// 다른 프로그램들과의 협력레벨을 설정한다. ( 배제 상태로 설정)
	result = m_KeyBoard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
	{
		JGLog::Write(ELogLevel::Error, TT("Create Keyboard SetCooperativeLevel Failed."));
		return false;
	}
	// acquire 함수를 이용하여 이 포인터 변수로 키보드에 대한 접근을 취득한다.
	result = m_KeyBoard->Acquire();
	if (FAILED(result))
	{
		JGLog::Write(ELogLevel::Error, TT("Create Keyboard Acquire Failed."));
		return false;
	}


	/**  마우스 장치 */
	result = m_DirectInput->CreateDevice(GUID_SysMouse, &m_Mouse, nullptr);
	if (FAILED(result))
	{
		JGLog::Write(ELogLevel::Error, TT("Create Mouse Device Failed."));
		return false;
	}
	result = m_Mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		JGLog::Write(ELogLevel::Error, TT("Create Mouse SetDataFormat Failed."));
		return false;
	}
	// 마우스 협력상태 설정 (배제 상태로) <- 마우스 포커스가 잃었는지 찾았는지 알기위해서..
	result = m_Mouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		JGLog::Write(ELogLevel::Error, TT("Create Mouse SetCooperativeLevel Failed."));
		return false;
	}
	// acquire 함수를 이용하여 이포인터 변수로 마우스에 대한 접근을 취득한다.
	result = m_Mouse->Acquire();
	if (FAILED(result))
	{
		JGLog::Write(ELogLevel::Error, TT("Create Mouse Acquire Failed."));
		return false;
	}

	// JGPressManager에 키보드 상태를 넘겨준다.
	m_PressManager->LinkInputSystemKeyBoardState(m_KeyBoardState,&m_MouseState);
	return true;
}
void InputSystem::Tick(const float DeltaTime)
{
	ReadKeyboard();
	ReadMouse();
}

bool InputSystem::ReadKeyboard()
{
	HRESULT result = S_OK;

	result = m_KeyBoard->GetDeviceState(sizeof(m_KeyBoardState), (LPVOID)&m_KeyBoardState);
	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_KeyBoard->Acquire();
		}
		else
		{
			JGLog::Write(ELogLevel::Warning, TT("Update KeyboardState Failed."));
			return false;
		}
	}
	return true;
}
bool InputSystem::ReadMouse()
{
	HRESULT result = S_OK;

	result = m_Mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_MouseState);
	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_Mouse->Acquire();
		}
		else
		{
			JGLog::Write(ELogLevel::Warning, TT("Update MouseState Failed."));
			return false;
		}
	}
	return true;
}