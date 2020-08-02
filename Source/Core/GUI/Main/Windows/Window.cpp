#include "pch.h"
#include "Window.h"
#include "Components/DrawComponent.h"
#include "Components/LayerComponent.h"
//
#include "Elements/Border.h"
#include "Elements/ResizeGrip.h"
#include "Elements/TitleBar.h"
#include "Elements/Button.h"
using namespace std;




namespace GUI
{
	void Window::SetName(const std::wstring& name)
	{
		GUIObject::SetName(name);
		if (m_Collection.titleBar)
		{
			m_Collection.titleBar->SetTitleName(name);
		}
	}
	void Window::GUIAwake()
	{
		CreateRootElement();
		SceneUpdate();
		Awake();
	}

	void Window::GUIStart()
	{
		Start();
	}

	void Window::GUITick()
	{
		SettingUpdate();
		Tick();
		if (!m_ElementRoot->m_IsExecuteStart)
		{
			m_ElementRoot->m_IsExecuteStart = true;
			m_ElementRoot->GUIStart();
		}

		m_ElementRoot->GUITick();

		uint32_t childCnt = m_WindowRoot->GetTransform()->GetChildCount();
		for (uint32_t i = 0; i < childCnt; ++i)
		{
			auto child = m_WindowRoot->GetTransform()->GetChild(i)->GetOwner()->GetOwnerWindow();
			
			if (!child->m_IsExecuteStart)
			{
				child->m_IsExecuteStart = true;
				child->GUIStart();
			}
			child->GUITick();
		}

		RenderUpdate();
	}

	void Window::GUIDestroy()
	{
		Destroy();

		if (GetTransform()->GetParent())
		{
			auto focusWin = GetTransform()->GetParent()->GetOwnerWindow()->m_FocusWindow;
			if (focusWin == this)
			{
				GetTransform()->GetParent()->GetOwnerWindow()->m_FocusWindow = nullptr;
			}
		}
		else
		{
			GUIIF::UnRegisterScreen(this);
		}



		uint32_t childCnt = m_WindowRoot->GetTransform()->GetChildCount();
		for (int i = 0; i < childCnt; ++i)
		{
			auto child = m_WindowRoot->GetTransform()->GetChild(i);
			child->GetOwner()->GetOwnerWindow()->GUIDestroy();
		}
		DestroyElement(m_WindowRoot);
		DestroyElement(m_WindowImage);
		DestroyElement(m_ElementRoot);
		m_FocusWindow  = nullptr;
		m_FocusElement = nullptr;




		GUIIF::UnAllocateGUIObject(this);
	}

	void Window::DestroyElement(Element* element)
	{
		GUIIF::ReserveDestroyElement(element);
	}

	void Window::DestroyWindow(Window* win)
	{
		GUIIF::ReserveDestroyWindow(win);
	}

	void Window::PushDrawItem(const GUIDrawItem& item)
	{
		m_WindowDrawItem.drawItems[item.priority].push_back(item.object);
	}

	void Window::RegisterInteractionItem(const GUIInteractionItem& item)
	{
		if (m_InteractionItems[item.ownerElement] == nullptr)
		{
			m_InteractionItems[item.ownerElement] = make_shared<GUIInteractionItem>();
		}
		bool register_enter = m_InteractionItems[item.ownerElement]->prevEnter;
		*m_InteractionItems[item.ownerElement] = item;
		m_InteractionItems[item.ownerElement]->prevEnter = register_enter;
	}

	void Window::UnRegisterInteractionItem(Element* element)
	{
		auto iter = m_InteractionItems.find(element);
		if (m_InteractionItems.end() == iter) return;
		m_InteractionItems.erase(iter);
	}

	void Window::SetFocusWindow(Window* win)
	{
		if (m_FocusWindow != win)
		{
			if (m_FocusWindow) m_FocusWindow->ReceiveFocusEvent(1);
			if (win) win->ReceiveFocusEvent(0);
		}
		m_FocusWindow = win;
	}

	void Window::SetFocusElement(Element* element)
	{
		if (m_FocusElement != element)
		{
			if (m_FocusElement) m_FocusElement->OnFocusExit();
			if (element) element->OnFocusEnter();
		}
		m_FocusElement = element;
	}

	Transform* Window::GetTransform() const
	{
		return m_WindowRoot->GetTransform();
	}

	JVector2 Window::GetMousePos() const
	{
		JVector2 cursorPos = GUIIF::GetCursorPos();
		JVector2 winPos = GetTransform()->GetLocation();

		return cursorPos - winPos;
	}

	void Window::SetBackGroundColor(const JColor& color)
	{
		m_ElementRoot->FindComponent<ImageComponent>()->SetColor(color);
	}

	void Window::SetFlags(WindowFlags flag)
	{
		m_Flags = flag;
		FlagUpdate();
	}


	void Window::ReceiveKeyDownEvent(KeyCode code)
	{
		if (code == KeyCode::LeftMouseButton || code == KeyCode::RightMouseButton || code == KeyCode::MouseWheelButton)
		{
			
			TrackingFocusWindow();
			if (GetFocusWindow())
			{
				GUILOG_INFO("%s : OnMouseButtonDown", GetFocusWindow()->GetName().c_str());
				GetFocusWindow()->ReceiveKeyDownEvent(code); return;
			}
			TrackingFocusElement();
			if (GetFocusElement())
			{
				GetFocusElement()->OnMouseButtonDown(code);
				return;
			}
			else
			{
				if (code == KeyCode::LeftMouseButton)
				{
					m_IsMouseDown = true;
					if (!(m_Flags & WindowFlag_NoMove))
					{
						m_WindowMove = true;
						m_DeltaBetWC = GUIIF::GetCursorPos() - GetTransform()->GetLocation();
					}
				}

				GUILOG_INFO("%s : OnMouseButtonDown", GetName().c_str());
				OnMouseButtonDown(code);
			}
		}
		else
		{
			if (GetFocusWindow())
			{
				GUILOG_INFO("%s : OnKeyDown", GetFocusWindow()->GetName().c_str());
				GetFocusWindow()->ReceiveKeyDownEvent(code);
			}
			if (GetFocusElement())
			{
				GetFocusElement()->OnKeyDown(code);
			}
			else
			{
				GUILOG_INFO("%s : OnKeyDown", GetName().c_str());
				OnKeyDown(code);
			}
		}
	}

	void Window::ReceiveKeyUpEvent(KeyCode code)
	{
		if (code == KeyCode::LeftMouseButton || code == KeyCode::RightMouseButton || code == KeyCode::MouseWheelButton)
		{
			if (GetFocusWindow())
			{
				GetFocusWindow()->ReceiveKeyUpEvent(code);
			}
			else if (GetFocusElement())
			{
				auto& rect = GetFocusElement()->GetTransform()->GetRect();
				GUILOG_INFO("%s : OnMouseButtonUp", GetFocusElement()->GetName().c_str());
				GetFocusElement()->OnMouseButtonUp(code);
				if (rect.Contain(GetMousePos()))
				{
					GUILOG_INFO("%s : OnMouseClick", GetFocusElement()->GetName().c_str());
					GetFocusElement()->OnMouseClick(code);
					if (m_IsMouseClick)
					{
						auto& io   = GUIIF::GetIO();
						float dist = JVector2::Length(m_MouseClickedPos - GUIIF::GetCursorPos());
						if (m_MouseClickDuration <= io.MouseDoubleClickTime &&
							dist <= io.MouseDoubleClickMaxDist &&
							m_MouseClickedButton == code)
						{
							GUILOG_INFO("%s : OnMouseDoubleClick", GetFocusElement()->GetName().c_str());
							GetFocusElement()->OnMouseDoubleClick(code);
						}
						m_MouseClickedButton = KeyCode(-1);
						m_IsMouseClick = false;
						m_MouseClickDuration = 0.0f;
						JVector2 m_MouseClickedPos = { 0,0 };
					}
					else
					{
						m_MouseClickedButton = code;
						m_MouseClickedPos = GUIIF::GetCursorPos();
						m_IsMouseClick = true;
					}
				}
			}
			else
			{
				auto& rect = GetTransform()->GetRect();
				GUILOG_INFO("%s : OnMouseButtonUp", GetName().c_str());
				OnMouseButtonUp(code);
				if (rect.Contain(GUIIF::GetCursorPos()))
				{
					GUILOG_INFO("%s : OnMouseButtonClick", GetName().c_str());
					OnMouseClick(code);
					if (m_IsMouseClick)
					{
						auto& io = GUIIF::GetIO();
						float dist = JVector2::Length(m_MouseClickedPos - GUIIF::GetCursorPos());
						if (m_MouseClickDuration <= io.MouseDoubleClickTime &&
							dist <= io.MouseDoubleClickMaxDist &&
							m_MouseClickedButton == code)
						{
							GUILOG_INFO("%s : OnMouseButtonDoubleClick", GetName().c_str());
							OnMouseDoubleClick(code);
						}
						m_MouseClickedButton = KeyCode(-1);
						m_IsMouseClick = false;
						m_MouseClickDuration = 0.0f;
						JVector2 m_MouseClickedPos = { 0,0 };
					}
					else
					{
						m_MouseClickedButton = code;
						m_MouseClickedPos = GUIIF::GetCursorPos();
						m_IsMouseClick = true;
					}
				}

				if (m_IsMouseDown && code == KeyCode::LeftMouseButton)
				{
					m_IsMouseDown = false;
					m_WindowMove = false;
					m_DeltaBetWC = { 0.0f,0.0f };
				}
			}
		}
		else
		{
			if (GetFocusWindow())
			{
				GetFocusWindow()->ReceiveKeyUpEvent(code);
			}
			if (GetFocusElement())
			{
				GUILOG_INFO("%s : OnKeyUp", GetFocusElement()->GetName().c_str());
				GetFocusElement()->OnKeyUp(code);
				// Element KeyDown;
			}
			else
			{
				GUILOG_INFO("%s : OnKeyUp", GetName().c_str());
				OnKeyUp(code);
			}

	
		}
	}

	void Window::ReceiveKeyEvent(KeyCode code)
	{

		if (code == KeyCode::LeftMouseButton || code == KeyCode::RightMouseButton || code == KeyCode::MouseWheelButton)
		{
			if (GetFocusWindow())
			{
				GetFocusWindow()->ReceiveKeyEvent(code);
			}
			// 우선순위
			else if (GetFocusElement())
			{
				GetFocusElement()->OnMouseButton(code);
			}
			else
			{
				OnMouseButton(code);
			}
		}
		else
		{
			if (GetFocusWindow())
			{
				GetFocusWindow()->ReceiveKeyEvent(code);
			}
			if (GetFocusElement())
			{
				GetFocusElement()->OnKey(code);
			}
			else OnKey(code);
		}
	}

	void Window::ReceiveMouseMoveEvent(const JVector2& delta)
	{
		Window*  moveWin = nullptr;
		uint32_t childCnt = m_WindowRoot->GetTransform()->GetChildCount();
		for (int i = 0; i < childCnt; ++i)
		{
			auto child = m_WindowRoot->GetTransform()->GetChild(i)->GetOwnerWindow();

			if (child->GetTransform()->GetRect().Contain(GUIIF::GetCursorPos()))
			{
				moveWin = child;
				if (!moveWin->m_IsPrevMouseEnter)
				{
					moveWin->m_IsPrevMouseEnter = true;
					moveWin->OnMouseEnter();
				}
				break;
			}
			else if (moveWin && moveWin->m_IsPrevMouseEnter)
			{
				moveWin->m_IsPrevMouseEnter = false;
				moveWin->OnMouseExit();
			}
		}

		if (moveWin)
		{
			moveWin->ReceiveMouseMoveEvent(delta); return;
		}


		Element* move_Element = nullptr;
		map<uint64_t, vector<GUIInteractionItem*>> interactElements;
		for (auto& iter : m_InteractionItems) {
			
			interactElements[iter.second->draw_priority].push_back(iter.second.get());
		}

		for (auto& item_pair : interactElements)
		{
			for (auto& item : item_pair.second)
			{
				
				if (item->interactionFunc() && moveWin == nullptr)
				{
					move_Element = item->ownerElement;
					if (!item->prevEnter)
					{
						move_Element->OnMouseEnter();
						item->prevEnter = true;
						
					}
					
					break;
				}
				else if (item->prevEnter)
				{
					item->prevEnter = false;
					item->ownerElement->OnMouseExit();
				}
				
			}
		}


		if (move_Element)
		{
			move_Element->OnMouseMove(); return;
		}


		OnMouseMove();

	}

	void Window::ReceiveFocusEvent(int exit)
	{
		switch (exit)
		{
		case 0: OnFocusEnter();
		
			GUILOG_INFO("%s : FocusEnter", GetName().c_str());
			break;
		case 1:
			m_WindowMove = false;  m_DeltaBetWC = { 0.0f,0.0f };
			if (m_FocusWindow) m_FocusWindow->ReceiveFocusEvent(1);
			m_FocusWindow = nullptr;
			GUILOG_INFO("%s : FocusExit", GetName().c_str());
			OnFocusExit();
			break;
		case 2: OnFocus();
			break;
		}
	}



	void Window::CreateRootElement()
	{
		auto& style = GUIIF::GetStyle();
		m_WindowRoot = GUIIF::AllocateGUIObject<Element>();
		m_WindowRoot->SetName(TT("WindowRoot"));
		m_WindowRoot->m_OwnerWindow = this;
		m_WindowRoot->GUIAwake();
		m_WindowRoot->GetTransform()->SetSize(style.WindowMinSize);


		m_WindowImage = GUIIF::AllocateGUIObject<Element>();
		m_WindowImage->SetName(TT("WindowImage"));
		m_WindowImage->m_OwnerWindow = nullptr;
		m_WindowImage->GUIAwake();
		m_WindowImage->GetTransform()->SetSize(style.WindowMinSize);
		auto com = m_WindowImage->CreateComponent<ImageComponent>(TT("ImageComponent"));
		com->SetAutoPriority(false);
		com->SetInteractionActive(false);

		m_ElementRoot = GUIIF::AllocateGUIObject<Element>();
		m_ElementRoot->SetName(TT("ElementRoot"));
		m_ElementRoot->m_OwnerWindow = this;
		m_ElementRoot->GUIAwake();
		m_ElementRoot->GetTransform()->SetSize(style.WindowMinSize);
		com = m_ElementRoot->CreateComponent<ImageComponent>(TT("ImageComponent"));
		com->SetColor(style.GUIColors[GUIStyle_WindowBackGround]);
		com->SetDrawPriority(GUIDraw_Priority_WindowBackGround);
		com->SetAutoPriority(false);
		com->SetInteractionActive(false);
	}
	void Window::SceneUpdate()
	{
		JVector2 size = GetTransform()->GetSize();
		size.x = (float)(uint32_t)size.x;
		size.y = (float)(uint32_t)size.y;
		if (!m_WindowDrawItem.scene.IsValid())
		{
			m_WindowDrawItem.scene = GraphicsIF::RequestScene();
			m_WindowDrawItem.camera.SetLens(JG_PI * 0.25f, size.x, size.y, 100000.0f, 0.1f, true);
			m_WindowDrawItem.camera.SetPosition({ 0.0f,0.0f,-10.0f });
			m_WindowDrawItem.scene->BindCamera(&m_WindowDrawItem.camera);
		}
		else
		{
			m_WindowDrawItem.camera.SetLens(JG_PI * 0.25f, size.x, size.y, 100000.0f, 0.1f, true);
		}
		m_ElementRoot->GetTransform()->SetSize(size.x, size.y);
		m_WindowImage->GetTransform()->SetSize(size.x, size.y);
		m_WindowImage->FindComponent<ImageComponent>()->SetImage(m_WindowDrawItem.scene->GetSceneTexture());
	}
	void Window::RenderUpdate()
	{
		{
			std::vector<GE::SceneObject*> objs;
			for (auto& drawitem : m_WindowDrawItem.drawItems)
			{
				objs.insert(objs.end(), drawitem.second.begin(), drawitem.second.end());
			}

			if (m_WindowDrawItem.scene.IsValid())
			{
				m_WindowDrawItem.scene->PushObjects(objs);
			}
			m_ElementDrawPriorityOffset = GUIDraw_Priority_Element;
			GraphicsIF::DrawCall(m_WindowDrawItem.scene);
			m_WindowDrawItem.drawItems.clear();
		}
		if (GetTransform()->GetSize() != m_WindowDrawItem.prevSize)
		{
			auto& style = GUIIF::GetStyle();
			
			JVector2 size = GetTransform()->GetSize();
			uint32_t width  = max<uint32_t>((uint32_t)style.WindowMinSize.x, (uint32_t)size.x);
			uint32_t height = max<uint32_t>((uint32_t)style.WindowMinSize.y, (uint32_t)size.y);
			GetTransform()->SetSize((float)width, (float)height);
			SceneUpdate();
			OnResize(width, height);
			m_WindowDrawItem.prevSize = GetTransform()->GetSize();
		}

		if (GetTransform()->GetParent() == nullptr)
		{
			m_WindowImage->m_OwnerWindow = nullptr;
			m_WindowImage->GetTransform()->SetParent(nullptr);
			if (GetTransform()->GetPivot() != m_WindowDrawItem.prevPivot)
			{
				GetTransform()->SetPivot(0, 0);
			}
		}
		else
		{
			if (GetTransform()->IsDirty())
			{
				m_WindowImage->GetTransform()->SetPivot(GetTransform()->GetPivot());
				m_WindowImage->GetTransform()->SetAnchor(GetTransform()->GetAnchor());
				m_WindowImage->GetTransform()->SetLocalLocation(GetTransform()->GetLocalLocation());
				GetTransform()->ClearDirty(false);
			}

			m_WindowImage->m_OwnerWindow = GetTransform()->GetParent()->GetOwner()->GetOwnerWindow();
			auto parentWin = GetTransform()->GetParent()->GetOwner()->GetOwnerWindow();
			auto image = m_WindowImage->FindComponent<ImageComponent>();
			image->SetDrawPriority(m_DrawPriority);
			m_WindowImage->GetTransform()->SetParent(parentWin->m_ElementRoot->GetTransform());
		}
		
	}
	void Window::FlagUpdate()
	{
		auto& style = GUIIF::GetStyle();
		if (m_Flags & WindowFlag_RootWindow)
		{
			GUIIF::RegisterScreen(this);
		}
		else
		{
			GUIIF::UnRegisterScreen(this);
		}

		JRect work_area_off;
		if (m_Flags & WindowFlag_Border)
		{
			if (m_Collection.border == nullptr)
			{
				m_Collection.border = CreateElement<Border>(TT("Border"), ElementFlag_None);
				m_Collection.border->SetThickness(style.WindowBorderThick);
				m_Collection.border->SetDrawPriority(GUIDraw_Priority_WindowBorder);
				m_Collection.border->SetAutoPriority(false);
				m_Collection.border->SetColor(style.GUIColors[GUIStyle_WindowBorder]);
				auto borderTransform = m_Collection.border->GetTransform();
				borderTransform->SetSize(GetTransform()->GetSize());
				borderTransform->SetAnchor(AnchorPreset::Full_Stretch);

				work_area_off.left   += style.WindowBorderThick;
				work_area_off.right  -= style.WindowBorderThick;
				work_area_off.top    += style.WindowBorderThick;
				work_area_off.bottom -= style.WindowBorderThick;

			}
		}
		else
		{
			if (m_Collection.border)
			{
				DestroyElement(m_Collection.border);
				m_Collection.border = nullptr;
			}
		}
		if (m_Flags & WindowFlag_Resize)
		{
			if (m_Collection.resizeGrip == nullptr)
			{
				m_Collection.resizeGrip = CreateElement<ResizeGrip>(TT("ResizeGrip"), ElementFlag_None);
				m_Collection.resizeGrip->SetHightlightColor(style.GUIColors[GUIStyle_WindowResizeGripHightlight]);
				m_Collection.resizeGrip->SetNormalColor(style.GUIColors[GUIStyle_WindowResizeGrip]);
				m_Collection.resizeGrip->BindTransform(GetTransform());
				m_Collection.resizeGrip->SetMinSize(style.WindowMinSize);
				auto drawCom = m_Collection.resizeGrip->FindComponent<ShapeComponent>();
				if (drawCom)
				{
					drawCom->SetAutoPriority(false);
					drawCom->SetDrawPriority(GUIDraw_Priority_WindowResizeGrip);
				}
				auto transform = m_Collection.resizeGrip->GetTransform();
				transform->SetSize(style.WindowResizeGripSize);
				transform->SetPivot({ 1.0f,1.0f });
				transform->SetAnchor(AnchorPreset::Right_Bottom);
				
			}
		}
		else
		{
			if (m_Collection.resizeGrip)
			{
				DestroyElement(m_Collection.resizeGrip);
				m_Collection.resizeGrip = nullptr;
			}
		}

		if (m_Flags & WindowFlag_TitleBar)
		{
			if (m_Collection.titleBar == nullptr)
			{
				m_Collection.titleBar = CreateElement<TitleBar>(TT("TitleBar"), ElementFlag_None);
				m_Collection.titleBar->SetTitleName(GetName());
				auto transform = m_Collection.titleBar->GetTransform();
				transform->SetPivot(0, 0);
				transform->SetAnchor(AnchorPreset::Top_HStretch);

			}


			if (m_Flags & WindowFlag_HideBt)
			{
				if (m_Collection.hideBt == nullptr)
				{
					m_Collection.hideBt = m_Collection.titleBar->CreateElement<Button>(TT("HideBt"));
					auto transform = m_Collection.hideBt->GetTransform();
					transform->SetSize(style.TitleBarHeight, style.TitleBarHeight);
					transform->SetPivot(1.0f, 0.0f);
					transform->SetAnchor(AnchorPreset::Right_Top);
					m_Collection.hideBt->SetAllImage(style.GUIImages[GUIStyle_Resource_HideBt]);
				}
			}
			else
			{
				if (m_Collection.maxBt)
				{
					m_Collection.titleBar->DestroyElement(m_Collection.maxBt);
					m_Collection.maxBt = nullptr;
				}
				
			}
			if (m_Flags & WindowFlag_MaxBt)
			{
				if (m_Collection.maxBt == nullptr)
				{
					m_Collection.maxBt = m_Collection.titleBar->CreateElement<Button>(TT("MaxBt"));
					auto transform = m_Collection.maxBt->GetTransform();
					transform->SetSize(style.TitleBarHeight, style.TitleBarHeight);
					transform->SetPivot(1.0f, 0.0f);
					transform->SetAnchor(AnchorPreset::Right_Top);
					m_Collection.maxBt->SetAllImage(style.GUIImages[GUIStyle_Resource_MaxBt]);
				}
			}
			else
			{
				if (m_Collection.maxBt)
				{
					m_Collection.titleBar->DestroyElement(m_Collection.maxBt);
					m_Collection.maxBt = nullptr;
				}
			}
			if (m_Flags & WindowFlag_CloseBt)
			{
				if (m_Collection.closeBt == nullptr)
				{
					m_Collection.closeBt = m_Collection.titleBar->CreateElement<Button>(TT("CloseBt"));
					auto transform = m_Collection.closeBt->GetTransform();
					transform->SetSize(style.TitleBarHeight, style.TitleBarHeight);
					transform->SetPivot(1.0f, 0.0f);
					transform->SetAnchor(AnchorPreset::Right_Top);
					m_Collection.closeBt->SetAllImage(style.GUIImages[GUIStyle_Resource_CloseBt]);
					m_Collection.closeBt->BindOnClick([&]()
					{
						DestroyWindow(this);
					});
				}
			}
			else
			{
				if (m_Collection.closeBt)
				{
					m_Collection.titleBar->DestroyElement(m_Collection.closeBt);
					m_Collection.closeBt = nullptr;
				}
			}
		}
		else
		{
			if (m_Collection.titleBar)
			{

				DestroyElement(m_Collection.titleBar);
				m_Collection.titleBar = nullptr;

			}
		}


		if (m_Collection.resizeGrip) m_Collection.resizeGrip->GetTransform()->SetLocalLocation(work_area_off.right, work_area_off.bottom);
		if (m_Collection.titleBar)
		{
			m_Collection.titleBar->GetTransform()->SetSize(GetTransform()->GetSize().x - work_area_off.right, style.TitleBarHeight);
			m_Collection.titleBar->GetTransform()->SetLocalLocation(0.0f, work_area_off.top);
		}
		float btOffset = work_area_off.right;
		if (m_Collection.closeBt)
		{
			m_Collection.closeBt->GetTransform()->SetLocalLocation(btOffset, 0.0f);
			btOffset -= style.TitleBarHeight;
		}
		if (m_Collection.maxBt)
		{
			m_Collection.maxBt->GetTransform()->SetLocalLocation(btOffset, 0.0f);
			btOffset -= style.TitleBarHeight;
		}
		if (m_Collection.hideBt)
		{
			m_Collection.hideBt->GetTransform()->SetLocalLocation(btOffset, 0.0f);
		}

	

	}
	void Window::SettingUpdate()
	{
		auto tick = GUIIF::GetTick();
		auto& io = GUIIF::GetIO();
		auto& mousePos = GUIIF::GetCursorPos();
		float dist = JVector2::Length(m_MouseClickedPos - mousePos);



		if (GUIIF::GetMainWindow() == this && GetTransform()->GetParent())
		{
			GetTransform()->SetParent(nullptr);
		}

		if (GetTransform()->GetParent() == nullptr)
		{
			GUIIF::RegisterScreen(this);
			m_DrawPriority = GUIDraw_Priority_BottomMostWindow;
		}
		else GUIIF::UnRegisterScreen(this);
		



		//
		if (m_WindowMove && !(m_Flags & WindowFlag_NoMove))
		{
			JVector2 winPos = GUIIF::GetCursorPos();
			winPos -= m_DeltaBetWC;
			GetTransform()->SetLocation(winPos);
		}




		if (m_IsMouseClick)
		{
			m_MouseClickDuration += tick;

			// 지정된 범위를 벗어나면 더블 클릭 취소
			if (m_MouseClickDuration > io.MouseDoubleClickTime ||
				dist > io.MouseDoubleClickMaxDist)
			{
				m_IsMouseClick = false;
				m_MouseClickDuration = 0.0f;
				JVector2 m_MouseClickedPos = { 0,0 };
			}
		}

		if (m_FocusElement)
		{
			m_FocusElement->OnFocus();
		}
		if (m_FocusWindow)
		{
			m_FocusWindow->ReceiveFocusEvent(2);
		}

	}
	void Window::FocusWindowUpdate(Window* focusedWin)
	{
		uint32_t childCnt = GetTransform()->GetChildCount();
		if (childCnt == 0) return;

		uint64_t priorityOffset = GUIDraw_Priority_Window;
		uint32_t findIndex = -1;
		for (uint32_t i = 0; i < childCnt; ++i)
		{
			auto child = GetTransform()->GetChild(i)->GetOwnerWindow();
			if (child == focusedWin) {
				child->m_DrawPriority = GUIDraw_Priority_Window + (childCnt - 1);
			}
			else 	child->m_DrawPriority = priorityOffset++;
		
		}
		GetTransform()->SortChild([this](Transform* t1, Transform* t2)->bool
		{
			return t1->GetOwnerWindow()->GetDrawPriority() < t2->GetOwnerWindow()->GetDrawPriority();
		});
	}
	void Window::TrackingFocusWindow()
	{
		JVector2 cursorPos = GUIIF::GetCursorPos();
		Window* prevFocusWindow = m_FocusWindow;
		m_FocusWindow = nullptr;
		uint32_t childCnt = m_WindowRoot->GetTransform()->GetChildCount();
		for (int i = 0; i < childCnt; ++i)
		{
			auto child = m_WindowRoot->GetTransform()->GetChild(i)->GetOwnerWindow();
			if (child->GetTransform()->GetRect().Contain(cursorPos))
			{
				m_FocusWindow = child;
			}
		}
		FocusWindowUpdate(m_FocusWindow);
		if (prevFocusWindow != m_FocusWindow)
		{
			if (prevFocusWindow) prevFocusWindow->ReceiveFocusEvent(1);
			if (m_FocusWindow) m_FocusWindow->ReceiveFocusEvent(0);
		}
	}
	void Window::TrackingFocusElement()
	{
		JVector2 cursorPos = GUIIF::GetCursorPos();
		Element* prevFocusElement = m_FocusElement;
		m_FocusElement = nullptr;
		
		map<uint64_t, vector<GUIInteractionItem*>> interactElements;
		for (auto& iter : m_InteractionItems) {
			interactElements[iter.second->draw_priority].push_back(iter.second.get());
		}

		for (auto& item_pair : interactElements)
		{
			for (auto& item : item_pair.second)
			{
				if (item->interactionFunc())
				{
					m_FocusElement = item->ownerElement;
					break;
				}
			}
		}
		if (prevFocusElement != m_FocusElement)
		{
			if (prevFocusElement) prevFocusElement->OnFocusExit();
			if (m_FocusElement) m_FocusElement->OnFocusEnter();
			if (prevFocusElement)
			{
				GUILOG_INFO("%s : FocusExit", prevFocusElement->GetName().c_str());
			}
			if (m_FocusElement)
			{
				GUILOG_INFO("%s : FocusEnter" , m_FocusElement->GetName().c_str());
			}

		}



	}
}