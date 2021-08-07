#pragma once
#include "Class/Game/GameComponent.h"
#include "Math/JRect.h"


namespace JG
{
	class Camera : public GameComponent
	{
		static Camera* smMainCamera;
	public:
		static Camera* GetMainCamera();
		static void SetMainCamera(Camera* mainCamera);
	private:
		friend class GraphicsSystemLayer;
		JGCLASS
	protected:
		mutable bool mIsViewDirty = true;
		mutable bool mIsProjDirty = true;
		mutable bool mIsViewProjDirty = true;
	
		//
		mutable JMatrix  mProjMatrix;
		mutable JMatrix  mViewMatrix;
		mutable JMatrix  mInvViewMatrix;
		mutable JMatrix  mViewProjMatrix;
		//
		JVector2 mResolution;
		Color    mClearColor;
		//
		f32 mNearZ = 0.0f;
		f32 mFarZ  = 0.0f;
		f32 mFov   = 0.0f;
		//
		u64 mCullingLayerMask = JG_U64_MAX;
		//
		bool mIsOrthographic  = false;
		bool mIsMainCamera    = false;

		ERendererPath mRendererPath = ERendererPath::Foward;
	public:
		Camera();
	protected:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Update() override;
		virtual void Destory() override;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	public:
		void SetFOV(f32 fov);
		void SetFarZ(f32 farZ);
		void SetNearZ(f32 nearZ);

		void SetOrthographic(bool isOrthographic);
		void SetResolution(const JVector2& resolution);
		void SetCullingLayerMask(u64 mask);
		void SetClearColor(const Color& color);
		void SetRendererPath(ERendererPath rendererPath);

		JVector3 ScreenToWorldPoint(const JVector3& screenPos) const;
		JRay ScreenToWorldRay(const JVector3& screenPos) const;
	public:
		const JMatrix& GetViewProjMatrix() const;
		const JMatrix& GetViewMatrix() const;
		const JMatrix& GetInvViewMatrix() const;
		const JMatrix& GetProjMatrix() const;

		f32 GetFOV() const;
		f32 GetFarZ() const;
		f32 GetNearZ() const;

		JVector3 GetLook()  const;
		JVector3 GetRight() const;
		JVector3 GetUp()    const;

		bool IsOrthographic() const;
		float GetAspectRatio() const;
		const JVector2& GetResolution() const;
		const Color& GetClearColor() const;

		u64 GetCullingLayerMask() const;
		ERendererPath GetRendererPath() const;
	protected:
		virtual void UpdateProj() const;
		void UpdateView() const;
	public:
		virtual ~Camera() = default;
	protected:
		virtual void OnChange(const ChangeData& data) override;
		virtual void OnInspectorGUI() override;

	};

	class EditorCamera : public Camera
	{

		//float zoom = 2; // 200%
//float xPos = 0.75 * m_displaySize.Width; // where we are centred
//float yPos = 0.5 * m_displaySize.Height; // where we are centred
//float left = -m_displaySize.Width / (2 * zoom) + xPos;
//float right = m_displaySize.Width / (2 * zoom) + xPos;
//float top = -m_displaySize.Height / (2 * zoom) + yPos;
//float bottom = m_displaySize.Height / (2 * zoom) + yPos;
//m_projectionMatrix = Matrix.OrthoOffCenterLH(left, right, bottom, top, 0f, 100.0f);
//DirectX::XMMatrixOrthographicOffCenterLH()
	private:
		JGCLASS
		f32      mZoom = 1.0f;
		mutable JRect    mOrthoRect = JRect(-960, 540, 960, -540);
		JVector2 mFocusCenter = JVector2(0.5f, 0.5f);
	public:
		EditorCamera() : Camera() {}
	public:
		f32  GetZoom() const;
		void SetZoom(f32 zoom);

		const JVector2& GetFocusCenter() const;
		void SetFocusCenter(const JVector2& focusCenter);

		JRect GetOrthograhicRect() const;
		JRect GetOrthograhicOriginRect() const;
	private:
		virtual void UpdateProj() const override;
	};
}