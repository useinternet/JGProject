#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
#include"BufferSystem/JGBufferEnum.h"

namespace JGRC
{
	/*
	버퍼 만드는 과정 다시 고치자..
	*/
	typedef unsigned int index;
	enum class CORE_EXPORT MeshType
	{
		Default, Instance
	};
	class CORE_EXPORT Mesh
	{
		static class DirectX* m_Dx;
		typedef std::vector<real> Data;
	private:
		std::string         m_Path;
		std::vector<index>  m_IndexData;
		Data                m_OutputData;
		class JGBuffer* m_VertexBuffer = nullptr;
		class JGBuffer* m_IndexBuffer = nullptr;
	public:
		Mesh();
		~Mesh();
	public:
		void CreateBuffer();
		void Render(D3D_PRIMITIVE_TOPOLOGY set = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		uint getIndexCount();
	public:
		void CalculationTangentBinormal(jgVec3 vector[3],jgVec2 tex[3], jgVec3* normal ,jgVec3* tangent, jgVec3* binormal);

		// 임시임시임시
		bool LoadModel(const std::string& path,bool btangent = true);
		void CustomModel(const std::vector<real>& vertex, const uint count);
	};

}
