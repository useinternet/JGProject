#include"pch.h"
#include"ShaderType.h"
#include"Camera.h"

namespace GR
{
	namespace Dx12
	{
		void PassCB::Set(const Camera& camera)
		{
			camera.GetHLSLViewProjMatrix(&ViewProj);
			camera.GetHLSLViewMatrix(&View);
			camera.GetHLSLProjMatrix(&Proj);

			sim_matrix invViewproj = DirectX::XMLoadFloat4x4(&ViewProj);
			invViewproj = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(invViewproj), invViewproj);

			sim_matrix invView = DirectX::XMLoadFloat4x4(&View);
			invView = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(invView), invView);

			sim_matrix invProj = DirectX::XMLoadFloat4x4(&Proj);
			invProj = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(invProj), invProj);

			DirectX::XMStoreFloat4x4(&InvViewProj, invViewproj);
			DirectX::XMStoreFloat4x4(&InvView, invView);
			DirectX::XMStoreFloat4x4(&InvProj, invProj);

			EyePos = camera.GetPosition();
		}
	}
}