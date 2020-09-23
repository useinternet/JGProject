#pragma once
#include "MeshComponent.h"




namespace GFW
{

	class StaticMeshComponent : public MeshComponent
	{


	public:
		virtual void Awake()   override;
		virtual void Start()   override;
		virtual void Tick()    override;
		virtual void Destroy() override;
	public:
		void SetMesh(const std::vector<MeshGenerator::StaticMeshData>& meshData);
	private:
		struct Instance
		{
			std::vector<GE::SceneObject*>              objects;
			std::vector<MeshGenerator::StaticMeshData> meshDatas;
			SDObject sdData;
		};
		
		Instance m_Instance;


	};



}