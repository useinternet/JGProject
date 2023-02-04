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
	case EKeyCode::A:	return 0x41;
	case EKeyCode::B:	return 0x42;
	case EKeyCode::C:	return 0x43;
	case EKeyCode::D:	return 0x44;
	case EKeyCode::E:	return 0x45;
	case EKeyCode::F:	return 0x46;
	case EKeyCode::G:	return 0x47;
	case EKeyCode::H:	return 0x48;
	case EKeyCode::I:	return 0x49;
	case EKeyCode::J:   return 0x4A;
	case EKeyCode::K:	return 0x4B;
	case EKeyCode::L:	return 0x4C;
	case EKeyCode::M:	return 0x4D;
	case EKeyCode::N:	return 0x4E;
	case EKeyCode::O:	return 0x4F;
	case EKeyCode::P:	return 0x50;
	case EKeyCode::Q:	return 0x51;
	case EKeyCode::R:	return 0x52;
	case EKeyCode::S:	return 0x53;
	case EKeyCode::T:	return 0x54;
	case EKeyCode::U:	return 0x55;
	case EKeyCode::V:	return 0x56;
	case EKeyCode::W:	return 0x57;
	case EKeyCode::X:	return 0x58;
	case EKeyCode::Y:	return 0x59;
	case EKeyCode::Z:	return 0x5A;
	case EKeyCode::Num0: return 0x30;
	case EKeyCode::Num1: return 0x31;
	case EKeyCode::Num2: return 0x32;
	case EKeyCode::Num3: return 0x33;
	case EKeyCode::Num4: return 0x34;
	case EKeyCode::Num5: return 0x35;
	case EKeyCode::Num6: return 0x36;
	case EKeyCode::Num7: return 0x37;
	case EKeyCode::Num8: return 0x38;
	case EKeyCode::Num9: return 0x39;
	case EKeyCode::NumPad0: return 0x60;
	case EKeyCode::NumPad1: return 0x61;
	case EKeyCode::NumPad2:	return 0x62;
	case EKeyCode::NumPad3:	return 0x63;
	case EKeyCode::NumPad4:	return 0x64;
	case EKeyCode::NumPad5:	return 0x65;
	case EKeyCode::NumPad6:	return 0x66;
	case EKeyCode::NumPad7:	return 0x67;
	case EKeyCode::NumPad8:	return 0x68;
	case EKeyCode::NumPad9:	return 0x69;

	case EKeyCode::NumPadAdd:		return VK_ADD;
	case EKeyCode::NumPadMultiply:	return VK_MULTIPLY;
	case EKeyCode::NumPadSubtract:	return VK_SUBTRACT;
	case EKeyCode::NumPadDemical:	return VK_DECIMAL;
	case EKeyCode::NumPadDivide:	return VK_DIVIDE;
	case EKeyCode::NumLock:			return VK_NUMLOCK;

	case EKeyCode::F1:  return 0x70;
	case EKeyCode::F2:  return 0x71;
	case EKeyCode::F3:  return 0x72;
	case EKeyCode::F4:  return 0x73;
	case EKeyCode::F5:  return 0x74;
	case EKeyCode::F6:  return 0x75;
	case EKeyCode::F7:  return 0x76;
	case EKeyCode::F8:  return 0x77;
	case EKeyCode::F9:  return 0x78;
	case EKeyCode::F10: return 0x79;
	case EKeyCode::F11: return 0x7A;
	case EKeyCode::F12: return 0x7B;

	case EKeyCode::Left:  return VK_LEFT;
	case EKeyCode::Right: return VK_RIGHT;
	case EKeyCode::Up:	  return VK_UP;
	case EKeyCode::Down:  return VK_DOWN;

	case EKeyCode::LeftBracket:		return VK_OEM_4;
	case EKeyCode::RightBracket:	return VK_OEM_6;
	case EKeyCode::Semicolon:		return VK_OEM_1;
	case EKeyCode::QuotationMark:	return VK_OEM_7;
	case EKeyCode::Comma:			return VK_OEM_COMMA;
	case EKeyCode::Period:			return VK_OEM_PERIOD;
	case EKeyCode::Slash:			return VK_OEM_2;

	case EKeyCode::Del:				return VK_DELETE;
	case EKeyCode::PgUp:			return VK_PRIOR;
	case EKeyCode::PgDown:			return VK_NEXT;
	case EKeyCode::End:				return VK_END;
	case EKeyCode::Home:			return VK_HOME;
	case EKeyCode::Insert:			return VK_INSERT;

	case EKeyCode::Esc:				return VK_ESCAPE;
	case EKeyCode::GraveAccent:		return VK_OEM_3;
	case EKeyCode::Tab:				return VK_TAB;
	case EKeyCode::CapsLock:		return VK_CAPITAL;

	case EKeyCode::Plus:			return VK_OEM_PLUS;
	case EKeyCode::Minus:			return VK_OEM_MINUS;
	case EKeyCode::BackSpace:		return VK_BACK;
	case EKeyCode::BackSlash:		return VK_OEM_5;
	case EKeyCode::Enter:			return VK_RETURN;

	case EKeyCode::Pause:			return VK_PAUSE;
	case EKeyCode::ScrollLock:		return VK_SCROLL;
	case EKeyCode::PrintScreen:		return VK_SNAPSHOT;

	case EKeyCode::Space:			return VK_SPACE;

	case EKeyCode::MouseLeftButton:	return VK_LBUTTON;
	case EKeyCode::MouseRightButton:return VK_RBUTTON;
	case EKeyCode::MouseMidButton:	return VK_MBUTTON;
	}
	
	return 0;
}
