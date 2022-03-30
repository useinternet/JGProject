#pragma once
#include "Define.h"
#include <Windows.h>
#include <string>

#define ENUM_FLAG(enumName) \
enum class enumName; \
inline enumName operator|(enumName c1, enumName c2) \
{ \
	return (enumName)((int)c1 | (int)c2); \
} \
inline bool operator&(enumName c1, enumName c2) \
{ \
	return (bool)((int)c1 & (int)c2); \
} \

namespace JG
{
	enum class EWindowPlatform
	{
		Windows,
	};
	enum class EGraphicsAPI
	{
		DirectX12,
		DirectX11,
		OpenGL,
		Vulkan,
	};
	inline String GraphicsAPIToString(EGraphicsAPI api)
	{
		switch(api)
		{
			case EGraphicsAPI::DirectX12: return "DirectX12";
			case EGraphicsAPI::DirectX11: return "DirectX11";
			case EGraphicsAPI::OpenGL:    return "OpenGL";
			case EGraphicsAPI::Vulkan:    return "Vulkan";
			default: return "None";
		}
	}
	enum class EAssetFormat
	{
		None = 0,
		Mesh      = 1,
		Skeletal  = 2,
		Material  = 3,
		Texture   = 4,
		Directory = 5,
		GameWorld = 6,
		CubeMap   = 7,
		Prefab    = 8,
	};

	enum class ERendererPath
	{
		Foward,
		Deferred,
		RayTracing,
		Simple,
	};

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

		MouseLeftButton  = VK_LBUTTON,
		MouseRightButton = VK_RBUTTON,
		MouseMidButton	 = VK_MBUTTON,
		Mouse_X = 0x100,
		Mouse_Y = 0x101,
	};
	inline String KeyCodeToString(EKeyCode code)
	{
		switch (code)
		{
		case EKeyCode::Num0:			return "0";
		case EKeyCode::Num1:			return "1";
		case EKeyCode::Num2:			return "2";
		case EKeyCode::Num3:			return "3";
		case EKeyCode::Num4:			return "4";
		case EKeyCode::Num5:			return "5";
		case EKeyCode::Num6:			return "6";
		case EKeyCode::Num7:			return "7";
		case EKeyCode::Num8:			return "8";
		case EKeyCode::Num9:			return "9";
		case EKeyCode::NumPad0:			return "NumPad0";
		case EKeyCode::NumPad1:			return "NumPad1";
		case EKeyCode::NumPad2:			return "NumPad2";
		case EKeyCode::NumPad3:			return "NumPad3";
		case EKeyCode::NumPad4:			return "NumPad4";
		case EKeyCode::NumPad5:			return "NumPad5";
		case EKeyCode::NumPad6:			return "NumPad6";
		case EKeyCode::NumPad7:			return "NumPad7";
		case EKeyCode::NumPad8:			return "NumPad8";
		case EKeyCode::NumPad9:			return "NumPad9";
		case EKeyCode::F1:				return "F1";
		case EKeyCode::F2:				return "F2";
		case EKeyCode::F3:				return "F3";
		case EKeyCode::F4:				return "F4";
		case EKeyCode::F5:				return "F5";
		case EKeyCode::F6:				return "F6";
		case EKeyCode::F7:				return "F7";
		case EKeyCode::F8:				return "F8";
		case EKeyCode::F9:				return "F9";
		case EKeyCode::F10:				return "F10";
		case EKeyCode::F11:				return "F11";
		case EKeyCode::F12:				return "F12";
		case EKeyCode::A:				return "A";
		case EKeyCode::B:				return "B";
		case EKeyCode::C:				return "C";
		case EKeyCode::D:				return "D";
		case EKeyCode::E:				return "E";
		case EKeyCode::F:				return "F";
		case EKeyCode::G:				return "G";
		case EKeyCode::H:				return "H";
		case EKeyCode::I:				return "I";
		case EKeyCode::J:				return "J";
		case EKeyCode::K:				return "K";
		case EKeyCode::L:				return "L";
		case EKeyCode::N:				return "N";
		case EKeyCode::M:				return "M";
		case EKeyCode::O:				return "O";
		case EKeyCode::P:				return "P";
		case EKeyCode::Q:				return "Q";
		case EKeyCode::R:				return "R";
		case EKeyCode::S:				return "S";
		case EKeyCode::T:				return "T";
		case EKeyCode::U:				return "U";
		case EKeyCode::V:				return "V";
		case EKeyCode::W:				return "W";
		case EKeyCode::X:				return "X";
		case EKeyCode::Y:				return "Y";
		case EKeyCode::Z:				return "Z";
		case EKeyCode::BackSpace:		return "BackSpace";
		case EKeyCode::Tab:				return "Tab";
		case EKeyCode::Clear:			return "Clear";
		case EKeyCode::Enter:			return "Enter";
		case EKeyCode::Shift:			return "Shift";
		case EKeyCode::Ctrl:			return "Ctrl";
		case EKeyCode::Alt:				return "Alt";
		case EKeyCode::Pause:			return "Pause";
		case EKeyCode::CapsLock:		return "CapsLock";
		case EKeyCode::Esc:				return "Esc";
		case EKeyCode::Space:			return "Space";
		case EKeyCode::PgUp:			return "PgUp";
		case EKeyCode::PgDown:			return "PgDown";
		case EKeyCode::End:				return "End";
		case EKeyCode::Home:			return "Home";
		case EKeyCode::Left:			return "Left";
		case EKeyCode::Up:				return "Up";
		case EKeyCode::Right:			return "Right";
		case EKeyCode::Down:			return "Down";
		case EKeyCode::Select:			return "Select";
		case EKeyCode::Print:			return "Print";
		case EKeyCode::Execute:			return "Execute";
		case EKeyCode::PrintScreen:		return "PrintScreen";
		case EKeyCode::Insert:			return "Insert";
		case EKeyCode::Help:			return "Help";
		case EKeyCode::NumPadAdd:		return "NumPadAdd";
		case EKeyCode::NumPadMultiply:	return "NumPadMultiply";
		case EKeyCode::NumPadSubtract:	return "NumPadSubtract";
		case EKeyCode::NumPadDemical:	return "NumPadDecimal";
		case EKeyCode::NumPadDivide:	return "NumPadDivide";
		case EKeyCode::NumLock:			return "NumLock";
		case EKeyCode::ScrollLock:		return "ScrollLock";
		case EKeyCode::LeftShift:		return "LeftShift";
		case EKeyCode::RightShift:		return "RightShift";
		case EKeyCode::LeftCtrl:		return "LeftCtrl";
		case EKeyCode::RightCtrl:		return "RightCtrl";
		case EKeyCode::LeftAlt:			return "LeftAlt";
		case EKeyCode::RightAlt:		return "RightAlt";
		case EKeyCode::LeftBracket:		return "[";
		case EKeyCode::RightBracket:	return "]";
		case EKeyCode::BackSlash:		return "'\'";
		case EKeyCode::QuotationMark:	return "'";
		case EKeyCode::Semicolon:		return ";";
		case EKeyCode::Comma:			return ",";
		case EKeyCode::Plus:			return "+";
		case EKeyCode::Minus:			return "-";
		case EKeyCode::Period:			return ".";
		case EKeyCode::Slash:			return "/";
		case EKeyCode::GraveAccent:		return "`";
		case EKeyCode::MouseLeftButton: return "MouseLeftButton";
		case EKeyCode::MouseRightButton:return "MouseRightButton";
		case EKeyCode::MouseMidButton:  return "MouseMidButton";
			

		case EKeyCode::Mouse_X: return "Mouse_X";
		case EKeyCode::Mouse_Y: return "Mouse_Y";
		default:
			return "None";
		}
	}


	inline EKeyCode StringToKeyCode(const String& code)
	{
		
		if (code == "0") { return  EKeyCode::Num0; }
		else if(code == "1" ) { return  EKeyCode::Num1;	}		
		else if(code == "2" ) { return  EKeyCode::Num2;	}		
		else if(code == "3" ) { return  EKeyCode::Num3;	}		
		else if(code == "4" ) { return  EKeyCode::Num4;	}		
		else if(code == "5" ) { return  EKeyCode::Num5;	}		
		else if(code == "6" ) { return  EKeyCode::Num6;	}		
		else if(code == "7" ) { return  EKeyCode::Num7;	}		
		else if(code == "8" ) { return  EKeyCode::Num8;	}		
		else if(code == "9" ) { return  EKeyCode::Num9;	}		
		else if(code == "NumPad0") {return  EKeyCode::NumPad0; }		
		else if(code == "NumPad1") {return  EKeyCode::NumPad1; }		
		else if(code == "NumPad2") {return  EKeyCode::NumPad2; }		
		else if(code == "NumPad3") {return  EKeyCode::NumPad3; }		
		else if(code == "NumPad4") {return  EKeyCode::NumPad4; }		
		else if(code == "NumPad5") {return  EKeyCode::NumPad5; }		
		else if(code == "NumPad6") {return  EKeyCode::NumPad6; }		
		else if(code == "NumPad7") {return  EKeyCode::NumPad7; }		
		else if(code == "NumPad8") {return  EKeyCode::NumPad8; }		
		else if(code == "NumPad9") {return  EKeyCode::NumPad9; }		
		else if(code == "F1" ) { return EKeyCode::F1; }				
		else if(code == "F2" ) { return EKeyCode::F2; }				
		else if(code == "F3" ) { return EKeyCode::F3; }				
		else if(code == "F4" ) { return EKeyCode::F4; }				
		else if(code == "F5" ) { return EKeyCode::F5; }				
		else if(code == "F6" ) { return EKeyCode::F6; }				
		else if(code == "F7" ) { return EKeyCode::F7; }				
		else if(code == "F8" ) { return EKeyCode::F8; }				
		else if(code == "F9" ) { return EKeyCode::F9; }				
		else if(code == "F10") { return EKeyCode::F10;}				
		else if(code == "F11") { return EKeyCode::F11;}				
		else if(code == "F12") { return EKeyCode::F12;}				
		else if(code == "A") { return EKeyCode::A; }				
		else if(code == "B") { return EKeyCode::B; }				
		else if(code == "C") { return EKeyCode::C; }				
		else if(code == "D") { return EKeyCode::D; }				
		else if(code == "E") { return EKeyCode::E; }				
		else if(code == "F") { return EKeyCode::F; }				
		else if(code == "G") { return EKeyCode::G; }				
		else if(code == "H") { return EKeyCode::H; }				
		else if(code == "I") { return EKeyCode::I; }				
		else if(code == "J") { return EKeyCode::J; }				
		else if(code == "K") { return EKeyCode::K; }				
		else if(code == "L") { return EKeyCode::L; }				
		else if(code == "N") { return EKeyCode::N; }				
		else if(code == "M") { return EKeyCode::M; }				
		else if(code == "O") { return EKeyCode::O; }				
		else if(code == "P") { return EKeyCode::P; }				
		else if(code == "Q") { return EKeyCode::Q; }				
		else if(code == "R") { return EKeyCode::R; }				
		else if(code == "S") { return EKeyCode::S; }				
		else if(code == "T") { return EKeyCode::T; }				
		else if(code == "U") { return EKeyCode::U; }				
		else if(code == "V") { return EKeyCode::V; }				
		else if(code == "W") { return EKeyCode::W; }				
		else if(code == "X") { return EKeyCode::X; }				
		else if(code == "Y") { return EKeyCode::Y; }				
		else if(code == "Z") { return EKeyCode::Z; }				
		else if (code == "BackSpace")	{ return EKeyCode::BackSpace; }
		else if (code == "Tab")			{ return EKeyCode::Tab; }
		else if (code == "Clear")		{ return EKeyCode::Clear; }
		else if (code == "Enter")		{ return EKeyCode::Enter; }
		else if (code == "Shift")		{ return EKeyCode::Shift; }
		else if (code == "Ctrl")		{ return EKeyCode::Ctrl; }
		else if (code == "Alt")			{ return EKeyCode::Alt; }
		else if (code == "Pause")		{ return EKeyCode::Pause; }
		else if (code == "CapsLock")	{ return EKeyCode::CapsLock; }
		else if (code == "Esc")			{ return EKeyCode::Esc; }
		else if (code == "Space")		{ return EKeyCode::Space; }
		else if (code == "PgUp")		{ return EKeyCode::PgUp; }
		else if (code == "PgDown")		{ return EKeyCode::PgDown; }
		else if (code == "End")			{ return EKeyCode::End; }
		else if (code == "Home")		{ return EKeyCode::Home; }
		else if (code == "Left")		{ return EKeyCode::Left; }
		else if (code == "Up")			{ return EKeyCode::Up; }
		else if (code == "Right")		{ return EKeyCode::Right; }
		else if (code == "Down")		{ return EKeyCode::Down; }
		else if (code == "Select")		{ return EKeyCode::Select; }
		else if (code == "Print")		{ return EKeyCode::Print; }
		else if (code == "Execute")		{ return EKeyCode::Execute; }
		else if (code == "PrintScreen") { return EKeyCode::PrintScreen; }
		else if (code == "Insert") { return EKeyCode::Insert; }
		else if (code == "Help") { return EKeyCode::Help; }
		else if (code == "NumPadAdd") { return EKeyCode::NumPadAdd; }
		else if (code == "NumPadMultiply") { return EKeyCode::NumPadMultiply; }
		else if (code == "NumPadSubtract") { return EKeyCode::NumPadSubtract; }
		else if (code == "NumPadDecimal") { return EKeyCode::NumPadDemical; }
		else if (code == "NumPadDivide") { return EKeyCode::NumPadDivide; }
		else if (code == "NumLock") { return EKeyCode::NumLock; }
		else if (code == "ScrollLock") { return EKeyCode::ScrollLock; }
		else if (code == "LeftShift") { return EKeyCode::LeftShift; }
		else if (code == "RightShift") { return EKeyCode::RightShift; }
		else if (code == "LeftCtrl") { return EKeyCode::LeftCtrl; }
		else if (code == "RightCtrl") { return EKeyCode::RightCtrl; }
		else if (code == "LeftAlt") { return EKeyCode::LeftAlt; }
		else if (code == "RightAlt") { return EKeyCode::RightAlt; }
		else if (code == "[") { return EKeyCode::LeftBracket; }
		else if (code == "]") { return EKeyCode::RightBracket; }
		else if (code == "'\'") { return EKeyCode::BackSlash; }
		else if (code == "'") { return EKeyCode::QuotationMark; }
		else if (code == ";") { return EKeyCode::Semicolon; }
		else if (code == ",") { return EKeyCode::Comma; }
		else if (code == "+") { return EKeyCode::Plus; }
		else if (code == "-") { return EKeyCode::Minus; }
		else if (code == ".") { return EKeyCode::Period; }
		else if (code == "/") { return EKeyCode::Slash; }
		else if (code == "`") { return EKeyCode::GraveAccent; }
		else if (code == "MouseLeftButton") { return EKeyCode::MouseLeftButton; }
		else if (code == "MouseRightButton") { return EKeyCode::MouseRightButton; }
		else if (code == "MouseMidButton") { return EKeyCode::MouseMidButton; }
		else if (code == "Mouse_X") { return EKeyCode::Mouse_X; }
		else if (code == "Mouse_Y") { return EKeyCode::Mouse_Y; }
		else { return EKeyCode::Unknown; }
	}


	enum class EInputAction
	{
		Pressed,
		Released,
	};
	inline String InputActionToString(EInputAction inputAction)
	{
		switch(inputAction)
		{
		case EInputAction::Pressed:  return "Pressed";
		case EInputAction::Released: return "Released";
		default:
			return "None";
		}
	}

	

	enum class EInputMode
	{
		Default,
	};

	enum class EMouseButton
	{
		Left,
		Right,
		Middle,
		XButton1,
		XButton2,
	};
	inline String MouseButtonToString(EMouseButton inputAction)
	{
		switch (inputAction)
		{
		case EMouseButton::Left:     return "LeftButton";
		case EMouseButton::Right:    return "RightButton";
		case EMouseButton::Middle:   return "MiddleButton";
		case EMouseButton::XButton1: return "XButton1";
		case EMouseButton::XButton2: return "XButton2";
		default:
			return "None";
		}
	}
}