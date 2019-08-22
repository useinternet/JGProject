#include"pch.h"
#include"Light.h"
#include"GraphicsDevice.h"
using namespace DirectX;
namespace GR
{
	namespace Dx12
	{
		uint32_t Light::DepthSize = 1024;
		Light::Light(uint32_t arraySize)
		{
			auto device = GraphicsDevice::GetPointer();
			GPUResource resource = device->CreateRenderTargetGPUResource(DXGI_FORMAT_R32_FLOAT, DepthSize,
				DepthSize, 1, arraySize, { 1.0f,0.0f,0.0f,0.0f });
			GPUResource depthresource = device->CreateDepthStencilGPUResource(DXGI_FORMAT_D24_UNORM_S8_UINT,
				DepthSize, DepthSize, 1, arraySize);


			ColorTexture texture = device->CreateColorTexture(resource, { 1.0f,0.0f,0.0f,0.0f });
			DepthTexture depthtexture = device->CreateDepthTexture(depthresource);

			m_Rendertarget.AttachTexture(RenderTarget::Slot0, texture);
			m_Rendertarget.AttachTexture(RenderTarget::DepthStencil, depthtexture);
			m_IsUpdate = true;
		}
		ColorTexture* Light::GetTexture() {
			return m_Rendertarget.GetTexture(RenderTarget::Slot0);
		}
		uint32_t Light::GetType() const {
			return m_Data.Type;
		}
		void Light::SetBias(float bias)
		{
			m_Data.Bias = bias;
		}
		float Light::GetBias(float bias) const
		{
			return m_Data.Bias;
		}
		DepthTexture* Light::GetDepthTexture() {
			return m_Rendertarget.GetDepthTexture();
		}
		RenderTarget* Light::GetRenderTarget() {
			return &m_Rendertarget;
		}




		PointLight::PointLight() : Light(6)
		{
			m_Data.Type = LightCB::Point;
			m_Data.Position = { 0.0f,0.0f,0.0f };
			m_Data.Color = { 1.0f,1.0f,1.0f };
			m_Data.Intensity = 1.0f;
			m_Data.Range = 1000.0f;
			m_Data.AttConstant1 = 0.2f;
			m_Data.AttConstant2 = 0.1f;


			for (int i = 0; i < 6; ++i)
			{
				m_Camera[i].SetLens(0.5f * XM_PI, DepthSize, DepthSize,
					1.0f, m_Data.Range);
				m_Camera[i].SetPosition(m_Data.Position);
			}
			// right left
			m_Camera[0].RotationYaw(90.0f);
			m_Camera[1].RotationYaw(-90.0f);
			// up down
			m_Camera[2].RotationPitch(-90.0f);
			m_Camera[3].RotationPitch(90.0f);
			// foward back
			m_Camera[4].RotationYaw(0.0f);
			m_Camera[5].RotationYaw(180.0f);

			auto texture = m_Rendertarget.GetTexture(RenderTarget::Slot0);
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = texture->GetResource()->GetDesc().Format;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.TextureCube.MipLevels = texture->GetResource()->GetDesc().MipLevels;
			srvDesc.TextureCube.MostDetailedMip = 0;
			srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
			texture->SetSRVDesc(&srvDesc);

			m_IsUpdate = true;
		}
		void PointLight::SetPosition(float x, float y, float z)
		{
			m_Data.Position = { x,y,z };
			m_IsUpdate = true;
		}
		void PointLight::SetColor(float r, float g, float b)
		{
			m_Data.Color = { r,g,b };
		}
		void PointLight::SetIntensity(float i)
		{
			m_Data.Intensity = i;
		}
		void PointLight::SetAttConstant1(float a)
		{
			m_Data.AttConstant1 = a;
		}
		void PointLight::SetAttConstant2(float a)
		{
			m_Data.AttConstant2 = a;
		}
		void PointLight::SetRange(float r)
		{
			m_Data.Range = r;
		}
		PassCB& PointLight::GetPassCB(int slot) 
		{
			Update();
			return m_PassCB[slot];
		}
		const DirectX::BoundingFrustum& PointLight::GetFrustum(int slot)
		{
			assert(false && "DirectionLight not need frustum!!");
			Update();
			return m_Camera[slot].GetFrustum();
		}
		void PointLight::Update()
		{
			if (!m_IsUpdate)
				return;
			for (int i = 0; i < 6; ++i)
			{
				m_Camera[i].SetLens(0.5f * XM_PI, DepthSize, DepthSize,
					1.0f, m_Data.Range);
				m_Camera[i].SetPosition(m_Data.Position);
				m_PassCB[i].Set(m_Camera[i]);
			}
			m_IsUpdate = false;
		}





		DirectionLight::DirectionLight() : Light()
		{
			m_Around = { XMConvertToRadians(20),0.0f,0.0f };
			m_Distance = 0.0f;
			m_Data.Type = LightCB::Directional;
			m_Data.Direction = { 0.0f, 0.0f, 1.0f };
			m_Data.Intensity = 1.0;
			m_Data.Color = { 0.7f,0.78f,1.0f };
		}
		void DirectionLight::SetAround(float x, float y, float z)
		{
			x = XMConvertToRadians(x);
			y = XMConvertToRadians(y);
			z = XMConvertToRadians(z);
			m_Around = { x,y,z };

			m_IsUpdate = true;
		}
		void DirectionLight::SetColor(float r, float g, float b)
		{
			m_Data.Color = { r,g,b };
		}
		void DirectionLight::SetIntensity(float i)
		{
			m_Data.Intensity = i;
		}
		PassCB& DirectionLight::GetPassCB(int slot) 
		{
			Update();
			return m_PassCB;
		}

		const DirectX::BoundingFrustum& DirectionLight::GetFrustum(int slot)
		{
			assert(false && "DirectionLight not need frustum!!");
			Update();
			return m_Frustum;
		}
		void DirectionLight::Update()
		{
			if (!m_IsUpdate)
				return;
			float3 dir = { 0.0f,0.0f,1.0f };
			sim_matrix m = XMMatrixRotationRollPitchYaw(m_Around.x, m_Around.y, m_Around.z);
			auto v = XMVector3TransformNormal(XMLoadFloat3(&dir), m);
			XMStoreFloat3(&m_Data.Direction, XMVector3Normalize(v));
			// pass 데이터 업데이트
			{
				float  farZ = m_Distance * 5.656f;
				float  nearZ = 1.0f;
				float  fov = 0.5f * XM_PI;
				sim_vector sim_dir = XMVector3Normalize(XMLoadFloat3(&m_Data.Direction));
				float3 dir;
				XMStoreFloat3(&dir, sim_dir);

				float3 up = { 0.0f,1.0f,0.0f };
				float3 right = { 0.0f,0.0f,0.0f };
				float3 target = { 0.0f,0.0f,1.0f };
				
				float distance = m_Distance * 2.828f;
				float3 pos = { -dir.x * distance, -dir.y * distance, -dir.z * distance };
				float3 look = { -pos.x,-pos.y,-pos.z };

				
				sim_vector sim_look = XMVector3Normalize(XMLoadFloat3(&look));
				sim_vector sim_up = XMLoadFloat3(&up);
				sim_vector sim_right = XMVector3Normalize(XMVector3Cross(sim_up, sim_look));
				XMStoreFloat3(&look, sim_look);
				XMStoreFloat3(&right, sim_right);


				sim_up = XMVector3Normalize(XMVector3Cross(sim_look, sim_right));
				XMStoreFloat3(&up, sim_up);


				sim_vector sim_pos = XMLoadFloat3(&pos);
				sim_vector sim_target = XMVectorZero();

				sim_matrix sim_view = XMMatrixLookAtLH(sim_pos, sim_target, sim_up);
				
				sim_matrix sim_proj = XMMatrixPerspectiveFovLH(fov, DepthSize / DepthSize, nearZ, farZ);
				sim_matrix sim_viewproj = XMMatrixMultiply(sim_view, sim_proj);

				m_PassCB.EyePos = pos;
				m_PassCB.NearZ = nearZ;
				m_PassCB.FarZ = farZ;
				XMStoreFloat4x4(&m_PassCB.View, XMMatrixTranspose(sim_view));
				XMStoreFloat4x4(&m_PassCB.Proj, XMMatrixTranspose(sim_proj));
				XMStoreFloat4x4(&m_PassCB.ViewProj, XMMatrixTranspose(sim_viewproj));
				XMStoreFloat4x4(&m_PassCB.InvView, XMMatrixTranspose(XMMatrixInverse(&XMMatrixDeterminant(sim_view), sim_view)));
				XMStoreFloat4x4(&m_PassCB.InvProj, XMMatrixTranspose(XMMatrixInverse(&XMMatrixDeterminant(sim_proj), sim_proj)));
				XMStoreFloat4x4(&m_PassCB.InvViewProj, XMMatrixTranspose(XMMatrixInverse(&XMMatrixDeterminant(sim_viewproj), sim_viewproj)));
			}
			m_IsUpdate = false;
		}



		SpotLight::SpotLight() : Light()
		{
			m_Data.Intensity = 1.0f;
			m_Data.Range = 1000.0f;
			m_Data.AttConstant1 = 0.2f;
			m_Data.AttConstant2 = 0.1f;
			m_Data.Color = { 1.0f,1.0f,1.0f };
			m_Data.Type = LightCB::Spot;
			m_Data.Bias = 0.0005f;
			SetOutAngle(90.0f);
			SetInAngle(45.0f);

			m_Camera.SetLens(m_Data.OutCos, DepthSize, DepthSize, 1.0f, m_Data.Range);
			m_Camera.SetPosition(m_Data.Position);
			m_PassCB.Set(m_Camera);
			m_Data.Direction = m_Camera.GetLookVector();
			m_IsUpdate = true;
		}
		void SpotLight::SetInAngle(float angle)
		{
			m_Data.InCos = XMConvertToRadians(angle);
			m_IsUpdate = true;
		}
		void SpotLight::SetOutAngle(float angle)
		{
			m_Data.OutCos = XMConvertToRadians(angle);
			m_IsUpdate = true;
		}
		void SpotLight::SetPosition(float x, float y, float z)
		{
			m_Data.Position = { x,y,z };
			m_IsUpdate = true;
		}
		void SpotLight::SetColor(float r, float g, float b)
		{
			m_Data.Color = { r,g,b };
		}
		void SpotLight::SetIntensity(float i)
		{
			m_Data.Intensity = i;
		}
		void SpotLight::SetAttConstant1(float a)
		{
			m_Data.AttConstant1 = a;
		}
		void SpotLight::SetAttConstant2(float a)
		{
			m_Data.AttConstant2 = a;
		}
		void SpotLight::SetRange(float r)
		{
			m_Data.Range = r;
			m_IsUpdate = true;
		}
		void SpotLight::SetRotation(float pitch, float yaw, float roll)
		{
			m_Camera.Rotation(pitch, yaw, roll);
			m_IsUpdate = true;
		}

		PassCB& SpotLight::GetPassCB(int slot)
		{
			Update();
			return m_PassCB;
		}
		const DirectX::BoundingFrustum& SpotLight::GetFrustum(int slot)
		{
			Update();
			return m_Camera.GetFrustum();
		}
		void SpotLight::Update()
		{
			if (!m_IsUpdate)
				return;
			m_Camera.SetLens(m_Data.OutCos, DepthSize, DepthSize, 1.0f, m_Data.Range );
			m_Camera.SetPosition(m_Data.Position);
			m_PassCB.Set(m_Camera);
			m_Data.Direction = m_Camera.GetLookVector();
			m_IsUpdate = false;
		}
	}


}