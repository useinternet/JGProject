#include "PCH/PCH.h"
#include "Platform.h"
#include "JWindow.h"
#include "Misc/Guid.h"
#include "Input/Key.h"

#ifdef _PLATFORM_WINDOWS
#include "Windows/WindowsJWindow.h"
#endif

HJInstance HPlatform::LoadDll(const PString& dllPath)
{
#ifdef _PLATFORM_WINDOWS
	return ::LoadLibraryA(dllPath.GetCStr());
#else
	return 0;
#endif
}


void HPlatform::UnLoadDll(HJInstance instanceID)
{
	if (instanceID == nullptr)
	{
#ifdef _PLATFORM_WINDOWS
		::FreeLibrary(instanceID);
#else
#endif
	}
}

PSharedPtr<PJWindow> HPlatform::CreateJWindow(const HJWindowArguments& args)
{
#ifdef _PLATFORM_WINDOWS
	PSharedPtr<PJWindow> window = ::Allocate<PWindowsJWindow>();
	HJWindowCreater windowCreater(window.GetRawPointer(), args);
	if (windowCreater.IsSuccess())
	{
		return window;
	}

	return nullptr;
#else 
	return nullptr;
#endif
}

HGuid HPlatform::NewGuid()
{
	HGuid Guid;
#ifdef _PLATFORM_WINDOWS
	UUID newId;
	UuidCreate(&newId);

	Guid._data1 = newId.Data1;
	Guid._data2 = newId.Data2;
	Guid._data3 = newId.Data3;

	for (int32 i = 0; i < 8; ++i)
	{
		Guid._data4[i] = newId.Data4[i];
	}

#endif

	return Guid;
}

uint64 HPlatform::GuidHash(const HGuid& inGuid)
{
	if (inGuid.IsValid())
	{
		return 0;
	}

	uint64 hash = 0;

#ifdef _PLATFORM_WINDOWS
	UUID uuid;

	uuid.Data1 = inGuid._data1;
	uuid.Data2 = inGuid._data2;
	uuid.Data3 = inGuid._data3;
	for (int32 i = 0; i < 8; ++i)
	{
		uuid.Data4[i] = inGuid._data4[i];
	}

	hash = ::UuidHash(&uuid, nullptr);

#endif
	return hash;
}

int32 HPlatform::GetSystemKeyCode(EKeyCode keyCode)
{
	switch (keyCode)
	{
		
	}

	return 0;
}

EKeyCode HPlatform::GetKeyCode(int32 inSystemKeyCode)
{
	return EKeyCode::Unknown;


	/*
	* /*
/*
	enum class EKeyCode
	{
		Unknown = 0,
		Num0 = 0x30, Num1 = 0x31, Num2 = 0x32, Num3 = 0x33, Num4 = 0x34,
		Num5 = 0x35, Num6 = 0x36, Num7 = 0x37, Num8 = 0x38, Num9 = 0x39,
		// 숫자 패드 키코드
		NumPad0 = 0x60, NumPad1 = 0x61, NumPad2 = 0x62, NumPad3  = 0x63, NumPad4 = 0x64,
		NumPad5 = 0x65, NumPad6 = 0x66, NumPad7 = 0x67, NumPad8 = 0x68, NumPad9 = 0x69,
		// F 숫자 키코드
		F1  = 0x70, F2  = 0x71, F3  = 0x72, F4  = 0x73, F5  = 0x74, F6  = 0x75,
		F7  = 0x76, F8  = 0x77, F9  = 0x78, F10 = 0x79, F11 = 0x7A, F12 = 0x7B,
		// 영어 키코드
		A = 0x41, B = 0x42, C = 0x43, D = 0x44, E = 0x45,
		F = 0x46, G = 0x47, H = 0x48, I = 0x49, J = 0x4A,
		K = 0x4B, L = 0x4C, N = 0x4E, M = 0x4D, O = 0x4F,
		P = 0x50, Q = 0x51, R = 0x52, S = 0x53, T = 0x54,
		U = 0x55, V = 0x56, W = 0x57, X = 0x58, Y = 0x59,
		Z = 0x5A,

		BackSpace		= VK_BACK,      Tab				= VK_TAB,
		Clear			= VK_CLEAR,     Enter			= VK_RETURN,
		Shift			= VK_SHIFT,		Ctrl			= VK_CONTROL,
		Alt				= VK_MENU,		Pause			= VK_PAUSE,
		CapsLock		= VK_CAPITAL,	Esc				= VK_ESCAPE,
		Space			= VK_SPACE,		PgUp			= VK_PRIOR,
		PgDown			= VK_NEXT,		End				= VK_END,
		Home			= VK_HOME,		Left			= VK_LEFT,
		Up				= VK_UP,		Right			= VK_RIGHT,
		Down            = VK_DOWN,		Select			= VK_SELECT,
		Print			= VK_PRINT,		Execute			= VK_EXECUTE,
		PrintScreen		= VK_SNAPSHOT,	Insert			= VK_INSERT,
		Help			= VK_HELP,		NumPadAdd		= VK_ADD,
		NumPadMultiply  = VK_MULTIPLY,	NumPadSubtract  = VK_SUBTRACT,
		NumPadDemical   = VK_DECIMAL,	NumPadDivide	= VK_DIVIDE,
		NumLock			= VK_NUMLOCK,	ScrollLock		= VK_SCROLL,
		LeftShift	    = VK_LSHIFT,	RightShift		= VK_RSHIFT,
		LeftCtrl		= VK_LCONTROL,	RightCtrl		= VK_RCONTROL,
		LeftAlt			= VK_LMENU,		RightAlt		= VK_RMENU,
		LeftBracket		= VK_OEM_4,		RightBracket	= VK_OEM_6,
		BackSlash		= VK_OEM_5,		QuotationMark	= VK_OEM_7,
		Semicolon		= VK_OEM_1,		Plus			= VK_OEM_PLUS,
		Comma			= VK_OEM_COMMA,	Minus			= VK_OEM_MINUS,
		Period			= VK_OEM_PERIOD,Slash			= VK_OEM_2,
		GraveAccent		= VK_OEM_3,
		Del = VK_DELETE,
		MouseLeftButton  = VK_LBUTTON,
		MouseRightButton = VK_RBUTTON,
		MouseMidButton	 = VK_MBUTTON,
		Mouse_X = 0x100,
		Mouse_Y = 0x101,
	};
*/

}
