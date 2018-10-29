#include"TestModel.h"
#include"BufferSystem/JGBufferManager.h"
#include"BufferSystem/JGBuffer.h"
using namespace std;
using namespace JGRC;
TestModel::TestModel()
{
	m_model = 0;
}
TestModel::~TestModel()
{
}
bool TestModel::Initialize(ID3D11Device* device, const char* mdoelpath, const WCHAR* textureFilename)
{
	bool result;

	result = LoadModel(mdoelpath);
	if (!result)
	{
		return false;
	}
	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}
	return true;
}

void TestModel::Shutdown()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = nullptr;
	}
	return;
}
void TestModel::Render(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(real) * 8;
	offset = 0;
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, VertexBuffer->GetAddress(), &stride, &offset);


	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(IndexBuffer->Get(), DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
int TestModel::GetIndexCount()
{
	return m_indexCount;
}
bool TestModel::LoadModel(const char * filename)
{
	ifstream fin;
	char input;
	int i;

	fin.open(filename);

	if (fin.fail())
	{
		return false;
	}

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	fin >> m_vertexCount;

	m_indexCount = m_vertexCount;

	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	for (i = 0; i < m_vertexCount; ++i)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	fin.close();

	return true;
}

bool TestModel::InitializeBuffers(ID3D11Device* device)
{
	unsigned long* indices;


	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	for (int i = 0; i < m_vertexCount; ++i)
	{
		m_Sample.push_back(m_model[i].x);
		m_Sample.push_back(m_model[i].y);
		m_Sample.push_back(m_model[i].z);
		m_Sample.push_back(m_model[i].tu);
		m_Sample.push_back(m_model[i].tv);
		m_Sample.push_back(m_model[i].nx);
		m_Sample.push_back(m_model[i].ny);
		m_Sample.push_back(m_model[i].nz);

		indices[i] = i;
	}
	VertexBuffer = JGBufferManager::GetInstance()->CreateBuffer(EBufferType::VertexBuffer,
		EUsageType::Static, ECPUType::None, &m_Sample[0], sizeof(real), m_Sample.size());
	IndexBuffer = JGBufferManager::GetInstance()->CreateBuffer(EBufferType::IndexBuffer,
		EUsageType::Static, ECPUType::None, indices, sizeof(unsigned int), m_vertexCount);

	// Release the arrays now that the vertex and index buffers have been created and loaded.

	delete[] indices;
	indices = 0;

	return true;
}