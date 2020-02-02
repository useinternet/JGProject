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
		virtual void Load() override { m_hWnd = GetFocus(); }
		virtual void Update() override;
		virtual void OnEvent(Event& e) override;


		void Flush();
		void MouseFlush();
		bool GetKeyDown(KeyCode code);
		bool GetKeyUp(KeyCode code);

		const std::vector<KeyCode> GetKeyBoardDownList() const {
			return m_KeyDownList;
		}
		const std::vector<KeyCode> GetKeyBoardUpList() const {
			return m_KeyUpList;
		}
		const std::vector<KeyCode> GetMouseBtDownList() const {
			return m_MBtDownList;
		}
		const std::vector<KeyCode> GetMouseBtUpList() const {
			return m_MBtUpList;
		}




		const JVector2Int GetMousePosition();
		const JVector2Int GetMouseDelta();
		HWND GetHandle() {
			return m_hWnd;
		}
	public:
		void NotifyKeyDown(KeyCode code);
		void NotifyKeyUp(KeyCode code);
		void NotifyMousePos(int x, int y);
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
		EKeyState m_KeyMap[KeyMapCount];


		std::queue<KeyCode> m_KeyDownBuffer;
		std::queue<KeyCode> m_KeyUpBuffer;
		std::queue<KeyCode> m_KeyNoneBuffer;

		std::vector<KeyCode> m_KeyDownList;
		std::vector<KeyCode> m_KeyUpList;
		std::vector<KeyCode> m_MBtDownList;
		std::vector<KeyCode> m_MBtUpList;


		JVector2Int m_MousePos;
		JVector2Int m_PrevMousePos;
	};
}