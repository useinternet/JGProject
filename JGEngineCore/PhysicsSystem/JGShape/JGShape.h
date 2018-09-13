#pragma once
#include"../../EngineStatics/Engine.h"
class JGVector2D;
class JGAngle2D;
enum class EPivot;

/*
EnumClass : EShapeType 
@e Circle : 원 타입.
@e Rectangle : 사각형 타입 */
enum class EShapeType
{
	Circle,
	Rectangle
};
/*
Class : JGShape 
@m EShapeType m_ShapeType : 도형 타입
@m unique_ptr<JGVector2D> m_Location :  위치
@m unique_ptr<JGAngle2D> m_Angle : 각도. */
class ENGINE_EXPORT JGShape
{
private:
	EShapeType m_ShapeType;
	JGVector2D* m_Location = nullptr;
	JGAngle2D*  m_Angle = nullptr;
public:
	JGShape() = delete;
	JGShape(EShapeType type,JGVector2D* pVector, JGAngle2D* pAngle);
	virtual ~JGShape();
	virtual void Tick();
	EShapeType  GetShapeType();
	JGVector2D& GetLocation();
	JGAngle2D&  GetAngle();

	void AddLocation(JGVector2D& vec);
	void AddAngle(JGAngle2D& angle);

	void SetLocation(const JGVector2D& vec);
	void SetAngle(const JGAngle2D& angle);

	bool IsValue();
};