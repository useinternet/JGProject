#pragma once
#include"../TestCommon/JGWindow.h"
#include"DirectXToolKit/Resource.h"
#include"DirectXToolKit/DirectXCore.h"
#include"DirectXToolKit/CommandQueue.h"
#include"DirectXToolKit/CommandList.h"
#include"DirectXToolKit/DxUtil.h"
#include"DirectXToolKit/DescriptorAllocation.h"
#include"DirectXToolKit/Camera.h"
#include"DirectXToolKit/GeometryGenerator.h"
#include"DirectXToolKit/IndexBuffer.h"
#include"DirectXToolKit/VertexBuffer.h"
#include"DirectXToolKit/RootSigature.h"
#include"DirectXToolKit/PSO.h"
#include"DirectXToolKit/SamplerState.h"
#include"DirectXToolKit/Viewport.h"
#include"DirectXToolKit/ScissorRect.h"
#include"DirectXToolKit/Texture.h"
#include"DirectXToolKit/DescriptorHeap.h"
#include"DirectXToolKit/BlendState.h"
#include"DirectXToolKit/DepthStencilState.h"
#include"DirectXToolKit/RasterizerState.h"
#include"DirectXToolKit/RenderTarget.h"
#include"DirectXToolKit/Screen.h"
#include"DirectXObjects/Mesh.h"
#include"DirectXObjects/Dx12Object.h"
#include"DirectXObjects/GBuffer.h"
#include"DirectXObjects/Scene.h"
#include<AssetManager.h>
#include<AssetAnimTransform.h>
#include"DirectXObjects/CubeMap.h"
using namespace Dx12;
using namespace Dx2D;
class HelloWindow : public JGWindow
{
	static const int BackBufferCount = 3;
private:
	std::shared_ptr<Screen> m_Screen;
	CommandQueue* m_CommandQueue;
	Camera m_Camera;
	PassCB m_PassCB;
	std::unique_ptr<GBuffer> m_GBuffer;
	std::unique_ptr<Scene> m_Scene;
	//
	Texture m_Texture;
	Texture m_SkyTexture;
	Texture m_GridTexture;
	Texture m_GridNormalTexture;
	//
	Dx12Object m_Object;
	Dx12Object m_GridObject;
	Dx12Object m_SkyObject;
	//
	int m_PrevMouseX;
	int m_PrevMouseY;
	//
	std::unique_ptr<CubeMap> m_CubeMap;
	//
public:
	HelloWindow(HINSTANCE hInst) : JGWindow(hInst) {}
	virtual ~HelloWindow() {
		Dx2DDevice::DestoryDevice();
		DxDevice::DestroyDevice();
	}
	virtual bool Initialize();
protected:
	virtual void OnResize(int width, int height) override;
	virtual void Update() override;
	virtual void Draw() override;

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;
	virtual void KeyDown(WPARAM wparam) override;
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
};