#pragma once
#include"DxCommon/DxCommon.h"
#define LIGHT_NEARZ 1.0f
#define SPOTLIGHT_MAX_ANGLE XM_PI * 0.66f
namespace JGRC
{

	enum class ELightType
	{
		Direction,
		Point,
		Spot
	};
	class RCORE_EXPORT JGLight
	{
		static UINT Count;
		static float maxCosAngle;
		static float minCosAngle;
	private:// 공통
		ELightType m_Type;
		std::string m_Name = "Light";
		Light m_Data;
		DirectX::XMFLOAT3 m_Rotation;
		std::shared_ptr<class ShadowMap> m_ShadowMap;
		PassData* m_SceneMainPass = nullptr;
		PassData* m_ShadowPass    = nullptr;
		DirectX::XMFLOAT4X4 m_ProjMatrix = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 m_ViewMatrix = MathHelper::Identity4x4();
		DirectX::XMFLOAT3   m_BasicDirection;

		bool m_Active = true;
		int UpdateNotify = CPU_FRAMERESOURCE_NUM;
		// 개별 변수
		Camera m_SpotCamera;
		Camera m_PointCamera[6];
	public:
		JGLight(ELightType Type);
		~JGLight() = default;
	public:
		void Build();
		void Update(UINT LightCBIndex, FrameResource* CurrentResource);
		void DrawShadow(FrameResource* CurrentResource, ID3D12GraphicsCommandList* CommandList);
	public:
		const DirectX::XMFLOAT3& GetLightColor() const { return m_Data.Strength; }
		const DirectX::XMFLOAT3& GetLocation()   const { return m_Data.Position; }
		const DirectX::XMFLOAT3& GetDirection()  const { return m_BasicDirection; }
		const float GetFalloffStart() const  { return m_Data.FalloffStart; }
		const float GetFalloffEnd()   const  { return m_Data.FalloffEnd; }
		const float GetSpotPower()    const  { return m_Data.SpotPower; }

		bool IsActive() const                { return m_Active; }
		void DeActive()  { m_Active = false; }
		void Active()    { m_Active = true; }
	public:
		void SetLightColor(float r, float g, float b);
		void SetLocation(float x, float y, float z);
		void SetDirection(float x, float y, float z);
		void SetFalloffStart(float x);
		void SetFalloffEnd(float x);
		void SetSpotPower(float x);
		void SetOuterAngle(float angle);
		void SetInnerAngle(float angle);
		void SetRotation(float pitch, float yaw, float roll);
	public:
		void OffsetLightColor(float r, float g, float b);
		void OffsetLocation(float x, float y, float z);
		void OffsetDirection(float x, float y, float z);
		void OffsetFalloffStart(float x);
		void OffsetFalloffEnd(float x);
		void OffsetSpotPower(float x);
		void OffsetOuterAngle(float angle);
		void OffsetInnerAngle(float angle);
		void OffsetRotation(float pitch, float yaw, float roll);
	public:
		const DirectX::XMFLOAT4X4& GetViewMatrix() const { return m_ViewMatrix; }
		const DirectX::XMFLOAT4X4& GetProjMatrix() const { return m_ProjMatrix; }
		Camera& GetSpotLightCamera()                     { return m_SpotCamera; }
		Camera& GetPointLightCamera(UINT idx)            { return m_PointCamera[idx]; }
	private: // 프레임 알림 함수들
		void ClearNotify()    { UpdateNotify = CPU_FRAMERESOURCE_NUM; }
		void UpdatePerFrame() { UpdateNotify--; }
		bool IsCanUpdate()    { return UpdateNotify > 0; }
	private:
		void Direction_Update();
		void Point_Update();
		void Point_BuildCamera();
		void Spot_Update();
	};
}