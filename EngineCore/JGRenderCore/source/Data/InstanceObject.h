#pragma once
#include"JGRCObject.h"

namespace JGRC
{
	class RCORE_EXPORT InstanceObject
	{
		static UINT Count;
	private:
		std::string m_Name = "None";
		JGRCObjDesc m_Desc = JGRCObjDesc();
		std::vector<std::shared_ptr<JGRCObject>>    m_InsObjects;
		std::unique_ptr<UploadBuffer<InstanceData>> InstanceCB;
		UINT  m_CBIndex = 0;
		UINT  m_NotCullingObjCount = 0;
		//
		class JGMesh* m_Mesh         = nullptr;
		std::string   m_MeshName = "None";
		class JGMaterial* m_Material = nullptr;
	public:
		InstanceObject();
		void Build(ID3D12GraphicsCommandList* CommandList);
		void Update(const GameTimer& gt, FrameResource* CurrFrameResource);
		void CubeDraw(FrameResource* CurrentFrameResource, ID3D12GraphicsCommandList* CommandList);
		void Draw(FrameResource* CurrentFrameResource, ID3D12GraphicsCommandList* CommandList, EObjRenderMode mode = EObjRenderMode::Default);
	public:
		void SetMesh(class JGMesh* mesh, const std::string& name);
		void SetMaterial(class JGMaterial* mat);
		JGRCObject* AddObject();
	};
}