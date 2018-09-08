#pragma once
#include"Widget.h"


class StaticMesh2DComponent;

enum class EButtonImageType
{
	Ordinary,
	MouseButtonDown,
	PutMouse
};
// 충돌 사각형 만들어야함..
class ENGINE_EXPORT Button : public Widget
{
private:
	std::unique_ptr<StaticMesh2DComponent> m_OrdinaryImage;
	std::unique_ptr<StaticMesh2DComponent> m_MouseButtonDownImage;
	std::unique_ptr<StaticMesh2DComponent> m_PutMouseImage;

public:
	Button();
	virtual ~Button();

	void CreateButton(const std::wstring& OrdinaryImagePath, const std::wstring& MouseButtonDownPath = TT("None"), const std::wstring& PutMouseImagePath = TT("None"));
	void SetBlend(const float a, EButtonImageType buttonType);
};