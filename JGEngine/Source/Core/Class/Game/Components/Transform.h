#pragma once
#include "Class/Game/GameComponent.h"



namespace JG
{
	class Transform : public GameComponent
	{
		JGCLASS
	private:
		JVector3 mLocation;
		JVector3 mRotation;
		JVector3 mScale = { 1.0f,1.0f,1.0f };


		mutable JMatrix mWorldMatrix;
		mutable JMatrix mInvWorldMatrix;
		mutable JMatrix mLocalMatrix;
		mutable bool mIsDirty = true;
		mutable bool mIsInvDirty = true;
	public:
		void SetLocalLocation(const JVector3& location);
		void SetLocalRotation(const JVector3& rotation);
		void SetScale(const JVector3& scale);


		const JVector3& GetLocalLocation() const;
		const JVector3& GetLocalRotation() const;
		JVector3 GetWorldLocation() const;
		JVector3 GetWorldRotation() const;
		const JVector3& GetScale() const;

		const JMatrix& GetWorldMatrix() const;
		const JMatrix& GetInvWorldMatrix() const;
	public:
		virtual void MakeJson(SharedPtr<JsonData> jsonData)   const override;
		virtual void LoadJson(SharedPtr<JsonData> jsonData) override;
	private:
		void UpdateWorldMatrix() const;
		void UpdateInvWorldMatrix() const;
		void CheckLimitRadian(JVector3& toRadian) const;
		void CheckLimitRotation(JVector3& toDegree);
	public:
		virtual void OnInspectorGUI() override;
	private:
		void SendDirty();
	};
}