#include"JGLight.h"
#include"PostProcessing/ShadowMap.h"
#include"Scene.h"
#include"CommonData.h"
using namespace std;
using namespace JGRC;
using namespace DirectX;
UINT JGLight::Count = 0;
float JGLight::maxAngle = 60.0f;
float JGLight::minAngle = 0.0f;
JGLight::JGLight(ELightType Type, ELightExercise ExType)
{
	m_LightType    = Type;
	m_LightExcType = ExType;
	m_Name += to_string(Count++);
	m_Direction = m_Data.Direction;
}
void JGLight::Build()
{
	m_bInit = true;
	m_StaticShadowMap  = make_unique<ShadowMap>();
	m_DynamicShadowMap = make_unique<ShadowMap>();
	switch (m_LightType)
	{
	case ELightType::Direction:
	case ELightType::Spot:
		m_StaticShadowMap->BuildShadowMap(m_Name  + "Static", EShadowMapType::Texture);
		m_DynamicShadowMap->BuildShadowMap(m_Name + "Dyanmic", EShadowMapType::Texture);
		break;
	case ELightType::Point:
		m_StaticShadowMap->BuildShadowMap(m_Name  + "Static", EShadowMapType::Cube);
		m_DynamicShadowMap->BuildShadowMap(m_Name + "Dyanmic", EShadowMapType::Cube);
		break;
	}

	m_Data.StaticShadowMapIndex = m_StaticShadowMap->GetSrvIndex();
	m_Data.DynamicShadowMapIndex = m_DynamicShadowMap->GetSrvIndex();
}
void JGLight::Update(UINT LightCBIndex, FrameResource* CurrentResource)
{
	if (!m_Active)
		return;
	if (IsCanUpdate())
	{
		switch (m_LightType)
		{
		case ELightType::Direction:
			Direction_Update();
			break;
		case ELightType::Point:
			Point_Update();
			break;
		case ELightType::Spot:
			Spot_Update();
			m_Data.CosInnerAngle = cosf(XMConvertToRadians(m_InnerAngle));
			m_Data.CosOuterAngle = cosf(XMConvertToRadians(m_OuterAngle));
			break;
		}
		m_StaticShadowMap->UpdateShadowPass(CurrentResource, this);
		m_DynamicShadowMap->UpdateShadowPass(CurrentResource, this);
		auto LightCB = CurrentResource->LightCB.get();
		LightCB->CopyData(LightCBIndex, m_Data);
		UpdatePerFrame();
	}
}
void JGLight::BuildShadowMap(FrameResource* CurrentResource, ID3D12GraphicsCommandList* CommandList)
{
	if (!m_Active)
		return;

	m_DynamicShadowMap->Draw(CurrentResource, CommandList, ELightExercise::Dynamic);

	if (m_LightExcType == ELightExercise::Static && m_bDraw)
		return;
	m_StaticShadowMap->Draw(CurrentResource, CommandList, ELightExercise::Static);
	m_bDraw = true;
}
void JGLight::SetLightColor(float r, float g, float b)
{
	m_Data.Strength = { r,g,b };
	ClearNotify();
}
void JGLight::SetLocation(float x, float y, float z)
{
	if (m_bInit && m_LightExcType == ELightExercise::Static)
		return;
	if (m_LightType == ELightType::Direction)
		return;

	m_Data.Position = { x,y,z };
	ClearNotify();
}
void JGLight::SetDirection(float x, float y, float z)
{
	if (m_bInit && m_LightExcType == ELightExercise::Static)
		return;
	if (m_LightType == ELightType::Point)
		return;

	m_Direction = { x,y,z };
	ClearNotify();
}
void JGLight::SetFalloffStart(float x)
{
	if (m_bInit && m_LightExcType == ELightExercise::Static)
		return;
	if (m_LightType == ELightType::Direction)
		return;

	m_Data.FalloffStart = x;
	ClearNotify();
}
void JGLight::SetFalloffEnd(float x)
{
	if (m_bInit && m_LightExcType == ELightExercise::Static)
		return;
	if (m_LightType == ELightType::Direction)
		return;

	m_Data.FalloffEnd = x;
	ClearNotify();
}
void JGLight::SetSpotPower(float x)
{
	if (m_bInit && m_LightExcType == ELightExercise::Static)
		return;
	if (m_LightType == ELightType::Direction)
		return;

	m_Data.SpotPower = x;
	ClearNotify();
}
void JGLight::SetOuterAngle(float angle)
{
	if (m_bInit && m_LightExcType == ELightExercise::Static)
		return;
	if (angle > 60.0f)
		angle = 60.0f;
	if (angle < 0)
		angle = 0;
	if (m_LightType != ELightType::Spot)
		return;

	m_OuterAngle = angle;
	ClearNotify();
}
void JGLight::SetInnerAngle(float angle)
{
	if (m_bInit && m_LightExcType == ELightExercise::Static)
		return;
	if (angle > 60.0f)
		angle = 60.0f;
	if (angle < 0)
		angle = 0;
	if (m_LightType != ELightType::Spot)
		return;
	m_InnerAngle = angle;
	ClearNotify();
}
void JGLight::SetRotation(float pitch, float yaw, float roll)
{
	if (m_bInit && m_LightExcType == ELightExercise::Static)
		return;
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
	if (m_bInit && m_LightExcType == ELightExercise::Static)
		return;
	if (m_LightType == ELightType::Direction)
		return;
	m_Data.Position.x += x;
	m_Data.Position.y += y;
	m_Data.Position.z += z;
	ClearNotify();
}
void JGLight::OffsetDirection(float x, float y, float z)
{
	if (m_bInit && m_LightExcType == ELightExercise::Static)
		return;
	if (m_LightType == ELightType::Point)
		return;
	m_Direction.x += x;
	m_Direction.y += y;
	m_Direction.z += z;
	ClearNotify();
}
void JGLight::OffsetFalloffStart(float x)
{
	if (m_bInit && m_LightExcType == ELightExercise::Static)
		return;
	if (m_LightType == ELightType::Direction)
		return;
	m_Data.FalloffStart += x;
	ClearNotify();
}
void JGLight::OffsetFalloffEnd(float x)
{
	if (m_bInit && m_LightExcType == ELightExercise::Static)
		return;
	if (m_LightType == ELightType::Direction)
		return;
	m_Data.FalloffEnd += x;
	ClearNotify();
}
void JGLight::OffsetSpotPower(float x)
{
	if (m_bInit && m_LightExcType == ELightExercise::Static)
		return;
	if (m_LightType == ELightType::Direction)
		return;
	m_Data.SpotPower += x;
	ClearNotify();
}

void JGLight::OffsetOuterAngle(float angle)
{
	if (m_bInit && m_LightExcType == ELightExercise::Static)
		return;
	if (m_LightType != ELightType::Spot)
		return;

	m_OuterAngle += angle;

	if (m_OuterAngle > 60.0f)
		m_OuterAngle = 60.0f;
	if (m_OuterAngle < 0)
		m_OuterAngle = 0;
	ClearNotify();
}
void JGLight::OffsetInnerAngle(float angle)
{
	if (m_bInit && m_LightExcType == ELightExercise::Static)
		return;
	if (m_LightType != ELightType::Spot)
		return;

	m_InnerAngle += angle;

	if (m_InnerAngle > m_OuterAngle)
		m_InnerAngle = m_OuterAngle;
	if (m_InnerAngle < 0)
		m_InnerAngle = 0;

	ClearNotify();
}
void JGLight::OffsetRotation(float pitch, float yaw, float roll)
{
	if (m_bInit && m_LightExcType == ELightExercise::Static)
		return;
	m_Rotation.x += pitch;
	m_Rotation.y += yaw;
	m_Rotation.z += roll;
	ClearNotify();
}

void JGLight::Direction_Update()
{
	BoundingSphere SceneSphere = CommonData::_Scene()->GetSceneSphere();

	// 방향 벡터
	XMVECTOR lightDir = XMLoadFloat3(&m_Direction);
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
	XMVECTOR lightDir = XMLoadFloat3(&m_Direction);
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