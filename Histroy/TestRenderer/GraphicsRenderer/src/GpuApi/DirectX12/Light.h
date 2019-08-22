#pragma once
#include"Dx12Include.h"
#include"ShaderType.h"
#include"RenderTarget.h"
#include"Camera.h"
namespace GR
{
	namespace Dx12
	{
		class DepthFromLight;
		class ShadowMapping;
		class Renderer;
		class LightingPass;
		enum class ELightType
		{
			Ambient,
			Point,
			Direction,
			Spot
		};
		class Light
		{
		protected:
			friend DepthFromLight;
			friend Renderer;
			friend ShadowMapping;
			friend LightingPass;
			static uint32_t DepthSize;

		protected:
			LightCB m_Data;
			RenderTarget m_Rendertarget;
			bool m_IsUpdate;
		public:
			Light(uint32_t arraySize = 1);
			ColorTexture* GetTexture();
			uint32_t GetType() const;
			void SetBias(float bias);
			float GetBias(float bias) const;
		private:
	
			DepthTexture* GetDepthTexture();
			RenderTarget* GetRenderTarget();
			virtual PassCB& GetPassCB(int slot = 0) = 0;
			virtual const DirectX::BoundingFrustum& GetFrustum(int slot = 0) = 0;
		};
		class AmbientLight : public Light
		{

		};
		class PointLight : public Light
		{
		private:
			PassCB m_PassCB[6];
			Camera m_Camera[6];
		public:
			PointLight();
			void SetPosition(float x, float y, float z);
			void SetColor(float r, float g, float b);
			void SetIntensity(float i);
			void SetAttConstant1(float a);
			void SetAttConstant2(float a);
			void SetRange(float r);
		private:
			virtual PassCB& GetPassCB(int slot = 0) override;
			virtual const DirectX::BoundingFrustum& GetFrustum(int slot = 0) override;
			void Update();
		};
		class DirectionLight : public Light
		{
			friend Renderer;
		private:
			PassCB m_PassCB;
			float3 m_Around;
			float  m_Distance;
			DirectX::BoundingFrustum m_Frustum;
		public:
			DirectionLight();
			void SetAround(float x, float y, float z);
			void SetColor(float r, float g, float b);
			void SetIntensity(float i);
		private:
			virtual PassCB& GetPassCB(int slot =0) override;
			virtual const DirectX::BoundingFrustum& GetFrustum(int slot = 0) override;
			void Update();
		};
		class SpotLight : public Light
		{
		private:
			PassCB m_PassCB;
			Camera m_Camera;
		public:
			SpotLight();
			void SetInAngle(float angle);
			void SetOutAngle(float angle);
			void SetPosition(float x, float y, float z);
			void SetColor(float r, float g, float b);
			void SetIntensity(float i);
			void SetAttConstant1(float a);
			void SetAttConstant2(float a);
			void SetRange(float r);
			void SetRotation(float pitch, float yaw, float roll);
		private:
			virtual PassCB& GetPassCB(int slot = 0) override;
			virtual const DirectX::BoundingFrustum& GetFrustum(int slot = 0) override;
			void Update();
		};
	}
}