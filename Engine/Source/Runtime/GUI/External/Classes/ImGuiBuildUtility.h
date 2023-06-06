#pragma once
#include "Core.h"
#include "GUIDefine.h"
#include "External/imgui/imgui.h"
#include "External/imgui/imgui_internal.h"

inline ImGuiKey ToImGuiKey(EKeyCode inKeyCode)
{
	switch (inKeyCode)
	{
	case EKeyCode::Tab:		return ImGuiKey_Tab;
	case EKeyCode::Left:	return ImGuiKey_LeftArrow;
	case EKeyCode::Right:	return ImGuiKey_RightArrow;
	case EKeyCode::Up:		return ImGuiKey_UpArrow;
	case EKeyCode::Down:	return ImGuiKey_DownArrow;
	case EKeyCode::PgUp:	return ImGuiKey_PageUp;
	case EKeyCode::PgDown:	return ImGuiKey_PageDown;
	case EKeyCode::Home:	return ImGuiKey_Home;
	case EKeyCode::End:		return ImGuiKey_End;
	case EKeyCode::Insert:	return ImGuiKey_Insert;
	case EKeyCode::Del:		return ImGuiKey_Delete;
	case EKeyCode::BackSpace: return ImGuiKey_Backspace;
	case EKeyCode::Space:	return ImGuiKey_Space;
	case EKeyCode::Enter:	return ImGuiKey_Enter;
	case EKeyCode::Esc:		return ImGuiKey_Escape;
	case EKeyCode::Num0:	return ImGuiKey_0;
	case EKeyCode::Num1:	return ImGuiKey_1;
	case EKeyCode::Num2:	return ImGuiKey_2;
	case EKeyCode::Num3:	return ImGuiKey_3;
	case EKeyCode::Num4:	return ImGuiKey_4;
	case EKeyCode::Num5:	return ImGuiKey_5;
	case EKeyCode::Num6:	return ImGuiKey_6;
	case EKeyCode::Num7:	return ImGuiKey_7;
	case EKeyCode::Num8:	return ImGuiKey_8;
	case EKeyCode::Num9:	return ImGuiKey_9;
	case EKeyCode::A:		return ImGuiKey_A;
	case EKeyCode::B:		return ImGuiKey_B;
	case EKeyCode::C:		return ImGuiKey_C;
	case EKeyCode::D:		return ImGuiKey_D;
	case EKeyCode::E:		return ImGuiKey_E;
	case EKeyCode::F:		return ImGuiKey_F;
	case EKeyCode::G:		return ImGuiKey_G;
	case EKeyCode::H:		return ImGuiKey_H;
	case EKeyCode::I:		return ImGuiKey_I;
	case EKeyCode::J:		return ImGuiKey_J;
	case EKeyCode::K:		return ImGuiKey_K;
	case EKeyCode::L:		return ImGuiKey_L;
	case EKeyCode::M:		return ImGuiKey_M;
	case EKeyCode::N:		return ImGuiKey_N;
	case EKeyCode::O:		return ImGuiKey_O;
	case EKeyCode::P:		return ImGuiKey_P;
	case EKeyCode::Q:		return ImGuiKey_Q;
	case EKeyCode::R:		return ImGuiKey_R;
	case EKeyCode::S:		return ImGuiKey_S;
	case EKeyCode::T:		return ImGuiKey_T;
	case EKeyCode::U:		return ImGuiKey_U;
	case EKeyCode::V:		return ImGuiKey_V;
	case EKeyCode::W:		return ImGuiKey_W;
	case EKeyCode::X:		return ImGuiKey_X;
	case EKeyCode::Y:		return ImGuiKey_Y;
	case EKeyCode::Z:		return ImGuiKey_Z;
	case EKeyCode::F1:		return ImGuiKey_F1;
	case EKeyCode::F2:		return ImGuiKey_F2;
	case EKeyCode::F3:		return ImGuiKey_F3;
	case EKeyCode::F4:		return ImGuiKey_F4;
	case EKeyCode::F5:		return ImGuiKey_F5;
	case EKeyCode::F6:		return ImGuiKey_F6;
	case EKeyCode::F7:		return ImGuiKey_F7;
	case EKeyCode::F8:		return ImGuiKey_F8;
	case EKeyCode::F9:		return ImGuiKey_F9;
	case EKeyCode::F10:		return ImGuiKey_F10;
	case EKeyCode::F11:		return ImGuiKey_F11;
	case EKeyCode::F12:		return ImGuiKey_F12;
	}
	return ImGuiKey_None;
}

inline ImVec4 LinearColorToImVec4(const HLinearColor& inColor)
{
	return ImVec4(inColor.R, inColor.G, inColor.B, inColor.A);
}

inline ImU32 LinearColorToImU32(const HLinearColor& inColor)
{
	return ImGui::ColorConvertFloat4ToU32(LinearColorToImVec4(inColor));
}

inline ImVec2 Vector2ToImVec2(const HVector2& inVec)
{
	return ImVec2(inVec.x, inVec.y);
}

