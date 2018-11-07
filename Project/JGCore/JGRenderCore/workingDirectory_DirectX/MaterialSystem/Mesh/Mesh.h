#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
#include"BufferSystem/JGBufferEnum.h"
#include"MaterialSystem/Data/ShaderData.h"

namespace JGRC
{
	/*
	버퍼 만드는 과정 다시 고치자..
	*/
	typedef unsigned int index;
	enum class MeshType
	{
		Default, Instance
	};
	class CORE_EXPORT Mesh
	{
		static class JGBufferManager* m_BfManager;
		static class DirectX* m_Dx;
		typedef std::vector<real> Data;
	private:
		std::string        m_Path;
		std::vector<index>  m_Stride;
		std::vector<index>  m_Offset;
		std::vector<index>  m_IndexData;
		std::vector<Data>  m_OuputData;
		std::vector<class JGBuffer*> m_vVertexBuffers;
		class JGBuffer* m_IndexBuffer = nullptr;
	public:
		Mesh();
		Mesh(const Mesh& copy);
		Mesh(Mesh&& mesh);
		~Mesh();
	public:
		Mesh& operator=(const Mesh& mesh);
		Mesh& operator=(Mesh&& mesh);
	public:
		void CreateBuffer(InputLayoutData* data);
		void Render(D3D_PRIMITIVE_TOPOLOGY set = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		uint getIndexCount();
	public:
		void CalculationTangentBinormal(jgVec3 vector[3],jgVec2 tex[3], jgVec3* normal ,jgVec3* tangent, jgVec3* binormal);

		// 임시임시임시
		bool LoadModel(const std::string& path,bool btangent = true);
		void CustomModel(const std::vector<real>& vertex, const uint count);
	};

}
