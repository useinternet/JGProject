#pragma once
#include"JGShape.h"

class ENGINE_EXPORT JGRectangle : public JGShape
{
private:
	std::unique_ptr<JGVector2D> m_LeftTop;
	std::unique_ptr<JGVector2D> m_RightBottom;
	std::unique_ptr<JGVector2D> m_PrevLocation;
	float m_half_Width = 0.0f;
	float m_half_Height = 0.0f;
public:
	JGRectangle() = delete;
	JGRectangle(EShapeType type);
	virtual ~JGRectangle();
	virtual void Tick() override;
	void SetRectangle(const float half_Width, const float half_Height);
	JGVector2D& GetLeftTop();
	JGVector2D& GetRightBottom();
	float GetHalfWidth();
	float GetHalfHeight();
};