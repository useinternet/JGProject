#include"MotivatedComponent.h"
#include"../../RenderSystem/JGViewportD.h"
using namespace std;
MotivatedComponent::MotivatedComponent()
{
	RegisterComponentID(typeid(this));
	m_ViewMatrix = make_unique<JGMatrix>();
	TempViewMatrixInit();
}

MotivatedComponent::~MotivatedComponent()
{

}

const JGMatrix& MotivatedComponent::GetViewMatrix()
{
	return *m_ViewMatrix;
}
const JGMatrix& MotivatedComponent::GetProjectionMatrix()
{
	return GetViewport()->GetProjectionMatrix();
}
const JGMatrix & MotivatedComponent::GetOrthoMatrix()
{
	return GetViewport()->GetOrthoMatrix();
}
void MotivatedComponent::TempViewMatrixInit()
{
	JGVector3D up(0.0f, 1.0f, 0.0f), position(0.0f, 0.0f, -10.0f), lookAt(0.0f, 0.0f, 1.0f);
	JGMatrix rotationMatrix;

	rotationMatrix.MakeRotationMatirx(0.0f, 0.0f, 0.0f);
	lookAt.TransformCoord(rotationMatrix);
	up.TransformCoord(rotationMatrix);


	m_ViewMatrix->MakeViewMatrix(&position, &lookAt, &up);
}

