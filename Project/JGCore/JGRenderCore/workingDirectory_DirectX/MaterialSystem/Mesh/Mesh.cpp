#include"Mesh.h"
#include"DirectX/DirectX.h"
#include"BufferSystem/JGBufferManager.h"
#include"BufferSystem/JGBuffer.h"
using namespace std;
using namespace JGRC;
DirectX* Mesh::m_Dx = nullptr;
JGBufferManager* Mesh::m_BfManager = nullptr;
Mesh::Mesh()
{
	m_BfManager = JGBufferManager::GetInstance();
	m_Dx = DirectX::GetInstance();
	m_Path = "None";

}
Mesh::Mesh(const Mesh& copy)
{
	m_Path      = copy.m_Path;
	m_OuputData = copy.m_OuputData;
	m_vVertexBuffers = copy.m_vVertexBuffers;
	m_IndexBuffer = copy.m_IndexBuffer;
}
Mesh::Mesh(Mesh&& mesh)
{
	m_Path = move(mesh.m_Path);
	mesh.m_Path = "None";

	m_OuputData = move(mesh.m_OuputData);
	mesh.m_OuputData.clear();


	m_vVertexBuffers = move(mesh.m_vVertexBuffers);
	mesh.m_vVertexBuffers.clear();


	m_IndexBuffer = move(mesh.m_IndexBuffer);
	mesh.m_IndexBuffer = nullptr;
}
Mesh::~Mesh()
{

}
Mesh& Mesh::operator=(const Mesh& mesh)
{
	m_Path = mesh.m_Path;
	m_OuputData = mesh.m_OuputData;
	m_vVertexBuffers = mesh.m_vVertexBuffers;
	m_IndexBuffer = mesh.m_IndexBuffer;

	return *this;
}
Mesh& Mesh::operator=(Mesh&& mesh)
{
	m_Path = move(mesh.m_Path);
	mesh.m_Path = "None";

	m_OuputData = move(mesh.m_OuputData);
	mesh.m_OuputData.clear();

	m_vVertexBuffers = move(mesh.m_vVertexBuffers);
	mesh.m_vVertexBuffers.clear();


	m_IndexBuffer = move(mesh.m_IndexBuffer);
	mesh.m_IndexBuffer = nullptr;
	return *this;
}
void Mesh::CreateBuffer(InputLayoutData* data)
{
	if (data == nullptr) return; // data 가 nullptr이라면 버퍼 생성을 하지않는다.
	//
	index prevSlot = 999999;
	index count = 0;
	std::vector<EBufferType> vBfType;
	bool bInstance = false;

	//
	for (index i = 0; i < data->vDescs.size(); ++i)
	{
		if (prevSlot != data->vDescs[i].InputSlot)
		{
			prevSlot = data->vDescs[i].InputSlot;
			m_Stride.push_back((index)data->vStrides[count++]);
			m_Offset.push_back(0);
			switch (data->vDescs[i].InputSlotClass)
			{
			case D3D11_INPUT_PER_VERTEX_DATA:
				vBfType.push_back(EBufferType::VertexBuffer);
				break;
			case D3D11_INPUT_PER_INSTANCE_DATA:
				bInstance = true;
				vBfType.push_back(EBufferType::InstanceBuffer);
				break;
			}
		}
	}
	//
	if (m_OuputData.size() != m_Stride.size())
	{
		JGLOG(log_Error, "JGRC::Mesh", "Mesh's Stride value and VerteDataSize is not equal!");
		return;
	}
	for (uint i = 0; i < m_Stride.size(); ++i)
	{
		JGBuffer* bf = m_BfManager->CreateBuffer(vBfType[i], EUsageType::Static, ECPUType::None,
			&(m_OuputData[i])[0], sizeof(real), m_OuputData[i].size());
		m_vVertexBuffers.push_back(bf);
	}
	//
	if (!bInstance)
	{
		m_IndexBuffer = m_BfManager->CreateBuffer(EBufferType::IndexBuffer, EUsageType::Static,
			ECPUType::None, &m_IndexData[0], sizeof(index), m_IndexData.size());
	}
}
void Mesh::Render(D3D_PRIMITIVE_TOPOLOGY set)
{
	//  stride, offset 설정
	//
	std::vector<ID3D11Buffer*> bufferArrray;
	if (m_vVertexBuffers.size() == 0) return;
	for (uint i = 0; i < m_vVertexBuffers.size(); ++i)
	{
		bufferArrray.push_back(m_vVertexBuffers[i]->Get());
	}

	m_Dx->GetContext()->IASetVertexBuffers(0, (UINT)bufferArrray.size(), &bufferArrray[0], &m_Stride[0], &m_Offset[0]);
	if (m_IndexBuffer)
	{
		m_Dx->GetContext()->IASetIndexBuffer(m_IndexBuffer->Get(), DXGI_FORMAT_R32_UINT, 0);
	}
	m_Dx->GetContext()->IASetPrimitiveTopology(set);
}
uint Mesh::getIndexCount()
{
	return m_IndexData.size();
}
void Mesh::CalculationTangentBinormal(jgVec3 vector[3], jgVec2 tex[3], jgVec3* normal, jgVec3* tangent, jgVec3* binormal)
{
	jgVec3 v1 = vector[1] - vector[0];
	jgVec3 v2 = vector[2] - vector[0];

	jgVec2 t1 = tex[1] - tex[0];
	jgVec2 t2 = tex[2] - tex[0];

	// 
	real den = 1.0f / ((t1.x * t2.y) - (t2.x * t1.y));
	//
	*tangent = ((t2.y * v1) - (t1.y * v2)) * den;
	tangent->normalize();

	//
	*binormal = ((t1.x * v2) - (t2.x * v1)) * den;
	binormal->normalize();

	(*normal) = tangent->cross(*binormal);
}
bool Mesh::LoadModel(const string& path, bool btangent)
{
	ifstream fin;
	char input;
	index vertexCount;
	Data Vertex;
	fin.open(path);

	if (fin.fail())
	{
		return false;
	}

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	fin >> vertexCount;

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	real value;
	if (btangent)
	{
		jgVec3 v[3];
		jgVec2 t[3];
		for (index i = 0; i < vertexCount;)
		{
			jgVec3 v[3];
			jgVec2 t[3];

			for (index j = 0; j < 3; ++j)
			{
				real x, y, z;
				real tu, tv;

				fin >> x >> y >> z;
				fin >> tu >> tv;
				fin >> value >> value >> value;

				v[j].set(x, y, z);
				t[j].set(tu, tv);
			}
			jgVec3 normal;
			jgVec3 tangent;
			jgVec3 binormal;
			CalculationTangentBinormal(v, t, &normal, &tangent, &binormal);

			for (index j = 0; j < 3; ++j)
			{
				Vertex.push_back(v[j].x);
				Vertex.push_back(v[j].y);
				Vertex.push_back(v[j].z);

				Vertex.push_back(t[j].x);
				Vertex.push_back(t[j].y);

				Vertex.push_back(normal.x);
				Vertex.push_back(normal.y);
				Vertex.push_back(normal.z);

				Vertex.push_back(tangent.x);
				Vertex.push_back(tangent.y);
				Vertex.push_back(tangent.z);

				Vertex.push_back(binormal.x);
				Vertex.push_back(binormal.y);
				Vertex.push_back(binormal.z);

				m_IndexData.push_back(i + j);
			}
			i += 3;
		}
	}
	else
	{
		for (index i = 0; i < vertexCount; ++i)
		{
			fin >> value;
			Vertex.push_back(value);
			fin >> value;
			Vertex.push_back(value);
			fin >> value;
			Vertex.push_back(value);
			fin >> value;
			Vertex.push_back(value);
			fin >> value;
			Vertex.push_back(value);
			fin >> value;
			Vertex.push_back(value);
			fin >> value;
			Vertex.push_back(value);
			fin >> value;
			Vertex.push_back(value);
			m_IndexData.push_back(i);
		}
	}
	
	m_OuputData.push_back(move(Vertex));
	fin.close();

	return true;
}
void Mesh::CustomModel(const std::vector<real>& vertex, const uint count)
{
	for (index i = 0; i < count; ++i)
	{
		m_IndexData.push_back(i);
	}
	m_OuputData.push_back(move(vertex));
}
