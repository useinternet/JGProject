#include"MotivatedComponent.h"
#include"../../RenderSystem/JGViewportD.h"
#include"../World/World.h"
using namespace std;
MotivatedComponent::MotivatedComponent()
{
	RegisterComponentID(typeid(this));
}
MotivatedComponent::~MotivatedComponent()
{

}
const JGMatrix& MotivatedComponent::GetViewMatrix()
{
	if (m_bDefaultView)
	{
		return GetWorld()->GetDefaultViewMatrix();
	}
	else
	{
		return GetWorld()->GetViewMatrix();
	}

}
const JGMatrix& MotivatedComponent::GetProjectionMatrix()
{
	return GetViewport()->GetProjectionMatrix();
}
const JGMatrix & MotivatedComponent::GetOrthoMatrix()
{
	return GetViewport()->GetOrthoMatrix();
}

void MotivatedComponent::UseDefaultViewMatrix()
{
	m_bDefaultView = true;
}


