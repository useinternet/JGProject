#pragma once


#include "Object/ReObject.h"
#include <vector>
namespace RE
{
	class CommandList;
	enum class RENDERENGINE_API EReMeshType
	{
		Static,
		Skeletal,
		GUI
	};
	enum class RENDERENGINE_API EReMeshShape
	{
		Box
	};
	class RENDERENGINE_API ReMesh : public ReObject
	{

	protected:
		using VertexData = std::vector<byte>;
		using IndexData  = std::vector<uint32_t>;
	public:
		ReMesh(EReMeshType type) : ReObject("ReMesh"), m_MeshType(type) {}
		ReMesh(const std::string& name, EReMeshType type) : ReObject(name), m_MeshType(type) {}
	public:


		uint32_t GetTypeSize() const {
			return m_VertexTypeSize;
		}
		EReMeshType GetType() const {
			return m_MeshType;
		}
		const VertexData& GetVertexData_c() const {
			return m_MeshVertexDatas;
		}
		const IndexData& GetIndexData_c() const {
			return m_MeshIndexDatas;
		}
		VertexData& GetVertexData() {
			return m_MeshVertexDatas;
		}
		IndexData& GetIndexData() {
			return m_MeshIndexDatas;
		}
		void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY topology) {
			m_Topology = topology;
		}
		void Reset()
		{
			m_MeshIndexDatas.clear();
			m_MeshVertexDatas.clear();
			m_VertexTypeSize = 0;
		}
		void Draw(CommandList* cmdList, uint32_t instanceCount = 1);
	public:
		static std::shared_ptr<ReMesh> Create(EReMeshShape shape);
	protected:
		template<typename T>
		void Set(const std::vector<T>& v, const IndexData& i)
		{
			m_VertexTypeSize = (uint32_t)sizeof(T);
			size_t btSize = v.size() * sizeof(T);
			m_MeshVertexDatas.resize(btSize);
			memcpy_s(&m_MeshVertexDatas[0], btSize, v.data(), btSize);
			m_MeshIndexDatas = std::move(i);
		}
		template<typename T>
		void Add(const std::vector<T>& v, const IndexData& i)
		{
			if(m_MeshVertexDatas.empty()) m_VertexTypeSize = (uint32_t)sizeof(T);
			size_t btSize = v.size() * sizeof(T);
			std::vector<byte> btData(btSize);
			memcpy_s(&btData[0], btSize, v.data(), btSize);
			

			m_MeshVertexDatas.insert(m_MeshVertexDatas.end(), btData.begin(), btData.end());
			m_MeshIndexDatas.insert(m_MeshIndexDatas.end(), i.begin(), i.end());
		}
		template<typename T>
		void Insert(const std::vector<T>& v, uint32_t vpos, const IndexData& i,  uint32_t ipos)
		{
			size_t btSize = v.size() * sizeof(T);
			std::vector<byte> btData(btSize);
			memcpy_s(&btData[0], btSize, v.data(), btSize);


			size_t btpos = sizeof(T) * vpos;
			
			auto viter = m_MeshVertexDatas.begin() + btpos;
			auto iiter = m_MeshIndexDatas.begin() + ipos;
			m_MeshVertexDatas.insert(viter, btData.begin(), btData.end());
			m_MeshIndexDatas.insert(iiter, i.begin(), i.end());
		}
		template<typename T>
		T GetV(uint32_t idx)
		{
			T result;
			size_t btSize = sizeof(T);
			size_t btpos = sizeof(T) * idx;
			assert(btSize + btpos <= m_MeshVertexDatas.size());
			memcpy_s(&result, btSize, &m_MeshVertexDatas[btpos] , btSize);
			return result;
		}
		uint32_t GetI(uint32_t idx)
		{
			return m_MeshIndexDatas[idx];
		}
		template<typename T>
		void ModifyV(const T& data, uint32_t idx)
		{
			size_t btSize = sizeof(T);
			size_t btpos = sizeof(T) * idx;
			if (btSize + btpos > m_MeshVertexDatas.size()) return;
			memcpy_s(&m_MeshVertexDatas[btpos], btSize, &data, btSize);
		}
		void ModifyI(uint32_t i, uint32_t idx)
		{
			m_MeshIndexDatas[idx] = i;
		}
		template<typename T>
		void Modify(const std::vector<T>& v, uint32_t vpos)
		{
			size_t btSize = v.size() * sizeof(T);
			size_t btpos  = sizeof(T) * vpos;
			if (btSize + btpos > m_MeshVertexDatas.size()) return;
			memcpy_s(&m_MeshVertexDatas[btpos], btSize, v.data(), btSize);
		}
		template<typename T>
		void Modify(const IndexData& i, uint32_t ipos)
		{
			if (i.size() + ipos > m_MeshIndexDatas.size()) return;
			size_t iSize = i.size();
			memcpy_s(&m_MeshIndexDatas[ipos], iSize, i.data(), iSize);
		}
		template<typename T>
		void Modify(const std::vector<T>& v, uint32_t vpos, const IndexData& i, uint32_t ipos)
		{
			Modify(v, vpos);
			Modify(i, ipos);
		}
		void Remove(uint32_t vpos_start, uint32_t vpos_end, uint32_t ipos_start, uint32_t ipos_end)
		{
			auto typesize = m_VertexTypeSize;
			auto vstart = m_MeshVertexDatas.begin() + (vpos_start * typesize);
			auto vend = m_MeshVertexDatas.begin() + (vpos_end * typesize);


			m_MeshVertexDatas.erase(vstart, vend);


			auto istart = m_MeshIndexDatas.begin() + ipos_start;
			auto iend = m_MeshIndexDatas.begin() + ipos_end;

			m_MeshIndexDatas.erase(istart, iend);

		}


		EReMeshType  m_MeshType;
		VertexData   m_MeshVertexDatas;
		IndexData    m_MeshIndexDatas;
		uint32_t     m_VertexTypeSize;
		D3D_PRIMITIVE_TOPOLOGY m_Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	public:
		static ReMesh Merge(const ReMesh& m1, const ReMesh& m2);
	};


	class RENDERENGINE_API ReStaticMesh : public ReMesh
	{
	public:
		ReStaticMesh() : ReMesh("ReStaticMesh", EReMeshType::Static) {}
		ReStaticMesh(const std::string& name) : ReMesh(name, EReMeshType::Static) {}

		void Create(const std::vector<JVertex>& v, const IndexData& i)
		{
			Set(v, i);
		}
	};

	class RENDERENGINE_API ReSkeletalMesh : public ReMesh
	{
	public:
		ReSkeletalMesh() : ReMesh("ReSkeletalMesh", EReMeshType::Skeletal) {}
		ReSkeletalMesh(const std::string& name) : ReMesh(name, EReMeshType::Skeletal) {}

		void Create(const std::vector<JSkinnedVertex>& v, const IndexData& i)
		{
			Set(v, i);
		}
	};

	class RENDERENGINE_API ReGuiMesh : public ReMesh
	{
	public:
		ReGuiMesh() : ReMesh("ReGuiMesh", EReMeshType::GUI) {}
		ReGuiMesh(const std::string& name) : ReMesh(name, EReMeshType::GUI) {}
		void Create(const std::vector<JGUIVertex>& v, const IndexData& i)
		{
			Set(v, i);
		}
		void Add(const std::vector<JGUIVertex>& v, const IndexData& i)
		{
			ReMesh::Add(v, i);
		}
		void Insert(const std::vector<JGUIVertex>& v, uint32_t vpos, const IndexData& i, uint32_t ipos)
		{
			ReMesh::Insert(v, vpos, i, ipos);
		}
		JGUIVertex GetV(uint32_t idx)
		{
			return ReMesh::GetV<JGUIVertex>(idx);
		}
		uint32_t  GetI(uint32_t idx)
		{
			return ReMesh::GetI(idx);
		}
		void ModifyV(const JGUIVertex& v, uint32_t idx)
		{
			ReMesh::ModifyV(v, idx);
		}
		void ModifyI(uint32_t i, uint32_t idx)
		{
			ReMesh::ModifyI(i, idx);
		}
		void Modify(const std::vector<JGUIVertex>& v, uint32_t vpos)
		{
			ReMesh::Modify(v, vpos);
		}
		void Modify(const IndexData& i, uint32_t ipos)
		{
			ReMesh::Modify(i, ipos);
		}
		void Modify(const std::vector<JGUIVertex>& v, uint32_t vpos, const IndexData& i, uint32_t ipos)
		{
			ReMesh::Modify(v, vpos, i, ipos);
		}
		void Remove(uint32_t vstart, uint32_t vend, uint32_t istart, uint32_t iend)
		{
			ReMesh::Remove(vstart, vend, istart, iend);
		}
	public:
		static std::shared_ptr<ReGuiMesh> CreateFillRect(float width, float height);
		static std::shared_ptr<ReGuiMesh> CreateEmptyRect(float width, float height);
	};




}