#include"JGCollisionDebugDraw.h"
#include"../../../RenderSystem/JGMaterial/JGMesh.h"
#include"../../../RenderSystem/JGHLSLShaderDevice/JGHLSLShaderDevice.h"
#include"../../../RenderSystem/JGHLSLShaderDevice/JGBufferManager.h"
#include"../../../RenderSystem/JGHLSLShaderDevice/JGShaderData.h"
#include"../../../RenderSystem/JGHLSLShaderDevice/JGBuffer.h"
#include"../../../EngineStatics/JMath/JGMatrix.h"
#include"../../../RenderSystem/JGViewportD.h"
#include"../../../RenderSystem/JGDeviceD.h"
#include"../../../RenderSystem/ShaderCode/HLSLConstantBufferDesc.h"
#include"../../../RenderSystem/JGTexture.h"
using namespace std;
#pragma warning (disable : 4996)
JGCollisionDebugDraw::JGCollisionDebugDraw()
{
	m_WorldMatrix = make_unique<JGMatrix>();
	m_OthroMatrix = make_unique<JGMatrix>();
	m_ViewMatrix = make_unique<JGMatrix>();
	m_MatrixBufferDesc = make_unique<SMatrixBuffer_VS>();
	m_WorldMatrix->Identity();
	JGVector2D vec(0.0f, 0.0f);
	JGMatrix TransMatrix;
	TransMatrix.MakeTranslation2DMatrix(m_pViewPort->GetWidth(),
		m_pViewPort->GetHeight(), vec);
	*m_WorldMatrix = (*m_WorldMatrix) * TransMatrix;
}
JGCollisionDebugDraw::~JGCollisionDebugDraw()
{
}
void JGCollisionDebugDraw::InitCollisionDebugDraw(JGDeviceD* Device, HWND hWnd, JGHLSLShaderDevice* HLSLDevice, JGBufferManager* BufferManager, JGViewportD* viewPort, const JGMatrix& ViewMatrix)
{
	m_pDevice = Device;
	m_pHLSLDevice = HLSLDevice;
	m_pBufferManager = BufferManager;
	m_pViewPort = viewPort;
	*m_ViewMatrix = ViewMatrix;
	*m_OthroMatrix = m_pViewPort->GetOrthoMatrix();
	InitDebugMesh();
}
void JGCollisionDebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	SCollisionDebugVertex* vertex = new SCollisionDebugVertex[vertexCount + 1];
	for (int32 i = 0; i < vertexCount; ++i)
	{
		b2Vec2 vec = vertices[i];
		JGVector2D JGVec = b2ToJG_Vector(vec);
		vertex[i].position = { JGVec.X(), -JGVec.Y(),0.0f };
		vertex[i].color = { color.r,color.g,color.b,color.a };
	}
	b2Vec2 vec = vertices[0];
	JGVector2D JGVec = b2ToJG_Vector(vec);
	vertex[vertexCount].position = { JGVec.X(), -JGVec.Y(),0.0f };
	vertex[vertexCount].color = { color.r,color.g,color.b,color.a };


	wchar_t  PolygonCountStr[3];
	_itow((int)vertexCount, PolygonCountStr, 10);
	wstring PolygonCountString = PolygonCountStr;
	m_vDebugMeshs[vertexCount]->MeshWrite(TT("PolygonDebugMesh") + PolygonCountString,
		EJGMapType::Write_Discard, vertex);
	MeshDraw((CollisionMeshType)vertexCount);

	delete vertex;
	vertex = nullptr;
}

void JGCollisionDebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color & color)
{
	DrawPolygon(vertices, vertexCount, color);
}

void JGCollisionDebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color & color)
{

}

void JGCollisionDebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2 & axis, const b2Color & color)
{

}

void JGCollisionDebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2 & p2, const b2Color & color)
{

}

void JGCollisionDebugDraw::DrawTransform(const b2Transform& xf)
{

}

void JGCollisionDebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color & color)
{

}

void JGCollisionDebugDraw::InitDebugMesh()
{
	// ����  �� �޽� �ʱ�ȭ.
	std::unique_ptr<JGMesh> CircleMesh = make_unique<JGMesh>();
	m_vDebugMeshs.push_back(move(CircleMesh));

	// ������ 1~8 ��
	AddInitPolygonMesh(1);
	AddInitPolygonMesh(2);
	AddInitPolygonMesh(3);
	AddInitPolygonMesh(4);
	AddInitPolygonMesh(5);
	AddInitPolygonMesh(6);
	AddInitPolygonMesh(7);
	AddInitPolygonMesh(8);
}

void JGCollisionDebugDraw::AddInitPolygonMesh(const size_t PolygonCount)
{
	SCollisionDebugVertex* vertex = new SCollisionDebugVertex[PolygonCount + 1];
	wchar_t  PolygonCountStr[3];
	_itow((int)PolygonCount, PolygonCountStr, 10);
	wstring PolygonCountString = PolygonCountStr;

	std::unique_ptr<JGMesh> PolygonMesh = make_unique<JGMesh>();

	PolygonMesh->Create_Vertex_Index_Buffer(m_pBufferManager,
		TT("PolygonDebugMesh") + PolygonCountString, EJGUsageType::Dynamic,
		EJGCPUType::Access_Write, EJGUsageType::Static, EJGCPUType::None, vertex,
		PolygonCount + 1, sizeof(SCollisionDebugVertex));
	m_vDebugMeshs.push_back(move(PolygonMesh));

	delete vertex;
	vertex = nullptr;
}

void JGCollisionDebugDraw::MeshDraw(CollisionMeshType type)
{
	m_vDebugMeshs[type]->Render(m_pDevice, ERenderingType::LineStrip);
	JGShaderData data;
	SMatrixBuffer_VS desc;
	JGMatrix worldMatrix = *m_WorldMatrix;
	JGMatrix viewMatrix = *m_ViewMatrix;
	JGMatrix orthoMatrix = m_pViewPort->GetOrthoMatrix();
	worldMatrix.Transpose();

	viewMatrix.Transpose();
	orthoMatrix.Transpose();
	desc.WoldViewProjectionMatrix = (orthoMatrix* viewMatrix* worldMatrix).Get();
	data.InsertData(TT("MatrixBuffer"), &desc);

	if (type != Circle)
	{
		m_pHLSLDevice->Render(TT("ColorShader"), &data, nullptr, type + 1);
	}
}


