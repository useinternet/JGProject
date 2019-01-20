#pragma once
#include"DxCommon/DxCommon.h"


namespace JGRC
{
	// 컬링 안된 노드들만 오브젝트 Array 가져와서 Draw
	// 각 노드들은 BoundingBox를 가지고 있다.
	// 절두체를 이용해 박스 분류( BoundingFrustom )
	//  BoundingBox::Contain 활용 <- 할때 물체의 좌표를 국소 좌표로 이동
	// 통과된 오브젝트만 그린다.

	class RCORE_EXPORT OctreeNode
	{
	private:
		OctreeNode(const OctreeNode& rhs)            = delete;
		OctreeNode& operator=(const OctreeNode& rhs) = delete;
	private:
		std::vector<class JGRCObject*> m_StaticObjects;
		std::list<class JGRCObject*>   m_DynamicObjects;
		std::vector<OctreeNode*> m_ChildeNodes;
		DirectX::BoundingBox     m_BoundBox;
		OctreeNode*   m_ParentNode = nullptr;
		bool bCulling = false;
	public:
		void BuildOctree(UINT vCenter, UINT vHalfWidth, UINT Level);
		void DistributeObject(std::vector<class JGRCObject*>& ObjArr);
		bool IsIncludeNode(Camera* MainCamera);
		std::vector<JGRCObject*>& GetArray();
	};


}