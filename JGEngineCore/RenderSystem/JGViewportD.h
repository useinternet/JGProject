#pragma once
#include"../EngineStatics/Engine.h"
// D3D11_VIEWPORT
// float m_fFov;
// float m_fFarZ;
// float m_fNearZ;
// °¢Á¾ worldMatrix, projectionMatrix, orthMatrix

class JGMatrix;

class ENGINE_EXPORT JGViewportD
{
private:
	std::unique_ptr<D3D11_VIEWPORT> m_Viewport;
	std::unique_ptr<JGMatrix>       m_projectionMatrix;
	std::unique_ptr<JGMatrix>       m_orthoMatrix;
	float m_FOV    = 0.0f;
	float m_FarZ   = 0.0f;
	float m_NearZ  = 0.0f;
	float m_Width  = 0.0f;
	float m_Height = 0.0f;
public:
	JGViewportD();
	~JGViewportD();
	bool InitViewport(const int Width, const int Height, const float FOV, const float FarZ, const float NearZ);
	D3D11_VIEWPORT* Get();
	JGMatrix& GetProjectionMatrix();
	JGMatrix& GetOrthoMatrix();


	const float GetFOV();
	const float GetWidth();
	const float GetHeight();
	void SetFOV(const float FOV);
private:
	void CreateViewport(const float MinDepth = 0.0f, const float MaxDepth = 1.0f, 
		const float TopLeftX = 0.0f, const float TopLeftY = 0.0f);
	void CreateProjectionMatrix();
	void CreateOrthoMatrix();
};