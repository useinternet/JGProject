#pragma once
#include <memory>
#include <Windows.h>
#include "IE.h"
#include "DataType/Abstract/EngineCore.h"

namespace IE
{
	class INPUTEGINE_API InputEngine : public EngineCore
	{
	public:
		InputEngine(const GlobalLinkStream& stream);
		virtual ~InputEngine() = default;
	public:
		void Init(HWND hWnd);
		virtual void Load() override {}
		virtual void Update() override;
		virtual void OnEvent(Event& e) override;

		bool GetKeyDown(KeyCode code);
		bool GetKeyUp(KeyCode code);
		bool GetKeyAsButton(KeyCode code);


		const JVector2 GetMousePosition();
		const JVector2 GetMouseDelta();

		const JVector2 GetMousePositionFromScreen();
		const JVector2 GetMouseDeltaFromScreen();
	private:
		enum EKeyState
		{
			Key_None = 0,
			Key_Down = 1,
			Key_Up = 2,
			KeyMapCount = 256
		};
	private:
		HWND m_hWnd;

		//std::map<int, EKeyState> m_KeyMap;
		EKeyState m_KeyMap[KeyMapCount];
		EKeyState m_PrevKeyMap[KeyMapCount];
		JVector2 m_MousePos;
		JVector2 m_PrevMousePos;
		JVector2 m_MousePosFromScreen;
		JVector2 m_PrevMouseFromScreen;

	};
}