#pragma once
#include"Dx12Include.h"


namespace GR
{
	namespace Dx12
	{

		class Camera
		{
		private:
			mutable sim_matrix m_ProjMatrix;
			mutable sim_matrix m_ViewMatrix;
			mutable sim_matrix m_FinalMatrix;
			float3   m_Position;
			float3   m_Rotator;
			float    m_FarZ;
			float    m_NearZ;
			float    m_Width;
			float    m_Height;
			float    m_Fov;
			mutable DirectX::BoundingFrustum m_Frustum;

			mutable bool m_IsUpdate;
		public:
			Camera();
			void SetLens(float fov, float width, float height, float nearZ, float farZ);
			void SetPosition(const float3& pos);
			void RotationPitch(float pitch);
			void RotationYaw(float yaw);
			void RotationRoll(float roll);
			void Rotation(float pitch, float yaw, float roll);


			void GetViewMatrix(float4x4* m) const;
			void GetProjMatrix(float4x4* m) const;
			void GetViewProjMatrix(float4x4* m) const;

			void GetHLSLViewMatrix(float4x4* m) const;
			void GetHLSLProjMatrix(float4x4* m) const;
			void GetHLSLViewProjMatrix(float4x4* m) const;

			float3 GetLookVector()  const;
			float3 GetRightVector() const;
			float3 GetUpVector()    const;
			float GetFarZ() const;
			float GetNearZ() const;
			const DirectX::BoundingFrustum& GetFrustum() const;
			const DirectX::BoundingFrustum& GetFrustum();
			const float3& GetPosition() const;
			bool IsUpadte() const {
				return m_IsUpdate;
			}
		private:
			void UpdateMatrix() const;

		};
	}
}