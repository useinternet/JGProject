#pragma once
#include <memory>
#include <Windows.h>
#include "IE.h"
#include "DataType/Abstract/EngineCore.h"

namespace IE
{
	class INPUTEGINE_API InputEngine : public EngineCore
	{
		struct MousePos
		{
			float x, y;
		};
	public:
		InputEngine(const GlobalLinkStream& stream);
		virtual ~InputEngine() = default;
	public:
		void Init(HWND hWnd);
		virtual void Update() override;
		virtual void OnEvent(Event& e) override;
	private:
		enum EKeyState
		{
			Key_None = 0,
			Key_Pressed = 1,
			Key_Released = 2,
			Key_Click = 3,
			Key_DoubleClick = 4,
			KeyMapCount = 256
		};
	private:
		void KeyboardUpdate(EKeyState state, unsigned int code);
		void MouseUpdate(EKeyState state, unsigned int  code);
	private:
		HWND m_hWnd;
		EKeyState m_KeyMap[KeyMapCount];
		MousePos m_MousePos;
		MousePos m_PrevMousePos;

	};
}