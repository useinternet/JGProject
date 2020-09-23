#include "pch.h"
#include "GUI.h"
#include "GUIIF.h"
#include "InputIF.h"
#include "Components/Transform.h"
#include "Windows/Form.h"


#include "Elements/Element.h"
using namespace std;



namespace JGUI
{
	

	LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{

		auto& screenPool = GUIIF::GetInstance()->m_ScreenPool;
		if (screenPool.empty()) return 0;


		GUIIO& io = GUIIF::GetIO();
		Window* win = nullptr;
		GUIIF::GUIScreen* screen = nullptr;
		auto iter = std::find_if(screenPool.begin(), screenPool.end(), 
			[&](const pair< Window*, std::shared_ptr<GUIIF::GUIScreen>>& _pair)
		{
			if (_pair.second->win == nullptr) return false;
			return _pair.second->win->GetHandle() == hWnd;
		});
		if (iter == screenPool.end()) return 0;
		win    = iter->first;
		screen = iter->second.get();
	

		
		std::function<void(int)> downFunc =
			[&](int bt)
		{
			if (win == nullptr) return;
			else if (GUIIF::GetInstance()->m_SelectedWindow && GUIIF::GetInstance()->m_SelectedWindow != win) return;
			GUIIF::GetInstance()->m_SelectedWindow = win;
			screen->mouseDown[bt] = true;
			switch (bt)
			{
			case 0: GUIIF::GetInstance()->UpdateKeyDuration(KeyCode::LeftMouseButton, win); break;
			case 1: GUIIF::GetInstance()->UpdateKeyDuration(KeyCode::RightMouseButton, win); break;
			case 2: GUIIF::GetInstance()->UpdateKeyDuration(KeyCode::MouseWheelButton, win); break;
			}
		};
		static bool isHangule = false;

		switch (msg)
		{

		case WM_LBUTTONDOWN:    downFunc(0); 	return 0;
		case WM_RBUTTONDOWN:    downFunc(1);    return 0;
		case WM_MBUTTONDOWN:    downFunc(2);    return 0;


		case WM_SETFOCUS:
			GUIIF::GetInstance()->m_FocusWindow = win;
			GUIIF::GetInstance()->m_FocusWindow->ReceiveFocusEvent(0);
			break;
		case WM_KILLFOCUS:
			win->ReceiveFocusEvent(1);
			GUIIF::GetInstance()->m_FocusWindow = nullptr;
			break;
		case WM_DROPFILES:
		{
			::DragAcceptFiles(hWnd, false);
			GUIDropItem item;
			uint32_t dropCount = ::DragQueryFileW((HDROP)wParam, 0xFFFFFFFF, nullptr, 0);
			for (uint32_t i = 0; i < dropCount; ++i)
			{
				std::wstring path;
				path.resize(256);
				::DragQueryFileW((HDROP)wParam, i, path.data(), 256);

				path = path.substr(0, path.find(TT('\0')));


				auto dropSource = make_unique<GUIDropSource<wstring>>();
				dropSource->DataSize = path.length();
				dropSource->Data = path;
				item.DropSources.push_back(move(dropSource));
			}
			win->ReceiveDropItemEvent(item);
			::DragAcceptFiles(hWnd, true);
		}
		return 0;
		case WM_CHAR:
		{
			HIMC hIMC = ::ImmGetContext(hWnd);
			DWORD conVersion, senTence;
			::ImmGetConversionStatus(hIMC, &conVersion, &senTence);
			if (conVersion == 1) return 0;
			
			std::string s;
			s.resize(1); s[0] = (char)wParam;
			std::wstring ws = s2ws(s);
			
			win->ReceiveCharEvent(ws[0]);
		}
			return 0;
		case WM_IME_CHAR:
		{
			HIMC hIMC = ::ImmGetContext(hWnd);
			DWORD conVersion, senTence;
			::ImmGetConversionStatus(hIMC, &conVersion, &senTence);
			if (conVersion == 0) return 0;
			win->ReceiveCharEvent((wchar_t)wParam);
		}

			return 0;
		default:
			return 0;
		}
		return 0;
	}
	static std::unique_ptr<GUIIF> g_Instance = nullptr;
	void GUI::Awake()
	{
		g_Instance = make_unique<GUIIF>();
		g_Instance->m_ReservedElements.resize(GUIIF::GetIO().FrameCount);
		g_Instance->m_ReservedWindows.resize(GUIIF::GetIO().FrameCount);
	}

	void GUI::Start()
	{
		GlobalSharedData::GetWindowManager()->BindWindowProcFunc(this, MainWndProc);

		GE::MaterialProperty ppt;
		ppt.rendererMode = GE::RendererMode::GUI;
		ppt.blendMode    = GE::BlendMode::Transparent;
		auto default_Material = GraphicsIF::RequestMaterial(GUI_PREGENERATED_MATERIAL_DEFAULT, ppt);
		{
			default_Material->DefineVarAsFloat4(GUI_MATERIAL_DATA_COLOR, JVector4(1.0f, 1.0f, 1.0f, 1.0f));
			default_Material->SetCode(R"(
pout.Albedo = Color_Data;
)");
			if (!GraphicsIF::PreCompileMaterial(default_Material.Get()))
			{
				GUILOG_ERROR("GUIMaterial_Default 사전 컴파일에 실패했습니다.");
			}
		}

		auto texture_Material = GraphicsIF::RequestMaterial(GUI_PREGENERATED_MATERIAL_IMAGE, ppt);
		{
			texture_Material->DefineVarAsFloat4(GUI_MATERIAL_DATA_COLOR, JVector4(1.0f, 1.0f, 1.0f, 1.0f));
			texture_Material->DefineTexture(GUI_MATERIAL_DATA_IMAGE, nullptr);
			texture_Material->SetCode(R"(
pout.Albedo = Image_Texture.Sample(g_AnisotropicSampler, pin.tex) * Color_Data;
)");
			if (!GraphicsIF::PreCompileMaterial(texture_Material.Get()))
			{
				GUILOG_ERROR("GUIMaterial_Texture 사전 컴파일에 실패했습니다.");
			}
		}
		auto text_Material = GraphicsIF::RequestMaterial(GUI_PREGENERATED_MATERIAL_TEXT, ppt);
		{
			text_Material->DefineVarAsFloat4(GUI_MATERIAL_DATA_COLOR, JVector4(1.0f, 1.0f, 1.0f, 1.0f));
			text_Material->DefineTexture(GUI_MATERIAL_DATA_IMAGE, nullptr);
			text_Material->SetCode(R"(
float4 textureColor = Image_Texture.Sample(g_LinearSampler, pin.tex).rgba;
pout.Albedo = textureColor * Color_Data;
)");

			if (!GraphicsIF::PreCompileMaterial(text_Material.Get()))
			{
				GUILOG_ERROR("GUIMaterial_Text 사전 컴파일에 실패했습니다.");
			}
		}

		GUIIF::LoadFont(GUIIF::GetIO().DefaultFont);
		GUIIF::GetInstance()->m_EditPlugin = make_unique<Plugin>();

		if (GUIIF::GetIO().FormDllPath == TT("none"))
		{
			GUIIF::CreateMainWindowForm<Form>(L"GUI", WindowFlag_MainWindow);
		}
		else
		{
			GUIIF::GetInstance()->m_EditPlugin->Load(ws2s(GUIIF::GetIO().FormDllPath));
			if (!GUIIF::GetInstance()->m_EditPlugin->IsValid())
			{
				GUILOG_ERROR("DLL 로드 실패");
				GUIIF::CreateMainWindowForm<Form>(L"GUI", WindowFlag_MainWindow);
			}
			else
			{
				LINK_GLOBAL_SHARED_DATA(g_Instance->m_EditPlugin)
				using Func = void(*)();
				Func func = (Func)(g_Instance->m_EditPlugin->GetProcAddress("_CreateMainWindowForm"));
				func();
			}
		}
	}

	void GUI::Update()
	{


		if (g_Instance->m_IsCloseApp)
		{
			g_Instance->m_MainWindow->GUIDestroy();
			return;
		}

		auto& io = GUIIF::GetIO();
		io.MousePos = InputIF::GetCursorPos();
		
		g_Instance->ProcessGarbageCollection(GUIIF::GetTick());
		
		g_Instance->UpdateWindow();
		g_Instance->UpdateScreen();
		g_Instance->UpdateEvent();


		g_Instance->m_FrameIndex = (g_Instance->m_FrameIndex + 1) % GUIIF::GetIO().FrameCount;



	}
	void GUI::Destroy()
	{
		
		g_Instance->DestroyIF();
		g_Instance = nullptr;

	}
	void GUI::ReceiveMessage(const Message& msg)
	{

	}



	// GUI Interface



	float GUIIF::GetTick()
	{
		return GlobalSharedData::GetEngineTimer()->GetTick();;
	}




	void GUIIF::UnAllocateGUIObject(GUIObject* obj)
	{
		obj->m_IsAlive = false;
	}

	void GUIIF::RegisterScreen(Window* win)
	{
		auto& screenPool = GetInstance()->m_ScreenPool;
		if (screenPool.find(win) != screenPool.end()) return;

		screenPool[win] = make_shared<GUIScreen>();
		GUIScreen* screen = screenPool[win].get();


		Transform* transform = win->GetTransform();
		JVector2 size = transform->GetSize();

		JWindowDesc desc;
		desc.width = (uint32_t)size.x;
		desc.height = (uint32_t)size.y;
		desc.name = ws2s(win->GetName());

		if (GUIIF::GetMainWindow() != win)
		{
			desc.parent_hWnd = GlobalSharedData::GetWindowManager()->GetMainWindowHWND();
		}
		screen->win = GlobalSharedData::GetWindowManager()->Create(desc);
		if (screen->win == nullptr)
		{
			GUILOG_ERROR(" %s 윈도우 생성에 실패 하였습니다.", win->GetName().c_str());
			screenPool.erase(win);
			return;
		}

		screen->screen = GraphicsIF::RequestScreen(screen->win->GetHandle(), desc.width, desc.height);
		screen->screen->BindScene(win->m_WindowDrawItem.scene.Get());

		{
			auto& location = transform->GetLocation();
			screen->win->SetPosition((uint32_t)location.x, (uint32_t)location.y);

			screen->prevSize     = size;
			screen->prevLocation = location;
		}
		{// test
			DragAcceptFiles(screen->win->GetHandle(), TRUE);
		}
	}

	void GUIIF::UnRegisterScreen(Window* win)
	{
		auto& screenPool = GetInstance()->m_ScreenPool;
		if (screenPool.find(win) == screenPool.end()) return;
		
		auto screen = screenPool[win];
		screen->screen.Reset();


		GlobalSharedData::GetWindowManager()->Destroy(screen->win);
		screenPool.erase(win);
	}

	void GUIIF::ReserveDestroyElement(Element* element)
	{
		auto& reservedQueue = GetInstance()->m_ReservedElements;
		reservedQueue[GetInstance()->m_FrameIndex].push(element);
	}

	void GUIIF::ReserveDestroyWindow(Window* win)
	{
		if (win == GUIIF::GetMainWindow())
		{
			GetInstance()->m_IsCloseApp = true;
		}
		auto& reservedQueue = GetInstance()->m_ReservedWindows;
		reservedQueue[GetInstance()->m_FrameIndex].push(win);
	}

	JFont* GUIIF::LoadFont(const std::wstring& path)
	{
		return GUIIF::GetInstance()->m_FontManager.Load(path);
	}

	MaterialRef GUIIF::GetGUIMaterialRef(GUI_PreGenerated_Material m)
	{
		switch (m)
		{
		case GUI_PreGenerated_Material_Default: 
			return move(GraphicsIF::GetMaterial(GUI_PREGENERATED_MATERIAL_DEFAULT));
		case GUI_PreGenerated_Material_Image:
			return move(GraphicsIF::GetMaterial(GUI_PREGENERATED_MATERIAL_IMAGE));
		case GUI_PreGenerated_Material_Text:
			return move(GraphicsIF::GetMaterial(GUI_PREGENERATED_MATERIAL_TEXT));
		}
		return move(MaterialRef());
	}

	Window* GUIIF::GetMainWindow()
	{
		return GetInstance()->m_MainWindow;
	}
	void GUIIF::SetCursorPos(const JVector2& pos) {
		::SetCursorPos((int)pos.x, (int)pos.y);
	}
	void GUIIF::CursorLock(const JRect& area)
	{
		RECT r;
		r.left = area.left;
		r.top = area.top;
		r.right = area.right;
		r.bottom = area.bottom;
		ClipCursor(&r);

	}
	void GUIIF::CursorUnLock()
	{
		ClipCursor(nullptr);
	}
	void GUIIF::CursorVisible(bool is_visible)
	{
		static bool is = true;
		if (is == is_visible)
		{
			return;
		}
		is = is_visible;
		ShowCursor(is_visible);
	}

	GUIIO& GUIIF::GetIO()
	{
		return GetInstance()->m_IO;
		// TODO: 여기에 return 문을 삽입합니다.
	}

	GUIStyle& GUIIF::GetStyle()
	{
		return GetInstance()->m_Style;
		// TODO: 여기에 return 문을 삽입합니다.
	}





	GUIIF* GUIIF::GetInstance()
	{
		return g_Instance.get();
	}

	void GUIIF::ProcessGarbageCollection(float tick)
	{

		while (!m_ReservedElements[m_FrameIndex].empty())
		{
			auto element = m_ReservedElements[m_FrameIndex].front(); m_ReservedElements[m_FrameIndex].pop();
			if (!element->IsAlive()) continue;
			element->GUIDestroy();
			element->GetTransform()->SetParent(nullptr);
		}
		while (!m_ReservedWindows[m_FrameIndex].empty())
		{
			auto win = m_ReservedWindows[m_FrameIndex].front(); m_ReservedWindows[m_FrameIndex].pop();
			if (m_SelectedWindow == win) m_SelectedWindow = nullptr;
			if (!win->IsAlive()) continue;
			win->GUIDestroy();
			win->GetTransform()->SetParent(nullptr);
		}



		if (m_ObjectPool.empty()) return;
		static float gui_tick = 0.0f;
		gui_tick += tick;

		if (gui_tick > m_IO.GarbageCollectionTick)
		{
			gui_tick = 0.0f;
			static uint64_t countOffset = 0;

			uint64_t obj_count = m_ObjectPool.size();

			for (uint64_t i = 0; i < m_IO.GarbageProcessCountPerTick;)
			{
				int index = (i + countOffset) % obj_count;


				if (!m_ObjectPool[index]->m_IsAlive)
				{
					std::swap(m_ObjectPool[index], m_ObjectPool[obj_count - 1]);
					m_ObjectPool.pop_back();
					--obj_count;
				}
				else
				{
					++countOffset;
					++i;
				}
			}
		}
	}
	void GUIIF::UpdateScreen()
	{
		for (auto& screen_pair : m_ScreenPool)
		{
			auto  win = screen_pair.first;
			GUIScreen* screen = screen_pair.second.get();

			auto  size  = win->GetTransform()->GetSize();
			auto& location = win->GetTransform()->GetLocation();

			if (screen->prevSize != size)
			{
				screen->screen->RequestResize({ (int)size.x, (int)size.y });
				screen->screen->BindScene(win->m_WindowDrawItem.scene.Get());
				screen->win->SetClientSize((uint32_t)size.x, (uint32_t)size.y);
				screen->prevSize = size;
			}
			if (screen->prevLocation != location)
			{
				JVector2Int p = { 0,0 };

				p.x += (uint32_t)location.x;
				p.y += (uint32_t)location.y;

				screen->win->SetPosition(p.x, p.y);
				screen->prevLocation = location;
			}
		}
	}
	void GUIIF::UpdateWindow()
	{

		for (auto& screen_pair : m_ScreenPool)
		{
			auto win = screen_pair.first;

			if (!win->m_IsExecuteStart)
			{
				win->m_IsExecuteStart = true;
				win->GUIStart();
			}
			win->GUITick();
		}
	}

	void GUIIF::UpdateEvent()
	{
		// Focus Window 이벤트 업데이트
		HWND hWnd = GetFocus();
		if (hWnd != 0)
		{
			Window* focusWin = nullptr;
	
			auto iter = std::find_if(m_ScreenPool.begin(), m_ScreenPool.end(),
				[&](const pair< Window*, std::shared_ptr<GUIScreen>>& _pair)
			{
				return _pair.second->win->GetHandle() == hWnd;
			});
			if (iter != m_ScreenPool.end())
			{
				UpdateEventFocusedGUIScreen(iter->first, iter->second.get());

			}
		}
		if (m_SelectedWindow)
		{
			auto& selectedScreen = m_ScreenPool[m_SelectedWindow];
			for (int i = 0; i < GUIIF::GUIScreen::MouseButtonCount; ++i)
			{
				if (selectedScreen->mouseDown[i])
				{
					switch (i)
					{
					case 0: UpdateKeyDuration(KeyCode::LeftMouseButton, m_SelectedWindow);  break;
					case 1: UpdateKeyDuration(KeyCode::RightMouseButton, m_SelectedWindow); break;
					case 2: UpdateKeyDuration(KeyCode::MouseWheelButton, m_SelectedWindow); break;
					}
					break;
				}

			}



			int bt = -1;
			if (InputIF::GetKeyUp(KeyCode::LeftMouseButton)) bt = 0;
			else if (InputIF::GetKeyUp(KeyCode::RightMouseButton)) bt = 1;
			else if (InputIF::GetKeyUp(KeyCode::MouseWheelButton)) bt = 2;

			if (bt != -1)
			{
				GUIIF::GUIScreen* selectedScreen = m_ScreenPool[m_SelectedWindow].get();;
				selectedScreen->mouseDown[bt] = false;

				switch (bt)
				{
				case 0: GUIIF::GetInstance()->ResetKeyDuration(KeyCode::LeftMouseButton, m_SelectedWindow); break;
				case 1: GUIIF::GetInstance()->ResetKeyDuration(KeyCode::RightMouseButton, m_SelectedWindow); break;
				case 2: GUIIF::GetInstance()->ResetKeyDuration(KeyCode::MouseWheelButton, m_SelectedWindow); break;
				}

				if (!selectedScreen->mouseDown[0] && !selectedScreen->mouseDown[1] && !selectedScreen->mouseDown[2])
				{
					m_SelectedWindow = nullptr;
				}
			}


		}
		if (m_FocusWindow)
		{
			m_FocusWindow->ReceiveFocusEvent(2);
		}

	}

	void GUIIF::UpdateEventFocusedGUIScreen(Window* focusWin, GUIScreen* focusScreen)
	{
		auto& io = GUIIF::GetIO();
		float tick = GetTick();

		JRect& rect = focusWin->GetTransform()->GetRect();
		for (int i = 0; i < 256; ++i)
		{
			if (i == KeyCode::LeftMouseButton || i == KeyCode::RightMouseButton ||
				i == KeyCode::MouseWheelButton)
				continue;
			if (InputIF::GetKeyDown(i) || InputIF::GetKey(i))
			{
				UpdateKeyDuration(i, focusWin);
			}
			else if (InputIF::GetKeyUp(i))
			{
				ResetKeyDuration(i, focusWin);
			}
		}


		if (io.PrevMousePos != io.MousePos)
		{
			if (rect.Contain(io.MousePos))
			{
				focusScreen->mouseInScreen = true;
				if (focusScreen->prevMouseInScreen && focusScreen->mouseInScreen)
				{
					focusWin->ReceiveMouseMoveEvent(io.MousePos - io.PrevMousePos);
				}
				else
				{
					focusWin->OnMouseEnter();
				}
			}
			else
			{
				focusScreen->mouseInScreen = false;
				if (!focusScreen->mouseInScreen && focusScreen->prevMouseInScreen)
				{
					focusWin->OnMouseExit();
				}
			}

			focusScreen->prevMouseInScreen = focusScreen->mouseInScreen;
			io.PrevMousePos = io.MousePos;
		}
	}

	void GUIIF::UpdateKeyDuration(KeyCode code, Window* win)
	{

		auto& io   = GUIIF::GetIO();
		float tick = GUIIF::GetTick();
		
	
		if (io.KeyDownDuration[code.ToInt()] == 0.0f)
		{
			win->ReceiveKeyDownEvent(code);
		}
		else
		{
			if (io.KeyDownDuration[code.ToInt()] >= io.KeyRepeatDelay)
			{
				io.KeyDownDuration[code.ToInt()] = 0.0f;
				win->ReceiveKeyEvent(code);
			}
		}

		io.KeyDownDuration[code.ToInt()] += tick;
	}

	void GUIIF::ResetKeyDuration(KeyCode code, Window* win)
	{
		auto& io = GUIIF::GetIO();
		io.KeyDownDuration[code.ToInt()] = 0.0f;
		win->ReceiveKeyUpEvent(code);
	}

	void GUIIF::DestroyIF()
	{
		int frameCount = GUIIF::GetIO().FrameCount;
		for (int i = 0; i < frameCount; ++i)
		{
			while (!m_ReservedElements[i].empty())
			{
				auto element = m_ReservedElements[i].front(); m_ReservedElements[i].pop();
				if (!element->IsAlive()) continue;
				element->GUIDestroy();
				element->GetTransform()->SetParent(nullptr);
			}
			while (!m_ReservedWindows[i].empty())
			{
				auto win = m_ReservedWindows[i].front(); m_ReservedWindows[i].pop();
				if (m_SelectedWindow == win) m_SelectedWindow = nullptr;
				if (!win->IsAlive()) continue;
				win->GUIDestroy();
				win->GetTransform()->SetParent(nullptr);
			}
		}
		m_ScreenPool.clear();
		m_ObjectPool.clear();
		m_EditPlugin.reset();
		m_EditPlugin = nullptr;
	}




}