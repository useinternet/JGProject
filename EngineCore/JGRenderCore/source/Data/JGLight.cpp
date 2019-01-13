#include"JGLight.h"
#include"PostProcessing/ShadowMap.h"
#include"Scene.h"
#include"CommonData.h"
using namespace std;
using namespace JGRC;
using namespace DirectX;
UINT JGLight::Count = 0;
float JGLight::maxCosAngle = cosf(XMConvertToRadians(60.0f));
float JGLight::minCosAngle = cosf(XMConvertToRadians(0.0f));
JGLight::JGLight(ELightType Type)
{
	m_Type = Type;
	m_ShadowMap = make_unique<ShadowMap>();
	m_Name += to_string(Count++);
	m_BasicDirection = m_Data.Direction;
}
void JGLight::Build()
{
	m_SceneMainPass = CommonData::_Scene()->GetMainPass();
	m_ShadowPass    = CommonData::_Scene()->AddPassData();

	switch (m_Type)
	{
	case ELightType::Direction:
	case ELightType::Spot:
		m_ShadowMap->BuildShadowMap(m_Name, EShadowMapType::Direction);
		break;
	case ELightType::Point:
		m_ShadowMap->BuildShadowMap(m_Name, EShadowMapType::Point);
		break;
	}
	m_Data.ShadowMapIndex = m_ShadowMap->GetSrvIndex();
}
void JGLight::Update(UINT LightCBIndex, FrameResource* CurrentResource)
{
	if (!m_Active)
		return;
	if (IsCanUpdate())
	{
		switch (m_Type)
		{
		case ELightType::Direction:
			Direction_Update();
			break;
		case ELightType::Point:
			Point_Update();
			break;
		case ELightType::Spot:
			Spot_Update();
			break;
		}
		m_ShadowMap->UpdateShadowPass(CurrentResource, this);
		auto LightCB = CurrentResource->LightCB.get();
		LightCB->CopyData(LightCBIndex, m_Data);
		UpdatePerFrame();
	}
}
void JGLight::DrawShadow(FrameResource* CurrentResource, ID3D12GraphicsCommandList* CommandList)
{
	if (!m_Active)
		return;
	m_ShadowMap->Draw(CurrentResource, CommandList);
}
void JGLight::SetLightColor(float r, float g, float b)
{
	m_Data.Strength = { r,g,b };
	ClearNotify();
}
void JGLight::SetLocation(float x, float y, float z)
{
	if (m_Type == ELightType::Direction)
		return;
	m_Data.Position = { x,y,z };
	ClearNotify();
}
void JGLight::SetDirection(float x, float y, float z)
{
	if (m_Type == ELightType::Point)
		return;
	m_BasicDirection = { x,y,z };
	ClearNotify();
}
void JGLight::SetFalloffStart(float x)
{
	if (m_Type == ELightType::Direction)
		return;
	m_Data.FalloffStart = x;
	ClearNotify();
}
void JGLight::SetFalloffEnd(float x)
{
	if (m_Type == ELightType::Direction)
		return;
	m_Data.FalloffEnd = x;
	ClearNotify();
}
void JGLight::SetSpotPower(float x)
{
	if (m_Type == ELightType::Direction)
		return;
	m_Data.SpotPower = x;
	ClearNotify();
}
void JGLight::SetOuterAngle(float angle)
{
	if (angle > 60.0f)
		angle = 60.0f;
	if (angle < 0)
		angle = 0;
	if (m_Type != ELightType::Spot)
		return;
	m_Data.CosOuterAngle = cosf(XMConvertToRadians(angle));
	ClearNotify();
}
void JGLight::SetInnerAngle(float angle)
{
	if (angle > 60.0f)
		angle = 60.0f;
	if (angle < 0)
		angle = 0;
	if (m_Type != ELightType::Spot)
		return;
	m_Data.CosInnerAngle = cosf(XMConvertToRadians(angle));
	ClearNotify();
}
void JGLight::SetRotation(float pitch, float yaw, float roll)
{
	m_Rotation = { pitch, yaw, roll };
	ClearNotify();
}
void JGLight::OffsetLightColor(float r, float g, float b)
{
	m_Data.Strength.x += r;
	m_Data.Strength.y += g;
	m_Data.Strength.z += b;
	ClearNotify();
}
void JGLight::OffsetLocation(float x, float y, float z)
{
	if (m_Type == ELightType::Direction)
		return;
	m_Data.Position.x += x;
	m_Data.Position.y += y;
	m_Data.Position.z += z;
	ClearNotify();
}
void JGLight::OffsetDirection(float x, float y, float z)
{
	if (m_Type == ELightType::Point)
		return;
	m_Data.Direction.x += x;
	m_Data.Direction.y += y;
	m_Data.Direction.z += z;
	ClearNotify();
}
void JGLight::OffsetFalloffStart(float x)
{
	if (m_Type == ELightType::Direction)
		return;
	m_Data.FalloffStart += x;
	ClearNotify();
}
void JGLight::OffsetFalloffEnd(float x)
{
	if (m_Type == ELightType::Direction)
		return;
	m_Data.FalloffEnd += x;
	ClearNotify();
}
void JGLight::OffsetSpotPower(float x)
{
	if (m_Type == ELightType::Direction)
		return;
	m_Data.SpotPower += x;
	ClearNotify();
}

void JGLight::OffsetOuterAngle(float angle)
{
	if (m_Type != ELightType::Spot)
		return;

	float cosAngle = m_Data.CosOuterAngle - cosf(XMConvertToRadians(angle));
	if (cosAngle < maxCosAngle)
		cosAngle = maxCosAngle;

	m_Data.CosOuterAngle = cosAngle;
	ClearNotify();
}
void JGLight::OffsetInnerAngle(float angle)
{
	if (m_Type != ELightType::Spot)
		return;

	float cosAngle = m_Data.CosInnerAngle - cosf(XMConvertToRadians(angle));


	if (m_Data.CosOuterAngle > cosAngle)
		cosAngle = m_Data.CosOuterAngle;

	m_Data.CosInnerAngle = cosAngle;
	ClearNotify();
}
void JGLight::OffsetRotation(float pitch, float yaw, float roll)
{
	m_Rotation.x += pitch;
	m_Rotation.y += yaw;
	m_Rotation.z += roll;
	ClearNotify();
}

void JGLight::Direction_Update()
{
	BoundingSphere SceneSphere = CommonData::_Scene()->GetSceneSphere();
	// 방향 벡터
	XMVECTOR lightDir = XMLoadFloat3(&m_BasicDirection);
	// 회전 행렬 생성
	XMMATRIX R = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_Rotation.x),
		XMConvertToRadians(m_Rotation.y),
		XMConvertToRadians(m_Rotation.z));
	// 방향 라이트는 w성분을 0으로 하고 행렬 계산 후 데이터에 저장
	lightDir = XMVector3TransformNormal(lightDir, R);
	XMStoreFloat3(&m_Data.Direction, lightDir);

	// 방향 라이트 위치 계산
	XMVECTOR lightPos = -2.0f * SceneSphere.Radius * lightDir;
	XMVECTOR targetPos = XMLoadFloat3(&SceneSphere.Center);
	XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX lightView = XMMatrixLookAtLH(lightPos, targetPos, lightUp);

	XMStoreFloat3(&m_Data.Position, lightPos);

	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, lightView));
	// 왼쪽 / 밑 / near
	float l = sphereCenterLS.x - SceneSphere.Radius;
	float b = sphereCenterLS.y - SceneSphere.Radius;
	float n = sphereCenterLS.z - SceneSphere.Radius;
	// 오른쪽 / 탑 / far
	float r = sphereCenterLS.x + SceneSphere.Radius;
	float t = sphereCenterLS.y + SceneSphere.Radius;
	float f = sphereCenterLS.z + SceneSphere.Radius;

	XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	XMMATRIX S = lightView * lightProj * T;
	XMStoreFloat4x4(&m_ViewMatrix, lightView);
	XMStoreFloat4x4(&m_ProjMatrix, lightProj);
	XMStoreFloat4x4(&m_Data.ShadowTransform, XMMatrixTranspose(S));
}
void JGLight::Point_Update()
{
	Point_BuildCamera();
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PI *0.5f,
		1.0f, LIGHT_NEARZ, m_Data.FalloffEnd);
	XMStoreFloat4x4(&m_Data.ShadowTransform,XMMatrixTranspose(proj));
}
void JGLight::Point_BuildCamera()
{
	XMFLOAT3 center = m_Data.Position;
	float x = center.x; float y = center.y; float z = center.z;
	XMFLOAT3 worldUp(0.0f, 1.0f, 0.0f);

	// Look along each coordinate axis.
	XMFLOAT3 targets[6] =
	{
		XMFLOAT3(x + 1.0f, y, z), // +X
		XMFLOAT3(x - 1.0f, y, z), // -X
		XMFLOAT3(x, y + 1.0f, z), // +Y
		XMFLOAT3(x, y - 1.0f, z), // -Y
		XMFLOAT3(x, y, z + 1.0f), // +Z
		XMFLOAT3(x, y, z - 1.0f)  // -Z
	};

	// Use world up vector (0,1,0) for all directions except +Y/-Y.  In these cases, we
	// are looking down +Y or -Y, so we need a different "up" vector.
	XMFLOAT3 ups[6] =
	{
		XMFLOAT3(0.0f, 1.0f, 0.0f),  // +X
		XMFLOAT3(0.0f, 1.0f, 0.0f),  // -X
		XMFLOAT3(0.0f, 0.0f, -1.0f), // +Y
		XMFLOAT3(0.0f, 0.0f, +1.0f), // -Y
		XMFLOAT3(0.0f, 1.0f, 0.0f),	 // +Z
		XMFLOAT3(0.0f, 1.0f, 0.0f)	 // -Z
	};
	for (int i = 0; i < 6; ++i)
	{
		m_PointCamera[i].LookAt(center, targets[i], ups[i]);
		m_PointCamera[i].SetLens(0.5f*XM_PI, 1.0f, LIGHT_NEARZ, m_Data.FalloffEnd);
		m_PointCamera[i].UpdateViewMatrix();
	}
}
void JGLight::Spot_Update()
{

	XMVECTOR lightDir = XMLoadFloat3(&m_BasicDirection);
	XMMATRIX R = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_Rotation.x),
		XMConvertToRadians(m_Rotation.y),
		XMConvertToRadians(m_Rotation.z));
	lightDir = XMVector3TransformNormal(lightDir, R);
	XMStoreFloat3(&m_Data.Direction, lightDir);

	XMFLOAT3 Up;
	if (m_Data.Direction.y > 0.9f || m_Data.Direction.y < -0.9f)
	{
		Up = { 0.0f,0.0f,m_Data.Direction.y };
	}
	else
	{
		Up = { 0.0f,1.0f,0.0f };
	}
	float range = m_Data.FalloffEnd - m_Data.FalloffStart;
	XMVECTOR pos = XMLoadFloat3(&m_Data.Position);
	XMVECTOR TargetPos = lightDir + pos;
	XMFLOAT3 targetpos;
	XMStoreFloat3(&targetpos, TargetPos);

	m_SpotCamera.SetLens(SPOTLIGHT_MAX_ANGLE, 1.0f, LIGHT_NEARZ,
		m_Data.FalloffEnd);


	m_SpotCamera.LookAt(m_Data.Position, targetpos, Up);
	m_SpotCamera.UpdateViewMatrix();
	m_ProjMatrix = m_SpotCamera.GetProj4x4f();
	m_ViewMatrix = m_SpotCamera.GetView4x4f();


	XMMATRIX view = XMLoadFloat4x4(&m_ViewMatrix);
	XMMATRIX proj = XMLoadFloat4x4(&m_ProjMatrix);
	XMMATRIX ShadowTrans = XMMatrixMultiply(view, proj);
	XMStoreFloat4x4(&m_Data.ShadowTransform, XMMatrixTranspose(ShadowTrans));
}