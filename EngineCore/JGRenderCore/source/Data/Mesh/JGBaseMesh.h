#pragma once
#include"DxCommon/DxCommon.h"






namespace JGRC
{
	enum class EMeshType
	{
		Static,
		Skeletal
	};
	class RCORE_EXPORT JGBaseMesh
	{
		
		JGBaseMesh(const JGBaseMesh& copy) = delete;
		JGBaseMesh& operator=(const JGBaseMesh& copy) = delete;
	protected:
		std::string m_MeshName;
		EMeshType   m_MeshType;
		std::vector<std::string>      m_MeshArgNames;
		std::unique_ptr<MeshGeometry> m_MeshData;
		UINT m_CurrStartIndexLocation = 0;
		UINT m_CurrBaseVertexLocation = 0;
	public:
		JGBaseMesh() = default;
		virtual ~JGBaseMesh() = default;
		virtual void CreateMesh(ID3D12GraphicsCommandList* CommandList) {}
		virtual void Update(const GameTimer& gt, FrameResource* CurrFrameResource) {}
		virtual void ArgDraw(const std::string& name, ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource, UINT Count = 1, D3D12_PRIMITIVE_TOPOLOGY TopolgyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		{
			CommandList->IASetVertexBuffers(0, 1, &m_MeshData->VertexBufferView());
			CommandList->IASetIndexBuffer(&m_MeshData->IndexBufferView());
			CommandList->IASetPrimitiveTopology(TopolgyType);
			CommandList->DrawIndexedInstanced(
				m_MeshData->DrawArgs[name].IndexCount,
				Count,
				m_MeshData->DrawArgs[name].StartIndexLocation,
				m_MeshData->DrawArgs[name].BaseVertexLocation,
				0);
		}
		virtual void Draw(ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource, UINT Count = 1, D3D12_PRIMITIVE_TOPOLOGY TopolgyType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		{
			CommandList->IASetVertexBuffers(0, 1, &m_MeshData->VertexBufferView());
			CommandList->IASetIndexBuffer(&m_MeshData->IndexBufferView());
			CommandList->IASetPrimitiveTopology(TopolgyType);
			for (auto& Arg : m_MeshData->DrawArgs)
			{
				CommandList->DrawIndexedInstanced(
					Arg.second.IndexCount,
					Count,
					Arg.second.StartIndexLocation,
					Arg.second.BaseVertexLocation,
					0);
			}
		}
	public:
		MeshGeometry* Data() { return m_MeshData.get(); }
		EMeshType     Type() { return m_MeshType; }
	public:
		const SubmeshGeometry& GetMeshArg_c(const std::string& name) const { return m_MeshData->DrawArgs[name]; }
		SubmeshGeometry&       GetMeshArg(const std::string& name)         { return m_MeshData->DrawArgs[name]; }
	public:
		const std::string& GetName() const                   { return m_MeshName; }
		const std::vector<std::string>& GetArgNames() const  { return m_MeshArgNames; }
		UINT  GetArgCount() const { return (UINT)m_MeshData->DrawArgs.size(); }
	};
}