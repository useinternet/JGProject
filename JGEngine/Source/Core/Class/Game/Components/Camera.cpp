#include "Camera.h"
#include "Camera.h"
#include "Camera.h"
#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "Class/Game/GameNode.h"

#include "Graphics/Resource.h"
namespace JG
{
	Camera* Camera::smMainCamera = nullptr;
	Camera* Camera::GetMainCamera()
	{
		return smMainCamera;
	}
	void Camera::SetMainCamera(Camera* mainCamera)
	{
		if (mainCamera == nullptr)
		{
			return;
		}
		smMainCamera = mainCamera;
		RequestRegisterMainCameraEvent e;
		e.MainCamera = mainCamera;
		mainCamera->SendEvent(e);
	}
	Camera::Camera()
	{
		SetFOV(90);
		SetNearZ(0.001f);
		SetFarZ(10000.0f);
		SetOrthographic(true);
		SetResolution(GameSettings::GetResolution());
	}
	void Camera::Awake()
	{
		if (smMainCamera == nullptr)
		{
			SetMainCamera(this);
		}
	}

	void Camera::Start()
	{

	}
	void Camera::Update()
	{
		if (smMainCamera == nullptr)
		{
			SetMainCamera(this);
		}
	}
	void Camera::Destory()
	{
		if (smMainCamera == this)
		{
			smMainCamera = nullptr;
			RequestUnRegisterMainCameraEvent e;
			e.MainCamera = this;
			SendEvent(e);
		}
	}
	void Camera::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		GameComponent::MakeJson(jsonData);
		jsonData->AddMember("Resolution", GetResolution());
		jsonData->AddMember("NearZ", GetNearZ());
		jsonData->AddMember("FarZ", GetFarZ());
		jsonData->AddMember("FOV", GetFOV());
		jsonData->AddMember("ClearColor", JVector4(GetClearColor()));
		jsonData->AddMember("CullingLayerMask", GetCullingLayerMask());
		jsonData->AddMember("IsOrthographic", IsOrthographic());

	}
	void Camera::LoadJson(SharedPtr<JsonData> jsonData)
	{
		GameComponent::LoadJson(jsonData);
		auto val = jsonData->GetMember("Resolution");
		if (val)
		{
			SetResolution(val->GetVector2());
		}
		val = jsonData->GetMember("NearZ");
		if (val)
		{
			SetNearZ(val->GetFloat());
		}
		val = jsonData->GetMember("FarZ");
		if (val )
		{
			SetFarZ(val->GetFloat());
		}
		val = jsonData->GetMember("FOV");
		if (val)
		{
			SetFOV(val->GetFloat());
		}
		val = jsonData->GetMember("ClearColor");
		if (val)
		{
			SetClearColor(val->GetVector4());
		}
		val = jsonData->GetMember("CullingLayerMask");
		if (val)
		{
			SetCullingLayerMask(val->GetUint64());
		}
		val = jsonData->GetMember("IsOrthographic");
		if (val)
		{
			SetOrthographic(val->GetBool());
		}
	}
	void Camera::SetFOV(f32 fov)
	{
		f32 convertFOV = Math::ConvertToRadians(fov);
		if (mFov != convertFOV)
		{
			mIsProjDirty = true;
		}
		mFov = convertFOV;
	}

	void Camera::SetFarZ(f32 farZ)
	{
		if (mFarZ != farZ)
		{
			mIsProjDirty = true;
		}
		mFarZ = farZ;
	}

	void Camera::SetNearZ(f32 nearZ)
	{
		if (mNearZ != nearZ)
		{
			mIsProjDirty = true;
		}
		mNearZ = nearZ;
	}

	void Camera::SetOrthographic(bool isOrthographic)
	{
		if (mIsOrthographic == isOrthographic)
		{
			return;
		}
		mIsProjDirty = true;
		mIsOrthographic = isOrthographic;
	}

	void Camera::SetResolution(const JVector2& resolution)
	{
		if (mResolution == resolution)
		{
			return;
		}
		mIsProjDirty = true;
		mResolution = resolution;
		mResolution.x = std::max<f32>(1.0f, mResolution.x);
		mResolution.y = std::max<f32>(1.0f, mResolution.y);
	}
	void Camera::SetRendererPath(ERendererPath rendererPath)
	{
		mRendererPath = rendererPath;
	}
	JVector3 Camera::ScreenToWorldPoint(const JVector3& screenPos) const
	{
		f32 farZ  = GetFarZ();
		f32 nearZ = GetNearZ();


		float z = (farZ + nearZ) / (2 * (farZ - nearZ)) + (1.0f / std::max<float>(screenPos.z, nearZ)) * ((-farZ * nearZ) / (farZ - nearZ)) + 0.5f;



		JVector3 result = JVector3::UnProject(JVector3(screenPos.x, screenPos.y, z),
			GetProjMatrix(), GetViewMatrix(),
			JMatrix::Identity(), JVector2(0, 0), GetResolution());


		return result;
	}
	JRay Camera::ScreenToWorldRay(const JVector3& screenPos) const
	{
		JRay result;
		result.dir = JVector3::UnProject(JVector3(screenPos.x, screenPos.y, 1.0f),
			GetProjMatrix(), GetViewMatrix(),
			JMatrix::Identity(), JVector2(0, 0), GetResolution());
		result.dir = JVector3::Normalize(result.dir);
		result.origin = GetOwner()->GetTransform()->GetWorldLocation();
		return result;
	}
	void Camera::SetCullingLayerMask(u64 mask)
	{
		mCullingLayerMask = mask;
	}
	void Camera::SetClearColor(const Color& color)
	{
		mClearColor = color;
	}
	const JMatrix& Camera::GetViewProjMatrix() const
	{
		UpdateView();
		UpdateProj();
		if (mIsViewProjDirty)
		{
			mViewProjMatrix = mViewMatrix * mProjMatrix;
			mIsViewProjDirty = false;
		}
		return mViewProjMatrix;
	}

	const JMatrix& Camera::GetViewMatrix() const
	{
		UpdateView();

		return mViewMatrix;
	}
	const JMatrix& Camera::GetInvViewMatrix() const
	{
		UpdateView();

		return mInvViewMatrix;
	}

	const JMatrix& Camera::GetProjMatrix() const
	{
		UpdateProj();

		return mProjMatrix;
	}

	f32 Camera::GetFOV() const
	{
		return Math::ConvertToDegrees(mFov);
	}

	f32 Camera::GetFarZ() const
	{
		return mFarZ;
	}

	f32 Camera::GetNearZ() const
	{
		return mNearZ;
	}

	JVector3 Camera::GetLook() const
	{
		auto rotation = GetOwner()->GetTransform()->GetWorldRotation();
		rotation = Math::ConvertToRadians(rotation);
		return JVector3::Normalize(JMatrix::Rotation(rotation).TransformVector(JVector3(0, 0, 1)));
	}

	JVector3 Camera::GetRight() const
	{
		auto rotation = GetOwner()->GetTransform()->GetWorldRotation();
		rotation = Math::ConvertToRadians(rotation);
		return JVector3::Normalize(JMatrix::Rotation(rotation).TransformVector(JVector3(1, 0, 0)));
	}

	JVector3 Camera::GetUp() const
	{
		auto rotation = GetOwner()->GetTransform()->GetWorldRotation();
		rotation = Math::ConvertToRadians(rotation);
		return JVector3::Normalize(JMatrix::Rotation(rotation).TransformVector(JVector3(0, 1, 0)));
	}

	bool Camera::IsOrthographic() const
	{
		return mIsOrthographic;
	}

	float Camera::GetAspectRatio() const
	{
		return mResolution.x / mResolution.y;
	}

	const JVector2& Camera::GetResolution() const
	{
		return mResolution;
	}
	ERendererPath Camera::GetRendererPath() const
	{
		return mRendererPath;
	}
	const Color& Camera::GetClearColor() const
	{
		return mClearColor;
	}
	u64 Camera::GetCullingLayerMask() const
	{
		return mCullingLayerMask;
	}
	void Camera::UpdateProj() const
	{
		if (mIsProjDirty == false)
		{
			return;
		}
		mIsProjDirty = false;


		if (mIsOrthographic)
		{
			mProjMatrix = JMatrix::OrthographicLH(mResolution.x, mResolution.y, mNearZ, mFarZ);
		}
		else
		{
			mProjMatrix = JMatrix::PerspectiveFovLH(mFov, GetAspectRatio(), mNearZ, mFarZ);
		}

		mIsViewProjDirty = true;
	}

	void Camera::UpdateView() const
	{
		if (mIsViewDirty == false)
		{
			return;
		}
		mIsViewDirty = false;



		JVector3 target = JVector3(0, 0, 1);
		auto rotation = GetOwner()->GetTransform()->GetWorldRotation();
		rotation = Math::ConvertToRadians(rotation);
		auto& location = GetOwner()->GetTransform()->GetWorldLocation();
		location.z -= GetNearZ();
		target = JMatrix::Rotation(rotation).TransformVector(target);
		mViewMatrix = JMatrix::LookAtLH(location, location + target, JVector3(0, 1, 0));
		mInvViewMatrix = JMatrix::Inverse(mViewMatrix);

		mIsViewProjDirty = true;
	}
	void Camera::OnChange(const ChangeData& data)
	{
		if (data.Type == JGTYPE(Transform))
		{
			mIsViewDirty = true;
		}
	}
	void Camera::OnInspectorGUI()
	{

		JVector2 resolution = GetResolution();
		f32 fov   = GetFOV();
		Color color = GetClearColor();
		f32 nearZ = GetNearZ();
		f32 farZ  = GetFarZ();
		bool isOrth = IsOrthographic();

		ImGui::OnGui("Resolution   ", &resolution);
		ImGui::OnGui("Field of View", &fov);
		ImGui::OnGui("NearZ        ", &nearZ);
		ImGui::OnGui("FarZ         ", &farZ);
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Orthographic  "); ImGui::SameLine();
		ImGui::Checkbox("##Orthographic Toggle", &isOrth);
		ImGui::AlignTextToFramePadding();
		ImGui::Text( "ClearColor    "); ImGui::SameLine();
		ImGui::ColorEdit4("## ClearColor Edit ", (float*)&color);


		SetFOV(fov);
		SetClearColor(color);
		SetFarZ(farZ);
		SetNearZ(nearZ);
		SetOrthographic(isOrth);
		SetResolution(resolution);
	}
	f32 EditorCamera::GetZoom() const
	{
		return mZoom;
	}
	void EditorCamera::SetZoom(f32 zoom)
	{
		zoom = std::max<f32>(0.01f, zoom);
		if (mZoom != zoom)
		{
			mIsProjDirty = true;
		}
		mZoom = zoom;
	}
	const JVector2& EditorCamera::GetFocusCenter() const
	{
		return mFocusCenter;
	}
	void EditorCamera::SetFocusCenter(const JVector2& focusCenter)
	{
		if (mFocusCenter != focusCenter)
		{
			mIsProjDirty = true;
		}
		mFocusCenter = focusCenter;
	}
	JRect EditorCamera::GetOrthograhicRect() const
	{
		f32 centerPosX = mOrthoRect.left + mOrthoRect.Width() * mFocusCenter.x;
		f32 centerPosY = mOrthoRect.top  - mOrthoRect.Height() * mFocusCenter.y;
		f32 zoomFactor = (1 / mZoom);
		f32 hw = mResolution.x * 0.5f * zoomFactor;
		f32 hh = mResolution.y * 0.5f * zoomFactor;

		f32 leftOffset = mFocusCenter.x * hw;
		f32 rightOffset = (1.0f - mFocusCenter.x) * hw;

		f32 topOffset = mFocusCenter.y * hh;
		f32 bottomOffset = (1.0f - mFocusCenter.y) * hh;
		// 0.75f

		mOrthoRect.left   = centerPosX - leftOffset;
		mOrthoRect.right  = centerPosX + rightOffset;
		mOrthoRect.top    = centerPosY + topOffset;
		mOrthoRect.bottom = centerPosY - bottomOffset;
		return mOrthoRect;
	}
	JRect EditorCamera::GetOrthograhicOriginRect() const
	{
		f32 hw = mResolution.x * 0.5f;
		f32 hh = mResolution.y * 0.5f;
		return JRect(-hw, hh, hw, -hh);
	}
	void EditorCamera::UpdateProj() const
	{
		if (mIsProjDirty == false)
		{
			return;
		}
		mIsProjDirty = false;


		if (mIsOrthographic)
		{
			auto orthoRect = GetOrthograhicRect();
			mProjMatrix = JMatrix::OrthographicOffCenterLH(orthoRect.left, orthoRect.right, orthoRect.bottom, orthoRect.top, mNearZ, mFarZ);
		}
		else
		{
			mProjMatrix = JMatrix::PerspectiveFovLH(mFov, GetAspectRatio(), mNearZ, mFarZ);
		}

		mIsViewProjDirty = true;
	}
}