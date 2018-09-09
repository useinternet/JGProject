#pragma once
#include"Widget.h"


class StaticMesh2DComponent;
class InputComponent;
enum class EPivot;

/*
EnumClas : EButtonImageType
@e Ordinary : 평상시 버튼 이미지
@e ButtonClick : 버튼 클릭 시 이미지
@e MousePointerInButton : 마우스 포인터가 버튼 안에 있을 시의 이미지 */
enum class EButtonImageType
{
	Ordinary,
	ButtonClick,
	MousePointerInButton
};
/*
Class : Button 
@m unique_ptr<StaticMesh2DComponent> m_OrdinaryImage    : 평상시 이미지
@m unique_ptr<StaticMesh2DComponent> m_ButtonClickImage : 버튼 클릭시 이미지
@m unique_ptr<StaticMesh2DComponent> m_MousePointerInButtonImage : 마우스 포인터가 버튼 안에 있을 시의 이미지
@m unique_ptr<InputComponent> m_InputComponent : 입력 컴포넌트( 마우스 입력 )
@m unique_ptr<SButtonRect> m_ButtonRect : 버튼 충돌용 사각형
@m EButtonImageType m_ButtonImageType : 버튼 이미지 타입  */
class ENGINE_EXPORT Button : public Widget
{
	typedef struct SButtonRect
	{
		float Top;
		float Left;
		float Right;
		float Bottom;
	}SButtonRect;
private:
	std::unique_ptr<StaticMesh2DComponent> m_OrdinaryImage;
	std::unique_ptr<StaticMesh2DComponent> m_ButtonClickImage;
	std::unique_ptr<StaticMesh2DComponent> m_MousePointerInButtonImage;
	std::unique_ptr<InputComponent> m_InputComponent;
	std::unique_ptr<SButtonRect> m_ButtonRect;
	EButtonImageType m_ButtonImageType;

	bool m_bPosInit = false;
public:
	Button();
	virtual ~Button();
	virtual void Tick(const float DeltaTime) override;

	/*
	Exp : 버튼 생성
	@param const EPivot pivot
	@param cosnt wstring& OrdinaryImagePath : 평상시 버튼 이미지 경로
	@param const wstring& ButtonClickImagePath : 버튼 클릭시 이미지 경로
	@param const wstring& MousePointerInButtonImagePath : 마우스 포인터가 버튼 사각형안에 있을시 이미지 경로*/
	void CreateButton(const EPivot pivot,const std::wstring& OrdinaryImagePath, const std::wstring& ButtonClickPath = TT("None"), const std::wstring& MousePointerInButtonImagePath = TT("None"));
	/*
	Exp : 알파값 셋팅 
	@param const float a : 셋팅할 알파 값
	@param const EButtonImageType buttonType : 셋팅할 버튼 타입 */
	void SetBlend(const float a,const EButtonImageType buttonType);
	/*
	Exp : 마우스 포인터와 충돌 체크*/
	bool CollisionCheck();
	/*
	Exp : 버튼 이미지 타입 셋팅 */
	void SetButtonImage(const EButtonImageType type);

	/*
	Exp : 현제 마우스 포인터가 버튼 충돌 사각혀 안에 있는지 여부 */
	bool IsMousePointerInButton();
	/*
	Exp : 버튼클릭 여부 */
	bool IsMouseClickInButton();
	/*
	Exp : 평상시 여부 */
	bool IsNoneInButton();
};