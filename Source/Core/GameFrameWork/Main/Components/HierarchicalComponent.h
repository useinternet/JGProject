#pragma once
#include "GameObjectComponent.h"


namespace GFW
{
	/*
	1. 계층 구조를 가진 컴포넌트
	2. 위치를 가지고있다.
	3. 부모 자식 관계를 가지고 있다.
	
	*/

	class HierarchicalComponent : public GameObjectComponent
	{
		friend class GameObject;
	public:
		virtual void Awake()   override;
		virtual void Start()   override;
		virtual void Tick()    override;
		virtual void Destroy() override;
	public:
		void SetParent(HierarchicalComponent* p);
	public:
		void SetLocalLocation(const JVector3& l);
		void SetLocalRotation(const JVector3& r);
		void SetLocalScale(const JVector3& scale);
		void SetWorldLocation(const JVector3& l);
		void SetWorldRotation(const JVector3& r);
		void SetWorldScale(const JVector3& s);


		const JVector3&    GetLocalLocation() const;
		const JQuaternion& GetLocalRotation() const;
		const JVector3&    GetLocalScale() const;

		const JVector3&    GetWorldLocation() const;
		const JQuaternion& GetWorldRotation() const;
		const JVector3&    GetWorldScale()    const;
	protected:
		const JMatrix& GetWorldMatrix() const;
		void SendDirty(int Dirty);
	private:
		enum
		{
			Dirty_Matrix,
			Dirty_Location,
			Dirty_Rotation,
			Dirty_Scale,
			Dirty_Count
		};
		mutable JVector3    m_WorldLocation;
		mutable JQuaternion m_WorldRotation = JQuaternion::ToQuaternion(0, 0, 0);;
		mutable JVector3    m_WorldScale    = { 1.0f,1.0f,1.0f };


		JVector3    m_LocalLocation;
		JQuaternion m_LocalRotation = JQuaternion::ToQuaternion(0,0,0);
		JVector3    m_LocalScale    = { 1.0f,1.0f,1.0f };

		mutable bool m_IsDirty[Dirty_Count];
		mutable JMatrix m_WorldMatrix = JMatrix::Identity();


		std::vector<HierarchicalComponent*> m_Childs;
		HierarchicalComponent* m_Parent = nullptr;
	};


}

