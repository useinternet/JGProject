#pragma once
#include <string>
#include <Windows.h>
class KeyCode
{
public:
	enum EnumKeyCode
	{
		// 숫자 키코드
		Num0 = 0x30,
		Num1 = 0x31,
		Num2 = 0x32,
		Num3 = 0x33,
		Num4 = 0x34,
		Num5 = 0x35,
		Num6 = 0x36,
		Num7 = 0x37,
		Num8 = 0x38,
		Num9 = 0x39,
		// 숫자 패드 키코드
		NumPad0 = 0x60,
		NumPad1 = 0x61,
		NumPad2 = 0x62,
		NumPad3 = 0x63,
		NumPad4 = 0x64,
		NumPad5 = 0x65,
		NumPad6 = 0x66,
		NumPad7 = 0x67,
		NumPad8 = 0x68,
		NumPad9 = 0x69,
		// F 숫자 키코드
		F1 = 0x70,
		F2 = 0x71,
		F3 = 0x72,
		F4 = 0x73,
		F5 = 0x74,
		F6 = 0x75,
		F7 = 0x76,
		F8 = 0x77,
		F9 = 0x78,
		F10 = 0x79,
		F11 = 0x7A,
		F12 = 0x7B,
		// 영어 키코드
		A = 0x41,
		B = 0x42,
		C = 0x43,
		D = 0x44,
		E = 0x45,
		F = 0x46,
		G = 0x47,
		H = 0x48,
		I = 0x49,
		J = 0x4A,
		K = 0x4B,
		L = 0x4C,
		N = 0x4E,
		M = 0x4D,
		O = 0x4F,
		P = 0x50,
		Q = 0x51,
		R = 0x52,
		S = 0x53,
		T = 0x54,
		U = 0x55,
		V = 0x56,
		W = 0x57,
		X = 0x58,
		Y = 0x59,
		Z = 0x5A,

		LeftMouseButton = VK_LBUTTON,
		RightMouseButton = VK_RBUTTON,
		MouseWheelButton = VK_MBUTTON,
		BackSpace = VK_BACK,
		Tab = VK_TAB,
		Clear = VK_CLEAR,
		Enter = VK_RETURN,
		Shift = VK_SHIFT,
		Ctrl = VK_CONTROL,
		Alt = VK_MENU,
		Pause = VK_PAUSE,
		CapsLock = VK_CAPITAL,
		Esc = VK_ESCAPE,
		Space = VK_SPACE,
		PgUp = VK_PRIOR,
		PgDown = VK_NEXT,
		End = VK_END,
		Home = VK_HOME,
		Left = VK_LEFT,
		Up = VK_UP,
		Right = VK_RIGHT,
		Down = VK_DOWN,
		Select = VK_SELECT,
		Print = VK_PRINT,
		Execute = VK_EXECUTE,
		PrintScreen = VK_SNAPSHOT,
		Insert = VK_INSERT,
		Help = VK_HELP,
		NumPadAdd = VK_ADD,
		NumPadMultiply = VK_MULTIPLY,
		NumPadSubtract = VK_SUBTRACT,
		NumPadDecimal = VK_DECIMAL,
		NumPadDivide = VK_DIVIDE,
		NumLock = VK_NUMLOCK,
		ScrollLock = VK_SCROLL,
		LeftShift = VK_LSHIFT,
		RightShift = VK_RSHIFT,
		LeftCtrl = VK_LCONTROL,
		RightCtrl = VK_RCONTROL,
		LeftAlt = VK_LMENU,
		RightAlt = VK_RMENU,
	};
public:
	KeyCode(EnumKeyCode code) : code(code) {}
	KeyCode(WPARAM code) : KeyCode((EnumKeyCode)code) {}
	KeyCode(int code) : KeyCode((EnumKeyCode)code) {}
	KeyCode() : code(-1) {}
 public:
	KeyCode& operator=(const KeyCode& code) {
		this->code = code.code;
		return *this;
	}
	bool operator==(const KeyCode& code) const {
		return this->code == code.code;
	}
	bool operator!=(const KeyCode& code) const {
		return this->code != code.code;
		
	}
	KeyCode& operator=(KeyCode::EnumKeyCode code) {
		this->code = code;
		return *this;
	}
	bool operator==( KeyCode::EnumKeyCode code) const {
		return this->code == code;
	}
	bool operator!=( KeyCode::EnumKeyCode code) const {
		return this->code != code;

	}
	bool operator<(const KeyCode& code) const
	{
		return this->code < code.code;
	}
	bool operator>(const KeyCode& code) const
	{
		return this->code > code.code;
	}
	bool operator<=(const KeyCode& code) const
	{
		return this->code <= code.code;
	}
	bool operator>=(const KeyCode& code) const
	{
		return this->code >= code.code;
	}
public:
	int ToInt() const {
		return code;
	}
	std::string ToString() const
	{
		switch (code)
		{
		case Num0:
			return "0";
		case Num1:
			return "1";
		case Num2:
			return "2";
		case Num3:
			return "3";
		case Num4:
			return "4";
		case Num5:
			return "5";
		case Num6:
			return "6";
		case Num7:
			return "7";
		case Num8:
			return "8";
		case Num9:
			return "9";
		case NumPad0:
			return "NumPad0";
		case NumPad1:
			return "NumPad1";
		case NumPad2:
			return "NumPad2";
		case NumPad3:
			return "NumPad3";
		case NumPad4:
			return "NumPad4";
		case NumPad5:
			return "NumPad5";
		case NumPad6:
			return "NumPad6";
		case NumPad7:
			return "NumPad7";
		case NumPad8:
			return "NumPad8";
		case NumPad9:
			return "NumPad9";
		case F1:
			return "F1";
		case F2:
			return "F2";
		case F3:
			return "F3";
		case F4:
			return "F4";
		case F5:
			return "F5";
		case F6:
			return "F6";
		case F7:
			return "F7";
		case F8:
			return "F8";
		case F9:
			return "F9";
		case F10:
			return "F10";
		case F11:
			return "F11";
		case F12:
			return "F12";
		case A:
			return "A";
		case B:
			return "B";
		case C:
			return "C";
		case D:
			return "D";
		case E:
			return "E";
		case F:
			return "F";
		case G:
			return "G";
		case H:
			return "H";
		case I:
			return "I";
		case J:
			return "J";
		case K:
			return "K";
		case L:
			return "L";
		case N:
			return "N";
		case M:
			return "M";
		case O:
			return "O";
		case P:
			return "P";
		case Q:
			return "Q";
		case R:
			return "R";
		case S:
			return "S";
		case T:
			return "T";
		case U:
			return "U";
		case V:
			return "V";
		case W:
			return "W";
		case X:
			return "X";
		case Y:
			return "Y";
		case Z:
			return "Z";
			// Tab, CapsLock, LeftShift, RightShift, LeftCtrl, RightCtrl, LeftAlt, RightAlt, Enter,
			// Esc

		case LeftMouseButton: 
			return "LeftMouseButton";
		case RightMouseButton:
			return "RightMouseButton";
		case MouseWheelButton:
			return "MouseWheelButton";
		case BackSpace:
			return "BackSpace";
		case Tab:
			return "Tab";
		case Clear:
			return "Clear";
		case Enter:
			return "Enter";
		case Shift:
			return "Shift";
		case Ctrl:
			return "Ctrl";
		case Alt:
			return "Alt";
		case Pause:
			return "Pause";
		case CapsLock:
			return "CapsLock";
		case Esc:
			return "Esc";
		case Space:
			return "Space";
		case PgUp:
			return "PgUp";
		case PgDown:
			return "PgDown";
		case End:
			return "End";
		case Home:
			return "Home";
		case Left:
			return "Left";
		case Up:
			return "Up";
		case Right:
			return "Right";
		case Down:
			return "Down";
		case Select:
			return "Select";
		case Print:
			return "Print";
		case Execute:
			return "Execute";
		case PrintScreen:
			return "PrintScreen";
		case Insert:
			return "Insert";
		case Help:
			return "Help";
		case NumPadAdd:
			return "NumPadAdd";
		case NumPadMultiply:
			return "NumPadMultiply";
		case NumPadSubtract:
			return "NumPadSubtract";
		case NumPadDecimal:
			return "NumPadDecimal";
		case NumPadDivide:
			return "NumPadDivide";
		case NumLock:
			return "NumLock";
		case ScrollLock:
			return "ScrollLock";
		case LeftShift:
			return "LeftShift";
		case RightShift:
			return "RightShift";
		case LeftCtrl:
			return "LeftCtrl";
		case RightCtrl:
			return "RightCtrl";
		case LeftAlt:
			return "LeftAlt";
		case RightAlt:
			return "RightAlt";
		default:
			return "None";
		}
	}
private:
	unsigned int code = -1;
};