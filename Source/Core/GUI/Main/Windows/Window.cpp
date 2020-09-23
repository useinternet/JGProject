#include "pch.h"
#include "Window.h"
#include "Elements/Element.h"
#include "Components/DrawComponent.h"
#include "Components/LayerComponent.h"

#include "Components/Script/Border.h"
#include "Components/Script/Button.h"
#include "Components/Script/ResizeGrip.h"
using namespace std;




namespace JGUI
{
	void Window::SetName(const std::wstring& name)
	{
		GUIObject::SetName(name);
		if (m_Collection.titleBar)
		{
			auto textMesh = m_Collection.titleBar->GetTransform()->GetChild(0)->GetOwner()->FindComponent<TextMeshComponent>();
			if (textMesh)
			{
				textMesh->SetText(name);
			}
		}
	}
	void Window::GUIAwake()
	{
		CreateRootElement();
		Awake();
		SceneUpdate();
	}

	void Window::GUIStart()
	{
		m_ElementRoot->GetTransform()->SetSize(GetTransform()->GetSize());
		Start();
	}

	void Window::GUITick()
	{
		SettingUpdate();
		Tick();
		if (!m_ElementRoot->m_IsExecuteStart && m_ElementRoot->IsActive())
		{
			m_ElementRoot->m_IsExecuteStart = true;
			m_ElementRoot->GUIStart();
		}
		if(m_ElementRoot->IsActive()) m_ElementRoot->GUITick();
		

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


		m_AboveInteractionItems.clear();
		m_UnderInteractionItems.clear();
		while (!m_IteractionStack.empty())
		{
			auto& item = m_IteractionStack.top(); m_IteractionStack.pop();
			if (item->ownerElement->GetFlags() & ElementFlag_AboveOnWindow)
			{
				m_AboveInteractionItems[item->draw_priority].push_back(item);
			}
			else
			{
				m_UnderInteractionItems[item->draw_priority].push_back(item);
			}

		}
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

		m_IteractionStack.push(m_InteractionItems[item.ownerElement].get());
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

	JRect Window::GetClientRect() const
	{
		m_ElementRoot->GetTransform()->SetSize(m_WindowRoot->GetTransform()->GetSize());
		JVector2 size = m_ElementRoot->GetTransform()->GetSize();
		JRect result(0, 0, size.x, size.y);
		result.left    += m_WorkAreaOff.left;
		result.top     += m_WorkAreaOff.top;
		result.right   -= m_WorkAreaOff.right;
		result.bottom  -= m_WorkAreaOff.bottom;
		return result;
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
		//if (m_Maximizing) return;
		if (code == KeyCode::LeftMouseButton || code == KeyCode::RightMouseButton || code == KeyCode::MouseWheelButton)
		{
			

			Element* prevFocusElement = m_FocusElement;
			TrackingFocusElement(false); // nullptr이 되고

			// 1. nullptr 이면 under도 검사해봐야되고

			if (GetFocusElement())
			{
				TrackingItemFlush(prevFocusElement);
				GetFocusElement()->OnMouseButtonDown(code);
				return;
			}
			// Element 가 nullptr 이란 소리고, 포커싱
			TrackingFocusWindow();
			if (GetFocusWindow())
			{
				TrackingItemFlush(prevFocusElement);
				GetFocusWindow()->ReceiveKeyDownEvent(code); return;
			}
			TrackingFocusElement(true);
			if (GetFocusElement())
			{
				TrackingItemFlush(prevFocusElement);
				GetFocusElement()->OnMouseButtonDown(code);
				return;
			}
			else
			{
				TrackingItemFlush(prevFocusElement);
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
			else if (GetFocusElement())
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
			else if (GetFocusElement())
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
			else if (GetFocusElement())
			{
				GetFocusElement()->OnKey(code);
			}
			else OnKey(code);
		}
	}

	void Window::ReceiveMouseMoveEvent(const JVector2& delta)
	{
		//if (m_Maximizing) return;
		Element* above_move_Element = nullptr;
		for (auto& item_pair : m_AboveInteractionItems)
		{
			for (auto& item : item_pair.second)
			{

				if (item->ownerElement->IsActive() && item->interactionFunc())
				{
					above_move_Element = item->ownerElement;
					if (!item->prevEnter)
					{
						above_move_Element->OnMouseEnter();
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


		Window*  moveWin = nullptr;
		uint32_t childCnt = m_WindowRoot->GetTransform()->GetChildCount();
		for (int i = 0; i < childCnt; ++i)
		{
			auto child = m_WindowRoot->GetTransform()->GetChild(i)->GetOwnerWindow();

			if (child->GetTransform()->GetRect().Contain(GUIIF::GetCursorPos()) && above_move_Element == nullptr)
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




		Element* under_move_Element = nullptr;
		for (auto& item_pair : m_UnderInteractionItems)
		{
			for (auto& item : item_pair.second)
			{
				
				if (item->ownerElement->IsActive() && item->interactionFunc() && moveWin == nullptr)
				{
					under_move_Element = item->ownerElement;
					if (!item->prevEnter)
					{
						under_move_Element->OnMouseEnter();
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
		if (above_move_Element)
		{
			above_move_Element->OnMouseMove(); return;
		}
		if (moveWin)
		{
			moveWin->ReceiveMouseMoveEvent(delta); return;
		}
		if (under_move_Element)
		{
			under_move_Element->OnMouseMove(); return;
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

	void Window::ReceiveDropItemEvent(const GUIDropItem& item)
	{
		for (auto& item_pair : m_AboveInteractionItems)
		{
			for (auto& interactionitem : item_pair.second)
			{
				if (interactionitem->ownerElement->IsActive() && interactionitem->interactionFunc())
				{
					interactionitem->ownerElement->OnDropItem(item);
					return;
				}
			}
		}
		uint32_t childCnt = m_WindowRoot->GetTransform()->GetChildCount();
		for (int i = 0; i < childCnt; ++i)
		{
			auto child = m_WindowRoot->GetTransform()->GetChild(i)->GetOwnerWindow();

			if (child->GetTransform()->GetRect().Contain(GUIIF::GetCursorPos()) )
			{
				child->ReceiveDropItemEvent(item);

				return;
			}
		}
		for (auto& item_pair : m_UnderInteractionItems)
		{
			for (auto& interactionitem : item_pair.second)
			{

				if (interactionitem->ownerElement->IsActive() && interactionitem->interactionFunc())
				{
					interactionitem->ownerElement->OnDropItem(item);
					return;
				}
			}
		}

		OnDropItem(item);
	}

	void Window::ReceiveCharEvent(wchar_t s)
	{
		if (GetFocusWindow())
		{
			GetFocusWindow()->ReceiveCharEvent(s);
		}
		if (GetFocusElement())
		{
			GetFocusElement()->OnChar(s);
		}
		else
		{
			OnChar(s);
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
			m_WindowDrawItem.scene = GraphicsIF::RequestScene(GE::RendererMode::GUI);
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
				m_Collection.border = CreateElement<Element>(TT("Border"), ElementFlag_None);

				ElementTemplate::Template_Border(m_Collection.border);
				auto borderCom = m_Collection.border->FindComponent<Border>();
				if (borderCom)
				{
					borderCom->SetThickness(style.WindowBorderThick);
				}
			

				auto shapeCom = m_Collection.border->FindComponent<ShapeComponent>();
				if (shapeCom)
				{
					shapeCom->SetDrawPriority(GUIDraw_Priority_WindowBorder);
					shapeCom->SetAutoPriority(false);
					shapeCom->SetColor(style.GUIColors[GUIStyle_WindowBorder]);

				}

				auto borderTransform = m_Collection.border->GetTransform();
				borderTransform->SetSize(GetTransform()->GetSize());
				borderTransform->SetAnchor(AnchorPreset::Full_Stretch);
			}
			work_area_off.left += style.WindowBorderThick;
			work_area_off.right -= style.WindowBorderThick;
			work_area_off.top += style.WindowBorderThick;
			work_area_off.bottom -= style.WindowBorderThick;
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
				m_Collection.resizeGrip = CreateElement<Element>(TT("ResizeGrip"), ElementFlag_AboveOnWindow);

				ElementTemplate::Template_ResizeGrip(m_Collection.resizeGrip);
				auto resizeGripCom = m_Collection.resizeGrip->FindComponent<ResizeGrip>();
				resizeGripCom->SetHightlightColor(style.GUIColors[GUIStyle_WindowResizeGripHightlight]);
				resizeGripCom->SetNormalColor(style.GUIColors[GUIStyle_WindowResizeGrip]);
				resizeGripCom->BindTransform(GetTransform());
				resizeGripCom->SetMinSize(style.WindowMinSize);

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
				m_Collection.titleBar = CreateElement<Element>(TT("TitleBar"), ElementFlag_None);

				ElementTemplate::Template_TitleBar(m_Collection.titleBar);
				auto textMesh = m_Collection.titleBar->GetTransform()->GetChild(0)->GetOwner()->FindComponent<TextMeshComponent>();
				if (textMesh)
				{
					textMesh->SetText(GetName());
				}
	
				auto transform = m_Collection.titleBar->GetTransform();
				transform->SetPivot(0, 0);
				transform->SetAnchor(AnchorPreset::Top_HStretch);

			}


			if (m_Flags & WindowFlag_HideBt)
			{
				if (m_Collection.hideBt == nullptr)
				{
					m_Collection.hideBt = m_Collection.titleBar->CreateElement<Element>(TT("HideBt"));
					ElementTemplate::Template_Button(m_Collection.hideBt);
					auto transform = m_Collection.hideBt->GetTransform();
					transform->SetSize(style.TitleBarHeight, style.TitleBarHeight);
					transform->SetPivot(1.0f, 0.0f);
					transform->SetAnchor(AnchorPreset::Right_Top);

					auto btCom = m_Collection.hideBt->FindComponent<Button>();
					btCom->SetAllImage(style.GUIImages[GUIStyle_Resource_HideBt]);
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
					m_Collection.maxBt = m_Collection.titleBar->CreateElement<Element>(TT("MaxBt"));
					ElementTemplate::Template_Button(m_Collection.maxBt);
					auto transform = m_Collection.maxBt->GetTransform();
					transform->SetSize(style.TitleBarHeight, style.TitleBarHeight);
					transform->SetPivot(1.0f, 0.0f);
					transform->SetAnchor(AnchorPreset::Right_Top);

					auto btCom = m_Collection.maxBt->FindComponent<Button>();
					btCom->SetAllImage(style.GUIImages[GUIStyle_Resource_MaxBt]);
					btCom->BindOnClick([&]()
					{
						if (GetTransform()->GetParent()) return;
						if (m_IsMaximizeState)
						{
							//m_Maximizing = true;
							m_IsMaximizeState = false;

							GetTransform()->SetPivot(m_RestorePivot);
							float x = m_RestoreWindowRect.left + (m_RestoreWindowRect.Width() * m_RestorePivot.x);
							float y = m_RestoreWindowRect.top + (m_RestoreWindowRect.Height() * m_RestorePivot.y);
							GetTransform()->SetLocation(x, y);
							GetTransform()->SetSize(m_RestoreWindowRect.Width(), m_RestoreWindowRect.Height());
							m_ElementRoot->GetTransform()->SetSize(m_RestoreWindowRect.Width(), m_RestoreWindowRect.Height());
							SetFlags((JGUI::WindowFlags)(m_Flags | JGUI::WindowFlag_Resize));
						}
						else
						{
							//m_Maximizing = true;
							m_IsMaximizeState = true;
							m_RestoreWindowRect = GetTransform()->GetRect();
							m_RestorePivot      = GetTransform()->GetPivot();
							RECT rc;
							SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, FALSE);
							float width = rc.right - rc.left;
							float height = rc.bottom - rc.top;

							GetTransform()->SetPivot(0, 0);
							GetTransform()->SetSize(width, height);
							m_ElementRoot->GetTransform()->SetSize(width, height);
							GetTransform()->SetLocation(0, 0);

							// Resize 삭제
							SetFlags((JGUI::WindowFlags)(m_Flags & (~JGUI::WindowFlag_Resize)));
						}
					});

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
					m_Collection.closeBt = m_Collection.titleBar->CreateElement<Element>(TT("CloseBt"));
					ElementTemplate::Template_Button(m_Collection.closeBt);
					auto transform = m_Collection.closeBt->GetTransform();
					transform->SetSize(style.TitleBarHeight, style.TitleBarHeight);
					transform->SetPivot(1.0f, 0.0f);
					transform->SetAnchor(AnchorPreset::Right_Top);
					auto btCom = m_Collection.closeBt->FindComponent<Button>();
					btCom->SetAllImage(style.GUIImages[GUIStyle_Resource_CloseBt]);
					btCom->BindOnClick([&]()
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

		m_WorkAreaOff = work_area_off;
		if (m_Collection.resizeGrip)
		{
			m_Collection.resizeGrip->GetTransform()->SetLocalLocation(work_area_off.right, work_area_off.bottom);
		}
		if (m_Collection.titleBar)
		{
			m_Collection.titleBar->GetTransform()->SetSize(GetTransform()->GetSize().x - work_area_off.right, style.TitleBarHeight);
			m_Collection.titleBar->GetTransform()->SetLocalLocation(0.0f, work_area_off.top);

			m_WorkAreaOff.top += style.TitleBarHeight;
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
		auto& io  = GUIIF::GetIO();
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
	void Window::TrackingFocusElement(bool is_under)
	{
		JVector2 cursorPos = GUIIF::GetCursorPos();

		m_FocusElement = nullptr;
		
		auto& interactionItem = (is_under) ? m_UnderInteractionItems : m_AboveInteractionItems;
		for (auto& item_pair : interactionItem)
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
	}
	void Window::TrackingItemFlush(Element* prevFocusElement)
	{
		if (prevFocusElement != m_FocusElement)
		{
			if (prevFocusElement) prevFocusElement->OnFocusExit();
			if (m_FocusElement) m_FocusElement->OnFocusEnter();
			if (prevFocusElement) GUILOG_INFO("%s : FocusExit", prevFocusElement->GetName().c_str());
			if (m_FocusElement)   GUILOG_INFO("%s : FocusEnter", m_FocusElement->GetName().c_str());
		}


		m_UnderInteractionItems.clear();
		m_AboveInteractionItems.clear();
	}
}