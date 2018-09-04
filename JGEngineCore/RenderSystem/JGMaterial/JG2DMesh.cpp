#include"JG2DMesh.h"
#include"../JGHLSLShaderDevice/JGBuffer.h"
#include"../../EngineFrameWork/2D/Text/JGFontLoader.h"
using namespace std;

JG2DMesh::JG2DMesh()
{
}
JG2DMesh::~JG2DMesh()
{



}
bool JG2DMesh::Construct2DMesh(JGBufferManager* BufferManager,const std::wstring& ComponentName,
	const float Width, const float Height,EPivot Pivot, const STexInformaton& TexInformation)
{
	m_Pivot = Pivot;
	CreateVertexArray(Width, Height, Pivot, TexInformation);
	// 메쉬 생성
	bool result = Create_Vertex_Index_Buffer(
		BufferManager,
		ComponentName,
		EJGUsageType::Static, EJGCPUType::None,
		EJGUsageType::Static, EJGCPUType::None,
		&m_vVertexArray[0], m_2DVertexCount, sizeof(S2DVertexType));
	if (!result)
	{
		return false;
	}

	return true;
}

bool JG2DMesh::ConstructTextMesh(JGBufferManager* BufferManager, const wstring& ComponentName, const vector<JGFontVertexInformation>& VertexInformation, EPivot Pivot)
{
	m_Pivot = Pivot;
	STexInformaton TexInfor;
	for (size_t i = 0; i < VertexInformation.size(); ++i)
	{
		TexInfor.Width = VertexInformation[i].TexWidth;
		TexInfor.Height = VertexInformation[i].TexHeight;
		TexInfor.TopLeft = { VertexInformation[i].TexU,VertexInformation[i].TexV };

		CreateVertexArray(VertexInformation[i].Width, VertexInformation[i].Height,
			Pivot, TexInfor, VertexInformation[i].XAdvance);
	}

	// 메쉬 생성
	bool result = Create_Vertex_Index_Buffer(
		BufferManager,
		ComponentName,
		EJGUsageType::Dynamic, EJGCPUType::Access_Write,
		EJGUsageType::Static, EJGCPUType::None,
		&m_vVertexArray[0], m_vVertexArray.size(), sizeof(S2DVertexType));
	if (!result)
	{
		return false;
	}
	return true;
}
size_t JG2DMesh::GetIndexCount()
{
	return m_vVertexArray.size();
}
void JG2DMesh::WriteTextMesh(const vector<JGFontVertexInformation>& VertexInformation)
{
	m_vVertexArray.clear();
	STexInformaton TexInfor;
	for (size_t i = 0; i < VertexInformation.size(); ++i)
	{
		TexInfor.Width = VertexInformation[i].TexWidth;
		TexInfor.Height = VertexInformation[i].TexHeight;
		TexInfor.TopLeft = { VertexInformation[i].TexU,VertexInformation[i].TexV };
		CreateVertexArray(VertexInformation[i].Width, VertexInformation[i].Height,
			m_Pivot, TexInfor, VertexInformation[i].XAdvance);
	}
}
void JG2DMesh::CreateVertexArray(const float Width, const float Height, EPivot Pivot, const STexInformaton& TexInformation, 
	float AccX, float AccY)
{
	S2DVertexType Vertex;

	SPivotPoint TopLeft = { 0.0f, 0.0f };
	SPivotPoint TopRight = { 0.0f, 0.0f };
	SPivotPoint BottomLeft = { 0.0f, 0.0f };
	SPivotPoint BottomRight = { 0.0f, 0.0f };

	// 피봇에 따라 저장해야할 메쉬가 달라진다..
	switch (Pivot)
	{
	case EPivot::TopLeft:
		TopLeft = { AccX, AccY };
		TopRight = { AccX + Width, AccY };
		BottomLeft = { AccX, AccY -Height };
		BottomRight = { AccX + Width, AccY -Height };
		break;
	case EPivot::TopMiddle:
		TopLeft = { AccX -(Width / 2),AccY };
		TopRight = { AccX + (Width / 2), AccY };
		BottomLeft = { AccX -(Width / 2), AccY -Height };
		BottomRight = { AccX + (Width / 2), AccY -Height };
		break;
	case EPivot::TopRight:
		TopLeft = { AccX -Width,AccY };
		TopRight = { AccX, AccY };
		BottomLeft = { AccX -Width, AccY -Height };
		BottomRight = { AccX, AccY -Height };
		break;
	case EPivot::MiddleLeft:
		TopLeft = { AccX ,AccY + (Height / 2) };
		TopRight = { AccX + Width, AccY + (Height / 2) };
		BottomLeft = { AccX, AccY -(Height / 2) };
		BottomRight = { AccX + Width, AccY -(Height / 2) };
		break;
	case EPivot::MiddleMiddle:
		TopLeft = { AccX -(Width / 2), AccY + (Height / 2) };
		TopRight = { AccX + (Width / 2), AccY + (Height / 2) };
		BottomLeft = { AccX -(Width / 2),AccY -(Height / 2) };
		BottomRight = { AccX + (Width / 2) ,AccY -(Height / 2) };
		break;
	case EPivot::MiddleRight:
		TopLeft = { AccX -Width, AccY + (Height / 2) };
		TopRight = { AccX, AccY + (Height / 2) };
		BottomLeft = { AccX -Width, AccY -(Height / 2) };
		BottomRight = { AccX , AccY -(Height / 2) };
		break;
	case EPivot::BottomLeft:
		TopLeft = { AccX, AccY + Height };
		TopRight = { AccX + Width,AccY + Height };
		BottomLeft = { AccX, AccY, };
		BottomRight = { AccX + Width , AccY };
		break;
	case EPivot::BottomMiddle:
		TopLeft = { AccX -(Width / 2),AccY +  Height };
		TopRight = { AccX + (Width / 2), AccY + Height };
		BottomLeft = { AccX -(Width / 2), AccY };
		BottomRight = { AccX + (Width / 2) , AccY };
		break;
	case EPivot::BottomRight:
		TopLeft = { AccX -Width, AccY + Height };
		TopRight = { AccX, AccY + Height };
		BottomLeft = { AccX -Width, AccY };
		BottomRight = { AccX , AccY };
		break;
	}

	Vertex.position = D3DXVECTOR3(TopLeft.x, TopLeft.y, 0.0f);
	Vertex.tex = D3DXVECTOR2(TexInformation.TopLeft.x, TexInformation.TopLeft.y);
	m_vVertexArray.push_back(Vertex);
	Vertex.position = D3DXVECTOR3(BottomRight.x, BottomRight.y, 0.0f);
	Vertex.tex = D3DXVECTOR2(TexInformation.TopLeft.x + TexInformation.Width,
		TexInformation.TopLeft.y + TexInformation.Height);
	m_vVertexArray.push_back(Vertex);
	Vertex.position = D3DXVECTOR3(BottomLeft.x, BottomLeft.y, 0.0f);
	Vertex.tex = D3DXVECTOR2(TexInformation.TopLeft.x,
		TexInformation.TopLeft.y + TexInformation.Height);
	m_vVertexArray.push_back(Vertex);
	Vertex.position = D3DXVECTOR3(TopLeft.x, TopLeft.y, 0.0f);
	Vertex.tex = D3DXVECTOR2(TexInformation.TopLeft.x, TexInformation.TopLeft.y);
	m_vVertexArray.push_back(Vertex);
	Vertex.position = D3DXVECTOR3(TopRight.x, TopRight.y, 0.0f);
	Vertex.tex = D3DXVECTOR2(TexInformation.TopLeft.x + TexInformation.Width,
		TexInformation.TopLeft.y);
	m_vVertexArray.push_back(Vertex);
	Vertex.position = D3DXVECTOR3(BottomRight.x, BottomRight.y, 0.0f);
	Vertex.tex = D3DXVECTOR2(TexInformation.TopLeft.x + TexInformation.Width,
		TexInformation.TopLeft.y + TexInformation.Height);
	m_vVertexArray.push_back(Vertex);
}


