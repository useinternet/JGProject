#include "pch.h"
#include "HierarchicalComponent.h"
#include "GFWClasses/GameObject.h"

using namespace std;

namespace GFW
{
	void HierarchicalComponent::Awake()
	{
		GameObjectComponent::Awake();
		for (int i = 0; i < Dirty_Count; ++i)
		{
			m_IsDirty[i] = false;
		}
	}
	void HierarchicalComponent::Start()
	{
		GameObjectComponent::Start();
	}
	void HierarchicalComponent::Tick()
	{
		GameObjectComponent::Tick();

		auto childs = m_Childs;

		for (auto& child : childs)
		{
			child->Tick();
		}

	}
	void HierarchicalComponent::Destroy()
	{
		GameObjectComponent::Destroy();



	}

	void HierarchicalComponent::SetParent(HierarchicalComponent* p)
	{
		if (p == nullptr && this != GetOwner()->GetRootComponent()) m_Parent = GetOwner()->GetRootComponent();


		if (m_Parent)
		{
			//
			auto iter = find(m_Parent->m_Childs.begin(), m_Parent->m_Childs.end(), p);
			m_Parent->m_Childs.erase(iter);
		}


		m_Parent = p;
		if (m_Parent)
		{
			m_Parent->m_Childs.push_back(this);
		}

		SendDirty(Dirty_Count);
	}



	void HierarchicalComponent::SetLocalLocation(const JVector3& l)
	{
		m_LocalLocation = l;
		SendDirty(Dirty_Location);
	}
	void HierarchicalComponent::SetLocalRotation(const JVector3& r)
	{
		m_LocalRotation = JQuaternion::ToQuaternion(r);
		SendDirty(Dirty_Rotation);
	}
	void HierarchicalComponent::SetLocalScale(const JVector3& scale)
	{
		m_LocalScale = scale;
		SendDirty(Dirty_Scale);
	}
	void  HierarchicalComponent::SetWorldLocation(const JVector3& l)
	{
		SetLocalLocation(GetLocalLocation() + l - GetWorldLocation());
	}
	void  HierarchicalComponent::SetWorldRotation(const JVector3& r)
	{
		assert(true, "HierarchicalComponent::SetWorldRotation not imp");
		//JVector3 localEuler = JQuaternion::ToEuler(GetLocalRotation());
		//JVector3 worldEuler = JQuaternion::ToEuler(GetWorldRotation());
		//SetLocalRotation(localEuler + r - worldEuler);
	}
	void  HierarchicalComponent::SetWorldScale(const JVector3& s)
	{
		SetLocalScale(GetLocalScale() + s - GetWorldScale());
	}
	const JVector3& HierarchicalComponent::GetLocalLocation() const
	{
		return m_LocalLocation;
	}
	const JQuaternion& HierarchicalComponent::GetLocalRotation() const
	{
		return m_LocalRotation;
	}
	const JVector3& HierarchicalComponent::GetLocalScale() const
	{
		return m_LocalScale;
	}

	const JVector3&    HierarchicalComponent::GetWorldLocation() const
	{
		if (m_IsDirty[Dirty_Location])
		{
			m_WorldLocation = m_LocalLocation;
			if (m_Parent)
			{
				m_WorldLocation += m_Parent->GetWorldLocation();
			}
		}
		return m_WorldLocation;
	}
	const JQuaternion& HierarchicalComponent::GetWorldRotation() const
	{
		assert(true, "HierarchicalComponent::GetWorldRotation not imp");
		//if (m_IsDirty[Dirty_Rotation])
		//{
		//	m_WorldRotation = m_LocalRotation;
		//	if (m_Parent)
		//	{
		//		JVector3 eulerAngle = JQuaternion::ToEuler(m_LocalRotation) + JQuaternion::ToEuler(m_Parent->GetWorldRotation());
		//		m_WorldRotation = JQuaternion::ToQuaternion(eulerAngle);
		//	}
		//}
		return m_WorldRotation;
	}
	const JVector3&    HierarchicalComponent::GetWorldScale()    const
	{
		if (m_IsDirty[Dirty_Scale])
		{
			m_WorldScale = m_LocalScale;
			if (m_Parent)
			{
				m_WorldScale += m_Parent->GetWorldScale();
			}
		}
		return m_WorldScale;
	}
	const JMatrix& HierarchicalComponent::GetWorldMatrix() const
	{
		if(m_IsDirty[Dirty_Matrix])
		{
			m_IsDirty[Dirty_Matrix] = false;
			JMatrix R = JMatrix::Rotation(GetWorldRotation());
			JMatrix T = JMatrix::Translation(GetWorldLocation());
			JMatrix S = JMatrix::Scaling(GetWorldScale());

			m_WorldMatrix = JMatrix::Transpose(R * T * S);
		}
		return m_WorldMatrix;
	}
	void HierarchicalComponent::SendDirty(int dirty)
	{
		if (dirty == Dirty_Count)
		{
			for (int i = 0; i < Dirty_Count; ++i)
			{
				m_IsDirty[i] = true;
			}
		}
		else
		{
			m_IsDirty[Dirty_Matrix] = true;
			m_IsDirty[dirty] = true;
		}

		for (auto& child : m_Childs)
		{
			child->SendDirty(dirty);
		}
	}
}