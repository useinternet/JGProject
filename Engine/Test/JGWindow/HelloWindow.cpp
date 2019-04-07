#include"HelloWindow.h"
#include<wincodec.h>
#include"DirectXToolKit/Shader.h"
using namespace std;
using namespace DirectX;
using namespace Common;
bool HelloWindow::Initialize()
{
	if (!JGWindow::Initialize())
		return false;
	DxDevice::CreateDevice(DX12_LOG_PATH);

	m_CommandQueue = DxDevice::GetCommandQueue();
	m_Viewport.Set((float)mClientWidth, (float)mClientHeight);
	m_Rect.Set(mClientWidth, mClientHeight);

	auto commandList = m_CommandQueue->GetCommandList();


	m_Screen = make_unique<Screen>();
	m_Screen->InitScreen(mhMainWnd, mClientWidth, mClientHeight);

	// 도화지 생성
	{
		D3D12_CLEAR_VALUE clearColor;
		clearColor.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		clearColor.Color[0] = 0.0f;
		clearColor.Color[1] = 0.0f;
		clearColor.Color[2] = 0.0f;
		clearColor.Color[3] = 1.0f;
		MainBuffer = move(Texture(
			TextureUsage::RenderTarget,
			CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, mClientWidth, mClientHeight, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
			&clearColor,
			"MainBuffer"));


		D3D12_CLEAR_VALUE clearValue;
		clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;
		DepthBuffer = move(Texture(
			TextureUsage::DepthStencil,
			CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D24_UNORM_S8_UINT, mClientWidth, mClientHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
			&clearValue, "DepthBuffer"));
	}
	// 메쉬 생성
	{
		m_SkyObject.SetMesh(GeometryGenerator::CreateBox(commandList,
			10000.0f, 10000.0f, 10000.0f, 1));
		m_SkyObject.Build(commandList);
		m_SkyObject.AddObjectFragments();
		


		m_Object.Build(commandList);
		m_InstanceObj1 = m_Object.AddObjectFragments();
	
	
	}
	{
		GraphicsShader skyshader;
		skyshader.AddShaderPaths({ ShaderStage::Vertex, ShaderStage::Pixel },
			GLOBAL_SHADER_PATH("SkyBox.hlsl"));
		skyshader.Compile();

		m_SkyPSO.SetRasterizerState(RasterizerState::CullNone());
		m_SkyPSO.SetDepthStencilState(DepthStencilState::LessEqual());
		m_SkyPSO.SetRenderTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D24_UNORM_S8_UINT);
		m_SkyPSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		m_SkyPSO.SetRootSig(DxDevice::GetShaderCommonDefines()->GetMainRootSig().Get());
		m_SkyPSO.SetVertexShader(skyshader);
		m_SkyPSO.SetPixelShader(skyshader);
		m_SkyPSO.SetInputLayout(JgVertex::GetNumInputLayout(), JgVertex::GetInputLayoutDesc());
		m_SkyPSO.Finalize();
	}
	{
		m_Camera.SetLens(0.25f * JG_PI, (float)mClientWidth, (float)mClientHeight, 1.0f, 100000.0f);
		m_Camera.SetPosition({ 0.0f, 0.0f, -30.0f });
	}
	{
		commandList->LoadTextureFromFile(GLOBAL_TEXTURE_PATH("UV_Test_Pattern.png"), m_Texture, TextureUsage::Albedo);
		commandList->LoadTextureFromFile(GLOBAL_TEXTURE_PATH("grasscube1024.dds"), m_SkyTexture, TextureUsage::Albedo);
	}
	commandList->Close();

	m_CommandQueue->ExcuteCommandList(commandList);
	m_CommandQueue->Flush();



	auto cmdList = DxDevice::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE)->GetCommandList();

	cmdList->GenerateMipMaps(m_Texture);
	cmdList->Close();
	DxDevice::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE)->ExcuteCommandList(cmdList);
	DxDevice::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE)->Flush();

	return true;
}
void HelloWindow::OnResize(int width, int height)
{
	
	JGWindow::OnResize(width, height);

	m_Screen->Resize(width, height);
	MainBuffer.Resize(width, height);
	DepthBuffer.Resize(width, height);

	m_Camera.SetLens(0.25f * JG_PI, (float)width, (float)height, 1.0f, 100000.0f);
	m_Viewport.Set((float)width, (float)height);
	m_Rect.Set(width, height);
	DxDevice::Flush();
}
void HelloWindow::Update()
{


	m_Object.Update();
	m_SkyObject.Update();
	m_PassCB.ViewProj = m_Camera.GetHlslMatrix().Get();
	m_PassCB.ToEye = m_Camera.GetPosition().Get();
	m_CommandQueue->Flush();
}
void HelloWindow::Draw()
{
	auto commandList = m_CommandQueue->GetCommandList();


	Dx12::RenderTarget rendertarget;
	rendertarget.AttachTexture(RtvSlot::Slot_0, MainBuffer);
	rendertarget.AttachTexture(RtvSlot::DepthStencil, DepthBuffer);

	
	rendertarget.SetRenderTargetClearColor(RtvSlot::Slot_0, { 0.0f,0.0f,0.0f,1.0f });


	commandList->SetViewport(m_Viewport);
	commandList->SetScissorRect(m_Rect);
	commandList->ClearRenderTarget(rendertarget);
	commandList->SetRenderTarget(rendertarget);
	commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->SetGraphicsRootSignature(DxDevice::GetShaderCommonDefines()->GetMainRootSig());
	commandList->SetGraphicsConstantBufferView(CommonRootParam::PassCB, &m_PassCB);
	commandList->SetGraphicsDescriptorTables(CommonRootParam::Texture, 1, &m_Texture.GetShaderResourceView());


	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = m_SkyTexture.GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = m_SkyTexture.GetDesc().MipLevels;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;


	commandList->SetGraphicsDescriptorTables(CommonRootParam::CubeMap, 1, &m_SkyTexture.GetShaderResourceView(&srvDesc));

	m_SkyObject.Draw(commandList,m_SkyPSO);
	m_Object.Draw(commandList);


	commandList->Close();
    m_CommandQueue->ExcuteCommandList(commandList);
    m_CommandQueue->Flush();


	m_Screen->Present(MainBuffer);
}

void HelloWindow::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_PrevMouseX = x;
	m_PrevMouseY = y;

	SetCapture(mhMainWnd);
}
void HelloWindow::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}
void HelloWindow::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - m_PrevMouseX));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - m_PrevMouseY));
		
		m_Camera.RotationPitch(dy * 30.0f);
		m_Camera.RotationYaw(dx * 30.0f);
	}

	m_PrevMouseX = x;
	m_PrevMouseY = y;
}
void HelloWindow::KeyDown(WPARAM wparam) 
{
	static bool s = false;
	switch (wparam)
	{
	case 'W':
		//m_Camera.FowardMove(3.0f);
		m_InstanceObj1->GetTransform()._Rotation().OffsetPitch(3.0f);
		break;

	case 'S':
		//m_Camera.FowardMove(-3.0f);
		m_InstanceObj1->GetTransform()._Rotation().OffsetPitch(-3.0f);
		break;

	case 'A':
		//m_Camera.RightMove(-3.0f);
		m_InstanceObj1->GetTransform()._Rotation().OffsetYaw(3.0f);
		break;

	case 'D':
		//m_Camera.RightMove(3.0f);
		m_InstanceObj1->GetTransform()._Rotation().OffsetYaw(-3.0f);
		break;

	case 'E':
		m_Camera.UpMove(3.0f);
		break;

	case 'Q':
		m_Camera.UpMove(-3.0f);
		break;
	}
	
}

LRESULT HelloWindow::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	return JGWindow::MsgProc(hwnd, msg, wParam, lParam);
}