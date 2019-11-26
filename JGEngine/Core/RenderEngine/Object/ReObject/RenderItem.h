#pragma once
#include "Object/ReObject.h"



namespace RE
{

	class ReMesh;
	class ReMaterial;
	class SBDStructuredBuffer;
	class STStruct;
	// RenderItem은 renderengine에 저장
	// 모든 Mesh, Material 은 에셋 매니저에서 관리
	// 모든 module, pass 정보도 에셋매니저에서 관리
	// 모든 텍스쳐, 뼈대 정보, 애니메이션 정보도 에셋매니저에서 관리
	class InstanceRenderItem;
	class RenderItem : public ReObject
	{
		
	public:
		RenderItem(const std::string& name);

		InstanceRenderItem* AddInstance();
		void RemoveInstance(InstanceRenderItem* instance);
	public:

		std::shared_ptr<ReMaterial>            Material;
		std::shared_ptr<ReMesh>                Mesh;
		//  
	

		std::shared_ptr<SBDStructuredBuffer>              StructuredBuffer;
		std::vector<std::shared_ptr<InstanceRenderItem>>  InstanceItems;
		std::unordered_map<InstanceRenderItem*, uint32_t> InstanceMapByPointer;




		//
		// pipelinestate (각 blend상태, depth상태, 레스터화기 상태)
		// material;
		   /*
		        1. 2가지 용 머터리얼이 있음 GUI 용, 3D용
				2. 기본적으로 모듈 타입별 기본 모듈이 들어있고
				3. 픽셀 코드만 관여 가능
		   */
		// mesh
		  /*
		     Mesh타입에따라 material이 3d용인지 gui용인지 판별

		  */
		// 좌표정보(위치, 회전, 스케일)
	};

	class InstanceRenderItem : public ReObject
	{
		friend RenderItem;
	public:
		InstanceRenderItem(const std::string& name) : ReObject(name) {}
	public:
		void Set(const std::string& name, const JMatrix& m);



		bool Get(const std::string& name, JMatrix& m);
	private:
		STStruct* Element;
	};


	enum class EReMeshType;

	class RenderItemManager
	{
	public:
		RenderItemManager();
		RenderItem* CreateItem(const std::string& name);
		void DeleteItem(RenderItem* item);
	public:
		std::vector<RenderItem*> GetAllItem();
		std::vector<RenderItem*> GetItemByMesh(EReMeshType type);
	private:
		std::unordered_map<RenderItem*, std::shared_ptr<RenderItem>> m_RenderItemPool;
	};
}