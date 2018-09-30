#pragma once
#include"JGMesh.h"

class JGBufferManager;
class JGFontVertexInformation;

/*
EnumClass : EPivot
@e TopLeft      : 왼쪽위
@e TopMiddle    : 중간위
@e TopRight     : 오른쪽위
@e MiddleLeft   : 왼쪽중간
@e MiddleMiddle : 가운데
@e MiddleRight  : 오른쪽중간
@e BottomLeft   : 왼쪽아래
@e BottomMiddle : 중간아래
@e BottomRight  : 오른쪽아래 */
enum class ENGINE_EXPORT EPivot
{
	TopLeft      = 0,
	TopMiddle    = 1,
	TopRight     = 2,
	MiddleLeft   = 3,
	MiddleMiddle = 4,
	MiddleRight  = 5,
	BottomLeft   = 6,
	BottomMiddle = 7,
	BottomRight  = 8
};

/*
Class : JG2DMesh */
class ENGINE_EXPORT JG2DMesh : public JGMesh
{
private:
	const UINT m_2DVertexCount = 6;
public:
	typedef struct S2DVertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 tex;
	}S2DVertexType;
	typedef struct SPivotPoint
	{
		float x;
		float y;
	}SPivotPoint;
	typedef struct STexInformaton
	{
		SPivotPoint TopLeft = { 0.0f,0.0f };
		float Width = 1.0f;
		float Height = 1.0f;
		
	}STexInformaton;
private:
	std::vector<S2DVertexType> m_vVertexArray;
	EPivot m_Pivot;
	float m_MeshWidth = 0.0f;
	float m_MeshHeight = 0.0f;
	bool  m_ChangingMesh = false;
public:
	JG2DMesh();
	virtual ~JG2DMesh();
	/*
	Exp : 2D 용 메쉬를 생성한다. 
	@param JGBufferManager* BufferManager : 버퍼 매니저 포인터
	@param const wstring& ComponentName  : 컴포넌트 이름
	@param const float Width  : 텍스쳐 가로길이
	@param const float Height : 텍스쳐 세로길이
	@param EPivot Pivot : 중심 위치 */
	bool Construct2DMesh(JGBufferManager* BufferManager,const std::wstring& ComponentName, 
		const float Width, const float Height,EPivot Pivot = EPivot::TopLeft,const STexInformaton& TexInformation = STexInformaton(),
		const EJGUsageType UsageType = EJGUsageType::Static);
	/*
	Exp : 텍스트용 메쉬를 생성한다. 
	@param JGBufferManager* BufferManager : 버퍼 매니저 포인터
	@param const wstring& ComponentName  : 컴포넌트 이름
	@param const vector<JGFontVertexInformation>& VertexInformation : 폰트 정점 정보
	@param EPivot Pivot : 중심 위치 */
	bool ConstructTextMesh(JGBufferManager* BufferManager, const std::wstring& ComponentName,const std::vector<JGFontVertexInformation>& VertexInformation, EPivot Pivot = EPivot::TopLeft);
	/*
	Exp : 정점 갯수를 가져온다. */ //( 이거 메시로 옮긴다. )
	size_t GetIndexCount();

	/*
	Exp : 메시 Pivot을 가져온다.*/
	EPivot GetPivot();
	/*
	Exp : 메시 가로길이를 가져온다.*/
	float GetMeshWidth();
	/*
	Exp : 메시 세로길이를 가져온다.*/
	float GetMeshHeight();
	//
	void MeshReverse(const EReverse reverseType);
	// 렌더링
	virtual void Render(JGDeviceD* Device, ERenderingType RenderingType);
	/*
	Exp : 구현중*/
	void WriteTextMesh(const std::vector<JGFontVertexInformation>& VertexInformation);
private:
	void CreateVertexArray(const float Width, const float Height, EPivot Pivot, const STexInformaton& TexInformation, float AccX = 0.0f,float AccY = 0.0f);
};
