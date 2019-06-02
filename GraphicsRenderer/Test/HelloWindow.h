#pragma once
#include"JGWindow.h"

#include"GpuApi/DirectX12/GraphicsDevice.h"
#include"GpuApi/DirectX12/PipelineState.h"
#include"GpuApi/DirectX12/RootSignature.h"
#include"GpuApi/DirectX12/GraphicsResource.h"
#include"GpuApi/DirectX12/Viewport.h"
#include"GpuApi/DirectX12/ScissorRect.h"
#include"GpuApi/DirectX12/Commander.h"
#include"GpuApi/DirectX12/GeometryGenerator.h"
#include"GpuApi/DirectX12/RenderTarget.h"
#include"GpuApi/DirectX12/Mesh.h"
#include"GpuApi/DirectX12/Camera.h"
#include"GpuApi/DirectX12/Renderer.h"
#include"GpuApi/DirectX12/RenderObject.h"
#include"GpuApi/DirectX12/RootSignatureCache.h"
#include"GpuApi/DirectX12/PSOCache.h"
#include<ppl.h>
#ifdef _DEBUG
// ../GraphicsRenderer/bin/GraphicsRenderer_d.lib
#pragma comment(lib, "../GraphicsRenderer/bin/GraphicsRenderer_d.lib")
#else
#pragma comment(lib, "../GraphicsRenderer/bin/GraphicsRenderer.lib")

#endif
using namespace GR::Dx12;
/*
1. 시작 커맨더 푸시
2. 각 커맨더로 그리기
3. 끝 커맨더 푸시




*/
class HelloWindow : public JGWindow
{
	std::unique_ptr<GraphicsDevice> m_Device;
	Renderer* m_Renderer;
	RenderObject m_Gun;
	RenderObject m_XBot1;
	RenderObject m_XBot2;

	Texture m_AlbedoTexture;
	Texture m_NormalTexture;
	Texture m_GunTexture_A;
	Texture m_GunTexture_N;
	Texture m_GunTexture_R;
	Texture m_GunTexture_M;
	Texture m_HDRSkyTexture;


	int m_LastMouseX;
	int m_LastMouseY;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float3 CameraPos;
public:
	HelloWindow(HINSTANCE hInst) : JGWindow(hInst) {}
	virtual bool Initialize();
protected:
	virtual void OnResize(int width, int height) override;
	virtual void Update() override;
	virtual void Draw() override;

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;
	void Input();
private:
	ObjectCB UpdateObjectCB(float x = 0, float y = 0, float z = 0);
};