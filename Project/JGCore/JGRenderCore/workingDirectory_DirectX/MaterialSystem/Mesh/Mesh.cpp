#include"Mesh.h"
#include"DirectX/DirectX.h"
#include"BufferSystem/JGBuffer.h"
using namespace std;
using namespace JGRC;
DirectX* Mesh::m_Dx = nullptr;
Mesh::Mesh()
{
	m_Dx = DirectX::GetInstance();
	m_Path = "None";

}
Mesh::~Mesh()
{

}
void Mesh::CreateBuffer()
{
	m_VertexBuffer = m_Dx->CreateObject<JGBuffer>();
	m_VertexBuffer->CreateBuffer(EBufferType::VertexBuffer, EUsageType::Static, ECPUType::None,
		&m_OutputData[0], sizeof(real) * m_OutputData.size());
	
	m_IndexBuffer = m_Dx->CreateObject<JGBuffer>();
		m_IndexBuffer->CreateBuffer(EBufferType::IndexBuffer, EUsageType::Static,
		ECPUType::None, &m_IndexData[0], sizeof(index) * m_IndexData.size());
}
void Mesh::Render(D3D_PRIMITIVE_TOPOLOGY set)
{
	UINT Stride = 56;
	UINT Offset = 0;

	m_Dx->GetContext()->IASetVertexBuffers(0, (UINT)1, m_VertexBuffer->GetAddress(),
		&Stride, &Offset);

	m_Dx->GetContext()->IASetIndexBuffer(m_IndexBuffer->Get(), DXGI_FORMAT_R32_UINT, 0);
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
	m_OutputData = move(Vertex);
	fin.close();

	return true;
}
void Mesh::CustomModel(const std::vector<real>& vertex, const uint count)
{
	for (index i = 0; i < count; ++i)
	{
		m_IndexData.push_back(i);
	}
	m_OutputData = move(vertex);
}
