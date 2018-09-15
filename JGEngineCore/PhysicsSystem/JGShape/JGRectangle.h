#pragma once
#include"JGShape.h"

class ENGINE_EXPORT JGRectangle : public JGShape
{
private:
	std::unique_ptr<JGVector2D> m_LeftTop;
	std::unique_ptr<JGVector2D> m_RightBottom;
	float m_half_Width = 0.0f;
	float m_half_Height = 0.0f;
	float m_Prev_half_Width = 0.0f;
	float m_Prev_half_Height = 0.0f;
	bool m_bTransSize = false;
public:
	JGRectangle() = delete;
	JGRectangle(EShapeType type, JGVector2D* pVector, JGAngle2D* pAngle);
	virtual ~JGRectangle();
	virtual void Tick() override;
	void SetRectangle(const float half_Width, const float half_Height);
	JGVector2D& GetLeftTop();
	JGVector2D& GetRightBottom();
	float GetHalfWidth();
	float GetHalfHeight();
	bool GetIsTransRectangleSize();
};