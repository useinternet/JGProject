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
	enum class ELightExercise
	{
		Static,
		Dynamic
	};
	class RCORE_EXPORT JGLight
	{
		// 총 라이트 갯수
		static UINT Count;
		// 최대 Angle
		static float maxAngle;
		static float minAngle;
	private:  // 공통
		Light m_Data;
		ELightType     m_LightType;
		ELightExercise m_LightExcType;
		bool m_bInit  = false;
		bool m_bDraw  = false;
		bool m_On     = true;   // 빛 On / Off
		bool m_Active = true;   // 빛 활성화 여부 (활성화 false이면 추상적으로 삭제)
		int UpdateNotify = CPU_FRAMERESOURCE_NUM;
	private: // 라이트 데이터
		std::string m_Name = "Light";
		DirectX::XMFLOAT3 m_Rotation  = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT3 m_Direction = { 0.0f,0.0f,0.0f };
		float             m_OuterAngle = 60.0f;
		float             m_InnerAngle = 30.0f;
	private: // 
		DirectX::XMFLOAT4X4 m_ProjMatrix = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 m_ViewMatrix = MathHelper::Identity4x4();
	private:
		Camera m_SpotCamera;
		Camera m_PointCamera[6];
		std::shared_ptr<class ShadowMap> m_StaticShadowMap;
		std::shared_ptr<class ShadowMap> m_DynamicShadowMap;
	public:
		JGLight(ELightType Type, ELightExercise ExType);
		~JGLight() = default;
	public:
		void Build();
		void Update(UINT LightCBIndex, FrameResource* CurrentResource);
		void BuildShadowMap(FrameResource* CurrentResource, ID3D12GraphicsCommandList* CommandList);
	public:
		const DirectX::XMFLOAT3& GetLightColor() const { return m_Data.Strength; }
		const DirectX::XMFLOAT3& GetLocation()   const { return m_Data.Position; }
		const DirectX::XMFLOAT3& GetDirection()  const { return m_Direction; }
		const float GetFalloffStart() const  { return m_Data.FalloffStart; }
		const float GetFalloffEnd()   const  { return m_Data.FalloffEnd; }
		const float GetSpotPower()    const  { return m_Data.SpotPower; }

		bool IsActive() const                { return m_Active; }
		void DeActive()  { m_Active = false; }
		void Active()    { m_Active = true; }
		ELightType GetLightType() const { return m_LightType; }
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
		void ClearNotify()    { UpdateNotify = CPU_FRAMERESOURCE_NUM;}
		void UpdatePerFrame() { UpdateNotify--; }
		bool IsCanUpdate()    { return UpdateNotify > 0; }
	private:
		void Direction_Update();
		void Point_Update();
		void Point_BuildCamera();
		void Spot_Update();
	};
}