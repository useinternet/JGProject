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
#include"GpuApi/DirectX12/GPUAllocator.h"
#include"imgui/imgui.h"
#include"imgui/imgui_impl_dx12.h"
#include"imgui/imgui_impl_win32.h"
#include<ppl.h>
#ifdef _DEBUG
// ../GraphicsRenderer/bin/GraphicsRenderer_d.lib
#pragma comment(lib, "../GraphicsRenderer/bin/GraphicsRenderer_d.lib")
#else
#pragma comment(lib, "../GraphicsRenderer/bin/GraphicsRenderer.lib")

#endif
using namespace GR::Dx12;
/*

1. gpuallocator ø° mutex√ﬂ∞°(multi thread)




*/
struct SkyTexturePack
{
	Texture sky;
	Texture spMap;
	Texture irrMap;
	Texture spBrdf;
};
class HelloWindow : public JGWindow
{
public:
	GPUAllocation m_FontGPU;
	GPUAllocation m_GBufferGPU;
	GPUAllocation m_ToneMapGpu;
private:
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

	std::vector<SkyTexturePack> m_SkyPack;
	uint64_t m_TextureID;


	int m_LastMouseX;
	int m_LastMouseY;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float3 CameraPos;



	std::string m_CurrentItem = "0";
	bool m_isToneMapping = true;
public:
	HelloWindow(HINSTANCE hInst) : JGWindow(hInst) {}
	virtual bool Initialize();
protected:
	virtual void OnResize(int width, int height) override;
	virtual void Update() override;
	virtual void Draw() override;

	// Convenience overrides for handling mouse input.
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
	void Input();
public:
	virtual void Destroy() override;
	void UI();
private:
	ObjectCB UpdateObjectCB(float x = 0, float y = 0, float z = 0);
	void BindIBLTexture(SkyTexturePack& t);
};