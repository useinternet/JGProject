#include"Camera2DComponent.h"
#include"../World/World.h"
#include"../../RenderSystem/JGViewportD.h"
using namespace std;
Camera2DComponent::Camera2DComponent()
{
	RegisterComponentID(typeid(this));
	m_ViewMatrix = make_unique<JGMatrix>();
}

Camera2DComponent::~Camera2DComponent()
{

}
void Camera2DComponent::Tick(const float DeltaTime)
{
	Motivated2DComponent::Tick(DeltaTime);
	if (IsChangeLocation())
	{
		CalculateViewMatrix();
	}
}
void Camera2DComponent::CalculateViewMatrix()
{
	JGVector2D worldLocation = GetComponentWorldLocation();
	worldLocation.Set(worldLocation.X() - (GetViewport()->GetWidth() / 2),
		-worldLocation.Y() + (GetViewport()->GetHeight() / 2));

	JGVector3D up(0.0f,1.0f, 0.0f), 
		       position(worldLocation.X(), worldLocation.Y(), -10.0f),
		       lookAt(worldLocation.X(), worldLocation.Y(), 1.0f);
	JGMatrix rotationMatrix;

	rotationMatrix.MakeRotationMatirx(0.0f, 0.0f, 0.0f);
	lookAt.TransformCoord(rotationMatrix);
	up.TransformCoord(rotationMatrix);


	m_ViewMatrix->MakeViewMatrix(&position, &lookAt, &up);
}

JGMatrix* Camera2DComponent::GetViewMatrixPointer()
{
	return m_ViewMatrix.get();
}
