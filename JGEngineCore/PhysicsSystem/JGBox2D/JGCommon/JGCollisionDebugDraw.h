#pragma once
#include"../../../EngineStatics/Engine.h"
#include"b2ToJGCommon.h"


// 이제 그리는거 됬으니
// 뷰 매트릭스 월드에서 뽑고..
// 메쉬 쓰기 기능 추가..
class JGViewportD;
class JGDeviceD;
class JGHLSLShaderDevice;
class JGBufferManager;
class JGMatrix;
class JGMesh;
enum class ERenderingType;
class JGCollisionDebugDraw : public b2Draw
{
private:
	enum CollisionMeshType
	{
		Circle = 0,
		P_1 = 1,
		P_2,
		P_3,
		P_4,
		P_5,
		P_6,
		P_7,
		P_8
	};
	typedef struct SCollisionDebugVertex
	{
		D3DXVECTOR3 position = { 0.0f,0.0f,0.0f };
		D3DXVECTOR4 color =    { 1.0f,1.0f,1.0f,1.0f };
	}SCollisionDebugVertex;
private:
	std::unique_ptr<JGMatrix> m_WorldMatrix;
	std::unique_ptr<JGMatrix> m_OthroMatrix;
	std::unique_ptr<JGMatrix> m_ViewMatrix;
	std::vector<std::shared_ptr<JGMesh>> m_vDebugMeshs;
	std::unique_ptr<struct SMatrixBuffer_VS>   m_MatrixBufferDesc;
	JGDeviceD* m_pDevice = nullptr;
	JGHLSLShaderDevice* m_pHLSLDevice = nullptr;
	JGBufferManager* m_pBufferManager = nullptr;
	JGViewportD*     m_pViewPort = nullptr;
public:
	JGCollisionDebugDraw();
	~JGCollisionDebugDraw();
	void InitCollisionDebugDraw(JGDeviceD* Device, HWND hWnd,JGHLSLShaderDevice* HLSLDevice, JGBufferManager* BufferManager, JGViewportD* viewPort,const JGMatrix& ViewMatrix);
	virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
	virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
	virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
	virtual void DrawTransform(const b2Transform& xf);
	virtual void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);
private:
	void InitDebugMesh();
	void AddInitPolygonMesh(const size_t PolygonCount);
	void MeshDraw(CollisionMeshType type);
};