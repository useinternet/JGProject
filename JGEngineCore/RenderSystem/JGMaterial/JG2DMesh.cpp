#include"JG2DMesh.h"
#include"../JGHLSLShaderDevice/JGBuffer.h"
using namespace std;

JG2DMesh::JG2DMesh()
{
}
JG2DMesh::~JG2DMesh()
{



}
bool JG2DMesh::Construct2DMesh(JGBufferManager* BufferManager,const std::wstring& ComponentName,
	const float Width, const float Height, int ZOrder, EPivot Pivot)
{
	m_Width = Width;
	m_Height = Height;

	vector<S2DVertexType> vVertexArray;
	S2DVertexType Vertex;

	SPivotPoint TopLeft     = { 0.0f, 0.0f };
	SPivotPoint TopRight    = { 0.0f, 0.0f };
	SPivotPoint BottomLeft  = { 0.0f, 0.0f };
	SPivotPoint BottomRight = { 0.0f, 0.0f };

	// 피봇에 따라 저장해야할 메쉬가 달라진다..
	switch (Pivot)
	{
	case EPivot::TopLeft:
		TopLeft     = { 0.0f,0.0f };
		TopRight    = { m_Width, 0.0f };
		BottomLeft  = { 0.0f, -m_Height };
		BottomRight = { m_Width, -m_Height };
		break;
	case EPivot::TopMiddle:
		TopLeft     = { -(Width / 2),0.0f };
		TopRight    = { (Width / 2), 0.0f };
		BottomLeft  = { -(Width / 2), -m_Height };
		BottomRight = { (Width / 2), -m_Height };
		break;
	case EPivot::TopRight:
		TopLeft     = { -Width,0.0f };
		TopRight    = { 0.0f, 0.0f };
		BottomLeft  = { -Width, -m_Height };
		BottomRight = { 0.0f, -m_Height };
		break;
	case EPivot::MiddleLeft:
		TopLeft     = { 0.0f ,(Height / 2) };
		TopRight    = { m_Width, (Height / 2) };
		BottomLeft  = { 0.0f, -(Height / 2) };
		BottomRight = { m_Width, -(Height / 2) };
		break;
	case EPivot::MiddleMiddle:
		TopLeft     = { -(Width / 2), (Height / 2) };
		TopRight    = { (Width / 2), (Height / 2) };
		BottomLeft  = { -(Width / 2), -(Height / 2) };
		BottomRight = { (Width / 2) , -(Height / 2) };
		break;
	case EPivot::MiddleRight:
		TopLeft     = { -Width, (Height / 2) };
		TopRight    = { 0.0f, (Height / 2) };
		BottomLeft  = { -Width, -(Height / 2) };
		BottomRight = { 0.0f , -(Height / 2) };
		break;
	case EPivot::BottomLeft:
		TopLeft     = { 0.0f, Height };
		TopRight    = { Width, Height };
		BottomLeft  = { 0.0f, 0.0f, };
		BottomRight = { Width , 0.0f };
		break;
	case EPivot::BottomMiddle:
		TopLeft     = { -(Width / 2), Height };
		TopRight    = { (Width / 2), Height };
		BottomLeft  = { -(Width / 2), 0.0f };
		BottomRight = { (Width / 2) , 0.0f };
		break;
	case EPivot::BottomRight:
		TopLeft     = { -Width, Height };
		TopRight    = { 0.0f, Height };
		BottomLeft  = { -Width, 0.0f };
		BottomRight = { 0.0f , 0.0f };
		break;
	}


	float fZOrder = (float)ZOrder;


	Vertex.position = D3DXVECTOR3(TopLeft.x, TopLeft.y, fZOrder);
	Vertex.tex = D3DXVECTOR2(0.0f, 0.0f);
	vVertexArray.push_back(Vertex);
	Vertex.position = D3DXVECTOR3(BottomRight.x, BottomRight.y, fZOrder);
	Vertex.tex = D3DXVECTOR2(1.0f, 1.0f);
	vVertexArray.push_back(Vertex);
	Vertex.position = D3DXVECTOR3(BottomLeft.x, BottomLeft.y, fZOrder);
	Vertex.tex = D3DXVECTOR2(0.0f, 1.0f);
	vVertexArray.push_back(Vertex);
	Vertex.position = D3DXVECTOR3(TopLeft.x, TopLeft.y, fZOrder);
	Vertex.tex = D3DXVECTOR2(0.0f, 0.0f);
	vVertexArray.push_back(Vertex);
	Vertex.position = D3DXVECTOR3(TopRight.x, TopRight.y, fZOrder);
	Vertex.tex = D3DXVECTOR2(1.0f, 0.0f);
	vVertexArray.push_back(Vertex);
	Vertex.position = D3DXVECTOR3(BottomRight.x, BottomRight.y, fZOrder);
	Vertex.tex = D3DXVECTOR2(1.0f, 1.0f);
	vVertexArray.push_back(Vertex);

	// 메쉬 생성
	bool result = Create_Vertex_Index_Buffer(
		BufferManager,
		ComponentName,
		EJGUsageType::Static, EJGCPUType::None,
		EJGUsageType::Static, EJGCPUType::None,
		&vVertexArray[0], m_2DVertexCount, sizeof(S2DVertexType));
	if (!result)
	{
		return false;
	}

	return true;
}
