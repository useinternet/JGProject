#include"pch.h"
#include"Camera.h"
using namespace std;
using namespace DirectX;
namespace GR
{
	namespace Dx12
	{
		Camera::Camera() :
			m_Position(0.0f, 0.0f, 0.0f),
			m_Rotator(0.0f,0.0f,0.0f),
			m_FarZ(0.0f), m_NearZ(0.0f),
			m_Width(0.0f), m_Height(0.0f),
			m_Fov(0.0f), m_IsUpdate(true) {}
		
		void Camera::SetLens(float fov, float width, float height, float nearZ, float farZ)
		{
			m_FarZ = farZ;
			m_NearZ = nearZ;
			m_Width = width;
			m_Height = height;
			m_Fov = fov;
		

			m_ProjMatrix = XMMatrixPerspectiveFovLH(fov, width / height, nearZ, farZ);
			m_IsUpdate   = true;
		}
		void Camera::SetPosition(const float3& pos)
		{
			m_Position = pos;
			m_IsUpdate = true;
		}
		void Camera::RotationPitch(float pitch)
		{
			m_Rotator.x = XMConvertToRadians(pitch);
			m_IsUpdate = true;
		}
		void Camera::RotationYaw(float yaw)
		{
			m_Rotator.y = XMConvertToRadians(yaw);
			m_IsUpdate = true;
		}
		void Camera::RotationRoll(float roll)
		{
			m_Rotator.z = XMConvertToRadians(roll);
			m_IsUpdate = true;
		}
		void Camera::Rotation(float pitch, float yaw, float roll)
		{
			m_Rotator = { XMConvertToRadians(pitch), XMConvertToRadians(yaw), XMConvertToRadians(roll) };
			m_IsUpdate = true;

		}

		void Camera::GetViewMatrix(float4x4* m) const
		{
			UpdateMatrix();
			XMStoreFloat4x4(m, m_ViewMatrix);
		}
		void Camera::GetProjMatrix(float4x4* m) const
		{
			XMStoreFloat4x4(m, m_ProjMatrix);
		}
		void Camera::GetViewProjMatrix(float4x4* m) const
		{
			UpdateMatrix();
			XMStoreFloat4x4(m, m_FinalMatrix);
		}

		void Camera::GetHLSLViewMatrix(float4x4* m) const
		{
			UpdateMatrix();
			sim_matrix simMat = XMMatrixTranspose(m_ViewMatrix);
			XMStoreFloat4x4(m, simMat);
		}
		void Camera::GetHLSLProjMatrix(float4x4* m) const
		{
			sim_matrix simMat = XMMatrixTranspose(m_ProjMatrix);
			XMStoreFloat4x4(m, simMat);
		}
		void Camera::GetHLSLViewProjMatrix(float4x4* m) const
		{
			UpdateMatrix();
			sim_matrix simMat = XMMatrixTranspose(m_FinalMatrix);
			XMStoreFloat4x4(m, simMat);
		}

		float3 Camera::GetLookVector()  const
		{
			float3 target = { 0.0f,0.0f,1.0f };
			sim_vector targetVec = XMLoadFloat3(&target);
			auto rotationMat = XMMatrixRotationRollPitchYaw(m_Rotator.x, m_Rotator.y, m_Rotator.z);


			targetVec = XMVector3TransformNormal(targetVec, rotationMat);
			targetVec = XMVector3Normalize(targetVec);

			XMStoreFloat3(&target, targetVec);
			return target;
		}
		float3 Camera::GetRightVector() const
		{
			auto look = GetLookVector();
			auto up = GetUpVector();

			sim_vector lookVec = XMLoadFloat3(&look);
			sim_vector upVec = XMLoadFloat3(&up);

			sim_vector rightVec = XMVector3Cross(upVec, lookVec);
			float3 right;
			XMStoreFloat3(&right, rightVec);
			return right;
		}
		float3 Camera::GetUpVector()    const
		{
			float3 up = { 0.0f,1.0f,0.0f };
			sim_vector upVec = XMLoadFloat3(&up);
			auto rotationMat = XMMatrixRotationRollPitchYaw(m_Rotator.x, m_Rotator.y, m_Rotator.z);

			//
			upVec = XMVector3TransformNormal(upVec, rotationMat);
			upVec = XMVector3Normalize(upVec);

			XMStoreFloat3(&up, upVec);
			return up;
		}
		const float3& Camera::GetPosition() const
		{
			return m_Position;
		}
		void Camera::UpdateMatrix() const
		{
			if (!m_IsUpdate)
				return;
			float3 target = { 0.0f,0.0f,1.0f };
			float3 up     = { 0.0f,1.0f,0.0f };
			// 
			sim_vector targetVec = XMLoadFloat3(&target);
			sim_vector upVec = XMLoadFloat3(&up);
			sim_vector posVec = XMLoadFloat3(&m_Position);

			auto rotationMat = XMMatrixRotationRollPitchYaw(m_Rotator.x, m_Rotator.y, m_Rotator.z);

			targetVec = XMVector3TransformNormal(targetVec, rotationMat);
			upVec = XMVector3TransformNormal(upVec, rotationMat);


			m_ViewMatrix = DirectX::XMMatrixLookAtLH(posVec, XMVectorAdd(posVec, targetVec), upVec);


			m_FinalMatrix = XMMatrixMultiply(m_ViewMatrix, m_ProjMatrix);
			m_IsUpdate = false;
		}

	}
}