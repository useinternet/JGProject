#pragma once
#include"../EngineStatics/Engine.h"
// D3D11_VIEWPORT
// float m_fFov;
// float m_fFarZ;
// float m_fNearZ;
// 각종 worldMatrix, projectionMatrix, orthMatrix

class JGMatrix;


/*
Class : JGViewportD 
@m unique_ptr<D3D11_VIEWPORT> m_Viewport   : D3D11_VIEWPORT 구조체 
@m unique_ptr<JGMatrix> m_projectionMatrix : 투영 행렬 
@m unique_ptr<JGMatrix> m_orthoMatrix      : 정사영 행렬 
@m float m_FOV    : FieldOfView (시야각) 
@m float m_FarZ   : 렌더링 최대 거리
@m float m_NearZ  : 렌더링 최단 거리 
@m float m_Width  : 뷰포트 가로길이
@m float m_Height : 뷰포트 세로길이 */
class ENGINE_EXPORT JGViewportD
{
private:
	std::unique_ptr<D3D11_VIEWPORT> m_Viewport;
	std::unique_ptr<JGMatrix>       m_projectionMatrix;
	std::unique_ptr<JGMatrix>       m_orthoMatrix;
	float m_FOV    = 0.0f;
	float m_FarZ   = 0.0f;
	float m_NearZ  = 0.0f;
	static float m_Width;
	static float m_Height;
public:
	JGViewportD();
	~JGViewportD();
	/*
	Exp : 뷰포트를 초기화 한다. 
	@param const int Width   : 가로 길이
	@param const int Height  : 세로 길이
	@param const float FOV   : 시야각
	@param const float FarZ  : 렌더링 최대 거리
	@param const float NearZ : 렌더링 최소 거리  */
	bool InitViewport(const int Width, const int Height, const float FOV, const float FarZ, const float NearZ);

	/*
	Exp : 뷰포트를 가져온다. */
	D3D11_VIEWPORT* Get();
	/*
	Exp : 투영 행렬을 가져온다. */
	JGMatrix& GetProjectionMatrix();
	/*
	Exp : 정사영 행렬을 가져온다. */
	JGMatrix& GetOrthoMatrix();

	/*
	Exp : 시야각을 가져온다. */
	const float GetFOV();
	/*
	Exp : 가로길이를 가져온다. */
	static const float GetWidth();
	/*
	Exp : 세로길이를 가져온다. */
	static const float GetHeight();

	/*
	Exp : 시야각을 셋팅한다. */
	void SetFOV(const float FOV);
private:
	void CreateViewport(const float MinDepth = 0.0f, const float MaxDepth = 1.0f, 
		const float TopLeftX = 0.0f, const float TopLeftY = 0.0f);
	void CreateProjectionMatrix();
	void CreateOrthoMatrix();
};