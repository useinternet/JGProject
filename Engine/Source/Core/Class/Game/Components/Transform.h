#pragma once
#include "Class/Game/GameComponent.h"
#include "Class/Asset/Asset.h"


namespace JG
{
	class Transform : public GameComponent
	{
		JGCLASS
	private:
		enum SendDirtyFlags
		{
			SendDirtyFlag_LocationDirty = 0x0001,
			SendDirtyFlag_RoationDirty  = 0x0002,
			SendDirtyFlag_ScaleDirty    = 0x0004,


			SendDirtyFlag_All = 0xFFFFFFF,
		};

	private:
		JVector3 mLocation;
		JVector3 mRotation;
		JQuaternion mQuaternion;
		JVector3 mScale = { 1.0f,1.0f,1.0f };


		//JMatrix mLocalTranslateMatrix;
		//JMatrix mLocalRotationMatrix;
		//JMatrix mLocalScaleMatrix;


		//JMatrix mWorldTranslateMatrix;
		//JMatrix mWorldRotationMatrix;
		//JMatrix mWorldScaleMatrix;





		mutable JVector3 mWorldLocation;
		mutable JVector3 mWorldRotation;
		mutable JQuaternion mWorldQuaternion;
		mutable JVector3 mWorldScale;


		mutable JMatrix mWorldMatrix;
		mutable JMatrix mInvWorldMatrix;









		mutable bool mIsDirty = true;
		mutable bool mIsLocationDirty = true;
		mutable bool mIsRotationDirty = true;
		mutable bool mIsScaleDirty = true;


		mutable bool mIsInvDirty = true;
	public:
		void SetLocalLocation(const JVector3& location);
		void SetLocalRotation(const JVector3& rotation);
		void SetLocalScale(const JVector3& scale);
		void SetWorldLocation(const JVector3& location);
		void SetWorldRotation(const JVector3& rotation);
		void SetWorldScale(const JVector3& scale);


		void SetLocalQuaternion(const JQuaternion& q);
		void SetWorldQuaternion(const JQuaternion& q);


		const JVector3& GetLocalLocation() const;
		const JVector3& GetLocalRotation() const;
		const JVector3& GetLocalScale()    const;


		const JVector3& GetWorldLocation() const;
		const JVector3& GetWorldRotation() const;
		const JVector3& GetWorldScale() const;

		const JMatrix& GetWorldMatrix() const;
		const JMatrix& GetInvWorldMatrix() const;


		const JQuaternion& GetLocalQuaternion() const;
		const JQuaternion& GetWorldQuaternion() const;

		void Refresh();
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	private:
		void UpdateWorldMatrix()   const;
		void UpdateWorldLocation() const;
		void UpdateWorldRotation() const;
		void UpdateWorldScale()    const;






		void UpdateInvWorldMatrix() const;
		void NormalizeRotation(JVector3& toDegree) const;
	public:
		virtual void OnInspectorGUI() override;
	private:
		virtual void OnChange(const ChangeData& data) override;
		void SendDirty(SendDirtyFlags flags);
		void ApplyDirtyFlag(SendDirtyFlags flags);
	};
}