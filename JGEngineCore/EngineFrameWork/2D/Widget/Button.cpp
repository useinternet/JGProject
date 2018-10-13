#include"Button.h"
#include"../../Components/StaticMesh2DComponent.h"
#include"../../../EngineStatics/JGConstructHelper.h"
#include"../../../RenderSystem/JGMaterial/JG2DMesh.h"
#include"../../../InputSystem/JGCommandManager.h"
#include"../../Components/InputComponent.h"
#include"../../../InputSystem/JGPressManager.h"
#include"../../../EngineStatics/JGLog.h"
using namespace std;
Button::Button()
{
	RegisterComponentID(typeid(this));
	m_ButtonRect      = make_unique <SButtonRect>();
	m_ButtonImageType = EButtonImageType::Ordinary;
}

Button::~Button()
{

}
void Button::Tick(const float DeltaTime)
{
	Widget::Tick(DeltaTime);
	m_IsButtonClick = false;

	if (IsChangeScale())
	{
		m_OrdinaryImage->SetComponentScale(GetComponentScale());
		m_ButtonClickImage->SetComponentScale(GetComponentScale());
		m_MousePointerInButtonImage->SetComponentScale(GetComponentScale());
	}
	if (IsChangeLocation())
	{
		JGVector2D center = m_OrdinaryImage->GetCenterPoint();
		float height = m_OrdinaryImage->GetTextureHeight() * GetComponentScale().Y();
		float width = m_OrdinaryImage->GetTextureWdith()   * GetComponentScale().X();
		/// 버튼 충돌 사각형 초기화
		m_ButtonRect->Top = center.Y() - (height / 2);
		m_ButtonRect->Left = center.X() - (width / 2);
		m_ButtonRect->Right = center.X() + (width / 2);
		m_ButtonRect->Bottom = center.Y() + (height / 2);

	}

	/// 버튼 충돌 체크( 충돌이면 마우스 PutMouse 모드로.. 아니면 Ordinary모드로..
	bool result = CollisionCheck();
	if (result && m_ButtonImageType != EButtonImageType::ButtonClick)
	{
		m_ButtonImageType = EButtonImageType::MousePointerInButton;
	}
	else if(!result)
	{
		m_ButtonImageType = EButtonImageType::Ordinary;
	}

	/// 현제 버튼 이미지 타입에따라 .. 이미지들을 숨기거나 그린다.
	switch (m_ButtonImageType)
	{
	case EButtonImageType::ButtonClick:
		m_ButtonClickImage->ActiveComponent();
		//
		m_OrdinaryImage->BehindComponent();
		m_MousePointerInButtonImage->BehindComponent();
		break;
	case EButtonImageType::Ordinary:
		m_OrdinaryImage->ActiveComponent();
		//
		m_ButtonClickImage->BehindComponent();
		m_MousePointerInButtonImage->BehindComponent();
		break;
	case EButtonImageType::MousePointerInButton:
		m_MousePointerInButtonImage->ActiveComponent();
		//
		m_OrdinaryImage->BehindComponent();
		m_ButtonClickImage->BehindComponent();
		break;
	}
	if (m_ButtonImageType != m_PrevButtonImageType)
	{
		if (m_ButtonImageType == EButtonImageType::MousePointerInButton &&
			m_PrevButtonImageType == EButtonImageType::ButtonClick)
		{
			m_IsButtonClick = true;
		}
		m_PrevButtonImageType = m_ButtonImageType;

	}
}


void Button::CreateButton(const EPivot pivot, const wstring& OrdinaryImagePath, const wstring& ButtonClickPath, const wstring& MousePointerInButtonImagePath)
{
	/// 평상시일때의 버튼 이미지 메쉬 생성..
	m_OrdinaryImage = make_unique<StaticMesh2DComponent>();
	m_OrdinaryImage->SetOwnerObject(GetOwnerObject());
	/// 컴포넌트 등록..
	AddChild(m_OrdinaryImage.get());
	m_OrdinaryImage->RegisterName(GetComponentName() + TT("_Ordinary"));
	m_OrdinaryImage->UseDefaultViewMatrix();
	/// 리소스 오브젝트 생성..
	JGConstructHelper::StaticMesh2D OrdinaryImageMesh(
		m_OrdinaryImage->GetComponentName(),pivot, OrdinaryImagePath);
	if (OrdinaryImageMesh.Success)
	{
		m_OrdinaryImage->SetConstructObject(OrdinaryImageMesh.Object);
	}



	/// 버튼 눌렀을떄 이미지 생성
	m_ButtonClickImage = make_unique<StaticMesh2DComponent>();
	m_ButtonClickImage->SetOwnerObject(GetOwnerObject());
	/// 컴포넌트 등록..
	AddChild(m_ButtonClickImage.get());
	m_ButtonClickImage->BehindComponent();
	m_ButtonClickImage->RegisterName(GetComponentName() + TT("_ButtonClick"));
	if (ButtonClickPath != TT("None"))
	{
		/// 리소스 오브젝트 생성..
		JGConstructHelper::StaticMesh2D ClickImageMesh(
			m_ButtonClickImage->GetComponentName(), pivot, ButtonClickPath);
		if (ClickImageMesh.Success)
		{
			m_ButtonClickImage->SetConstructObject(ClickImageMesh.Object);
		}
	}
	else
	{
		if (OrdinaryImageMesh.Success)
		{
			m_ButtonClickImage->SetConstructObject(OrdinaryImageMesh.Object);
		}
	}
	m_ButtonClickImage->UseDefaultViewMatrix();


	/// 마우스를 데었을때 이미지 생성.
	m_MousePointerInButtonImage = make_unique<StaticMesh2DComponent>();
	m_MousePointerInButtonImage->SetOwnerObject(GetOwnerObject());
	/// 컴포넌트 등록..
	AddChild(m_MousePointerInButtonImage.get());
	m_MousePointerInButtonImage->BehindComponent();
	m_MousePointerInButtonImage->RegisterName(GetComponentName() + TT("_MousePointerInButton"));
	if (MousePointerInButtonImagePath != TT("None"))
	{
		/// 리소스 오브젝트 생성..
		JGConstructHelper::StaticMesh2D MousePointerInButtonImageMesh(
			m_MousePointerInButtonImage->GetComponentName(),
			pivot, MousePointerInButtonImagePath);
		if (MousePointerInButtonImageMesh.Success)
		{
			m_MousePointerInButtonImage->SetConstructObject(MousePointerInButtonImageMesh.Object);
		}
	}
	else
	{
		if (OrdinaryImageMesh.Success)
		{
			m_MousePointerInButtonImage->SetConstructObject(OrdinaryImageMesh.Object);
		}
	}
	m_MousePointerInButtonImage->UseDefaultViewMatrix();


	/// 마우스 버튼 관련해서 키 등록
	m_InputComponent = make_unique<InputComponent>();
	m_InputComponent->SetOwnerObject(GetOwnerObject());
	m_InputComponent->RegisterName(GetComponentName() + TT("InputEvent"));
	GetCommandManager()->RegisterKeyCommand(EKey::MouseLButton, m_InputComponent->GetComponentName());
	// 바인딩된 키 이벤트 함수 등록..
	m_InputComponent->BindKeyCommand(
		m_InputComponent->GetComponentName(), EKeyState::Down, [this]()
	{
		bool result = true;
		result = this->CollisionCheck();
		if (result)
		{
			this->SetButtonImage(EButtonImageType::ButtonClick);
		}
		else
		{
			this->SetButtonImage(EButtonImageType::Ordinary);
		}
	});
	m_InputComponent->BindKeyCommand(
		m_InputComponent->GetComponentName(), EKeyState::Up, [this]()
	{
		bool result = true;
		result = this->CollisionCheck();
		if (result)
		{
			this->SetButtonImage(EButtonImageType::MousePointerInButton);
		}
		else
		{
			this->SetButtonImage(EButtonImageType::Ordinary);
		}
	});
}

void Button::SetBlend(const float a,const EButtonImageType buttonType)
{
	switch (buttonType)
	{
	case EButtonImageType::Ordinary:
		m_OrdinaryImage->SetAlphaBlend(a);
		break;
	case EButtonImageType::ButtonClick:
		m_ButtonClickImage->SetAlphaBlend(a);
		break;
	case EButtonImageType::MousePointerInButton:
		m_MousePointerInButtonImage->SetAlphaBlend(a);
		break;
	}
}
bool Button::CollisionCheck()
{
	JGVector2D MousePos = GetCommandManager()->GetMouseLocation();
	if ((m_ButtonRect->Left < MousePos.X()  && MousePos.X() < m_ButtonRect->Right) &&
		(m_ButtonRect->Top  < MousePos.Y()  && MousePos.Y() < m_ButtonRect->Bottom))
	{
		return true;
	}
	return false;
}

void Button::SetButtonImage(const EButtonImageType type)
{
	m_ButtonImageType = type;
}

bool Button::IsMousePointerInButton()
{
	if (m_ButtonImageType == EButtonImageType::MousePointerInButton)
	{
		return true;
	}
	return false;
}
bool Button::IsMousePressedInButton()
{
	if (m_ButtonImageType == EButtonImageType::ButtonClick)
	{
		return true;
	}
	return false;
}
bool Button::IsNoneInButton()
{
	if (m_ButtonImageType == EButtonImageType::Ordinary)
	{
		return true;
	}
	return false;
}

bool Button::IsButtonClick()
{
	return m_IsButtonClick;
}
