#pragma once
#include"../../EngineStatics/Engine.h"


class JGBufferManager;
class JGDeviceD;
enum class EJGBufferType;
enum class EJGUsageType;
enum class EJGCPUType;
enum class EJGMapType;

/*
EnumClass : ERenderingType 
@e PointList : 점 목록 타입
@e LineList : 선 목록 타입
@e LineStrip : 선 라인 스트립 타입 
@e TriangleList : 삼각형 목록 타입
@e TriangleStrip : 삼각형 스트립 */
enum class ENGINE_EXPORT ERenderingType
{
	PointList = 0,
	LineList = 1,
	LineStrip = 2,
	TriangleList = 3,
	TriangleStrip = 4
};
class ENGINE_EXPORT JGMesh
{
private:
	std::vector<UINT> m_Stride;
	std::vector<UINT> m_Offset;
	std::vector<ID3D11Buffer*> m_VertexBuffer;
	std::vector<ID3D11Buffer*> m_IndexBuffer;
	JGBufferManager* m_BufferManager = nullptr;
	bool bUseInstanceBuffer = false;
public:
	JGMesh(const JGMesh& copy) = delete;
	JGMesh();
	virtual ~JGMesh();

	/*
	Exp : 정점 / 인덱스 버퍼 생성 */
	bool Create_Vertex_Index_Buffer(JGBufferManager* BufferManager,const std::wstring& MeshName,
		EJGUsageType VertexUsageType, EJGCPUType VertexCPUType, EJGUsageType IndexUsageType, EJGCPUType IndexCPUType,
		void* VertexData,size_t DataCount, size_t DataTypeSize);

	/*
	Exp : 미구현 (추후 구현할 것)*/
	bool Create_Vertex_Instance_Buffer(JGBufferManager* BufferManager);

	void Render(JGDeviceD* Device, ERenderingType RenderingType);
};