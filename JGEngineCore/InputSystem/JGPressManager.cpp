#include"JGPressManager.h"
#include"../EngineStatics/JMath/JGVector2D.h"


JGPressManager::JGPressManager()
{

}
JGPressManager::~JGPressManager()
{

}

void JGPressManager::LinkInputSystemKeyBoardState(KeyBoardState* state, MouseState* MouseState, JGVector2D* MousePosPointer)
{
	m_pKeyBoardState = state;
	m_pMouseState = MouseState;
	m_pMousePos = MousePosPointer;
}

bool JGPressManager::IsPressed(const EKey Key)
{

	switch (Key)
	{

		// 숫자 키
	case EKey::_0:
		if (m_pKeyBoardState[DIK_0] & 0x80) return true;
		break;
	case EKey::_1:
		if (m_pKeyBoardState[DIK_1] & 0x80) return true;
		break;
	case EKey::_2:
		if (m_pKeyBoardState[DIK_2] & 0x80) return true;
		break;
	case EKey::_3:
		if (m_pKeyBoardState[DIK_3] & 0x80) return true;
		break;
	case EKey::_4:
		if (m_pKeyBoardState[DIK_4] & 0x80) return true;
		break;
	case EKey::_5:
		if (m_pKeyBoardState[DIK_5] & 0x80) return true;
		break;
	case EKey::_6:
		if (m_pKeyBoardState[DIK_6] & 0x80) return true;
		break;
	case EKey::_7:
		if (m_pKeyBoardState[DIK_7] & 0x80) return true;
		break;
	case EKey::_8:
		if (m_pKeyBoardState[DIK_8] & 0x80) return true;
		break;
	case EKey::_9:
		if (m_pKeyBoardState[DIK_9] & 0x80) return true;
		break;

		// F숫자키
	case EKey::F1:
		if (m_pKeyBoardState[DIK_F1] & 0x80) return true;
		break;
	case EKey::F2:
		if (m_pKeyBoardState[DIK_F2] & 0x80) return true;
		break;
	case EKey::F3:
		if (m_pKeyBoardState[DIK_F3] & 0x80) return true;
		break;
	case EKey::F4:
		if (m_pKeyBoardState[DIK_F4] & 0x80) return true;
		break;
	case EKey::F5:
		if (m_pKeyBoardState[DIK_F5] & 0x80) return true;
		break;
	case EKey::F6:
		if (m_pKeyBoardState[DIK_F6] & 0x80) return true;
		break;
	case EKey::F7:
		if (m_pKeyBoardState[DIK_F7] & 0x80) return true;
		break;
	case EKey::F8:
		if (m_pKeyBoardState[DIK_F8] & 0x80) return true;
		break;
	case EKey::F9:
		if (m_pKeyBoardState[DIK_F9] & 0x80) return true;
		break;

		// 넘패드 키

	case EKey::NumPad0:
		if (m_pKeyBoardState[DIK_NUMPAD0] & 0x80) return true;
		break;
	case EKey::NumPad1:
		if (m_pKeyBoardState[DIK_NUMPAD1] & 0x80) return true;
		break;
	case EKey::NumPad2:
		if (m_pKeyBoardState[DIK_NUMPAD2] & 0x80) return true;
		break;
	case EKey::NumPad3:
		if (m_pKeyBoardState[DIK_NUMPAD3] & 0x80) return true;
		break;
	case EKey::NumPad4:
		if (m_pKeyBoardState[DIK_NUMPAD4] & 0x80) return true;
		break;
	case EKey::NumPad5:
		if (m_pKeyBoardState[DIK_NUMPAD5] & 0x80) return true;
		break;
	case EKey::NumPad6:
		if (m_pKeyBoardState[DIK_NUMPAD6] & 0x80) return true;
		break;
	case EKey::NumPad7:
		if (m_pKeyBoardState[DIK_NUMPAD7] & 0x80) return true;
		break;
	case EKey::NumPad8:
		if (m_pKeyBoardState[DIK_NUMPAD8] & 0x80) return true;
		break;
	case EKey::NumPad9:
		if (m_pKeyBoardState[DIK_NUMPAD9] & 0x80) return true;
		break;
	case EKey::NumLock:
		if (m_pKeyBoardState[DIK_NUMLOCK] & 0x80) return true;
		break;
	case EKey::NumPadEnter:
		if (m_pKeyBoardState[DIK_NUMPADENTER] & 0x80) return true;
		break;

	// 기능 키
	case EKey::Escape:
		if (m_pKeyBoardState[DIK_ESCAPE] & 0x80) return true;
		break;
	case EKey::Delete:
		if (m_pKeyBoardState[DIK_DELETE] & 0x80) return true;
		break;
	case EKey::End:
		if (m_pKeyBoardState[DIK_END] & 0x80) return true;
		break;
	case EKey::Home:
		if (m_pKeyBoardState[DIK_HOME] & 0x80) return true;
		break;
	case EKey::Insert:
		if (m_pKeyBoardState[DIK_INSERT] & 0x80) return true;
		break;
	case EKey::PgDn:
		if (m_pKeyBoardState[DIK_PGDN] & 0x80) return true;
		break;
	case EKey::PgUp:
		if (m_pKeyBoardState[DIK_PGUP] & 0x80) return true;
		break;
	case EKey::Shift:
		if (m_pKeyBoardState[DIK_LSHIFT] & 0x80) return true;
		if (m_pKeyBoardState[DIK_RSHIFT] & 0x80) return true;
		break;
	case EKey::Ctrl:
		if (m_pKeyBoardState[DIK_LCONTROL] & 0x80) return true;
		if (m_pKeyBoardState[DIK_RCONTROL] & 0x80) return true;
		break;
	case EKey::Alt:
		if (m_pKeyBoardState[DIK_LALT] & 0x80) return true;
		if (m_pKeyBoardState[DIK_RALT] & 0x80) return true;
		break;
	case EKey::CapsLock:
		if (m_pKeyBoardState[DIK_CAPSLOCK] & 0x80) return true;
		break;
	case EKey::Tab:
		if (m_pKeyBoardState[DIK_TAB] & 0x80) return true;
		break;
		// 알파벳
	case EKey::A:
		if (m_pKeyBoardState[DIK_A] & 0x80) return true;
		break;
	case EKey::B:
		if (m_pKeyBoardState[DIK_B] & 0x80) return true;
		break;
	case EKey::C:
		if (m_pKeyBoardState[DIK_C] & 0x80) return true;
		break;
	case EKey::D:
		if (m_pKeyBoardState[DIK_D] & 0x80) return true;
		break;
	case EKey::E:
		if (m_pKeyBoardState[DIK_E] & 0x80) return true;
		break;
	case EKey::F:
		if (m_pKeyBoardState[DIK_F] & 0x80) return true;
		break;
	case EKey::G:
		if (m_pKeyBoardState[DIK_G] & 0x80) return true;
		break;
	case EKey::H:
		if (m_pKeyBoardState[DIK_H] & 0x80) return true;
		break;
	case EKey::I:
		if (m_pKeyBoardState[DIK_I] & 0x80) return true;
		break;
	case EKey::J:
		if (m_pKeyBoardState[DIK_J] & 0x80) return true;
		break;
	case EKey::K:
		if (m_pKeyBoardState[DIK_K] & 0x80) return true;
		break;
	case EKey::L:
		if (m_pKeyBoardState[DIK_L] & 0x80) return true;
		break;
	case EKey::N:
		if (m_pKeyBoardState[DIK_N] & 0x80) return true;
		break;
	case EKey::M:
		if (m_pKeyBoardState[DIK_M] & 0x80) return true;
		break;
	case EKey::O:
		if (m_pKeyBoardState[DIK_O] & 0x80) return true;
		break;
	case EKey::P:
		if (m_pKeyBoardState[DIK_P] & 0x80) return true;
		break;
	case EKey::Q:
		if (m_pKeyBoardState[DIK_Q] & 0x80) return true;
		break;
	case EKey::R:
		if (m_pKeyBoardState[DIK_R] & 0x80) return true;
		break;
	case EKey::S:
		if (m_pKeyBoardState[DIK_S] & 0x80) return true;
		break;
	case EKey::T:
		if (m_pKeyBoardState[DIK_T] & 0x80) return true;
		break;
	case EKey::U:
		if (m_pKeyBoardState[DIK_U] & 0x80) return true;
		break;
	case EKey::V:
		if (m_pKeyBoardState[DIK_V] & 0x80) return true;
		break;
	case EKey::W:
		if (m_pKeyBoardState[DIK_W] & 0x80) return true;
		break;
	case EKey::X:
		if (m_pKeyBoardState[DIK_X] & 0x80) return true;
		break;
	case EKey::Y:
		if (m_pKeyBoardState[DIK_Y] & 0x80) return true;
		break;
	case EKey::Z:
		if (m_pKeyBoardState[DIK_Z] & 0x80) return true;
		break;
		//if(diMouseState.rgbButtons[0] & 0x80)
		// 마우스 상태
	case EKey::MouseLButton:
		if (m_pMouseState->rgbButtons[0] & 0x80) return true;
		break;
	case EKey::MouseRButton:
		if (m_pMouseState->rgbButtons[1] & 0x80) return true;
		break;
	case EKey::MouseWheelClick:
		if (m_pMouseState->rgbButtons[2] & 0x80) return true;
		break;
	case EKey::Up:
		if (m_pKeyBoardState[DIK_UP] & 0x80) return true;
		break;
	case EKey::Down:
		if (m_pKeyBoardState[DIK_DOWN] & 0x80) return true;
		break;
	case EKey::Right:
		if (m_pKeyBoardState[DIK_RIGHT] & 0x80) return true;
		break;
	case EKey::Left:
		if (m_pKeyBoardState[DIK_LEFT] & 0x80) return true;
		break;
	default:
		return false;
	}
	return false;
}
float JGPressManager::GetAxisKey(const EKey Key)
{
	switch (Key)
	{
	case EKey::MouseXAxis:
		return (float)m_pMouseState->lX;
	case EKey::MouseYAxis:
		return (float)m_pMouseState->lY;
	case EKey::MouseWheelAxis:
		return (float)m_pMouseState->lZ;
	default:
		return 0.0f;
	}
}

JGVector2D& JGPressManager::GetMouseLocation()
{
	return *m_pMousePos;
}
