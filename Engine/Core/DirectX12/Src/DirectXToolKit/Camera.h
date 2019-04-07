#pragma once
#include"DirectXCommon/DirectXCommon.h"
namespace Dx12
{
	class Camera
	{
		// VECTOR 갱신은 리셋 NO
		// POINT갱신은 리센 OK
	private:
		mutable Common::JVector3 m_Target;
		mutable Common::JVector3 m_Up;
		mutable Common::JVector3 m_Position;


		//
		Common::Rotation m_CameraRotator;
		mutable Common::JMatrix4x4 m_ViewMatrix;
		mutable Common::JMatrix4x4 m_ViewProjMatrix;
		Common::JMatrix4x4 m_ProjMatrix;
		mutable bool m_bUpdate = true;
	public:
		Camera();
		Camera(float fov, float width, float height, float nearZ = 1.0f, float farZ = 1000000.0f);
		void SetPosition(const Common::JVector3& v);
		void SetUp(const Common::JVector3& v);
		void SetTarget(const Common::JVector3& v);
		void RightMove(float speed);
		void FowardMove(float speed);
		void UpMove(float speed);

		void RotationPitch(float pitch);
		void RotationYaw(float yaw);
		void RotationRoll(float roll);
		void SetLens(float fov, float width, float height, float nearZ, float farZ);
		const Common::JMatrix4x4& GetMatrix() const;
		Common::JMatrix4x4 GetHlslMatrix() const;


		Common::JVector3 GetLookVector() const;
		Common::JVector3 GetRightVector() const;
		const Common::JVector3& GetPosition() const;
		const Common::JVector3& GetUp() const;
		const Common::JVector3& GetTarget() const;

		bool IsUpdate() const {
			return m_bUpdate;
		}
	private:
		void UpdateMatrix() const;
	};
}