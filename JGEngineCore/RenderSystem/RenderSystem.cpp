#include"RenderSystem.h"
#include"JGDeviceD.h"
#include"VideoDeviceInfo.h"
#include"JGSwapChainD.h"
#include"JGRenderTargetD.h"
#include"JGRenderSystemStateD.h"
#include"JGViewportD.h"
#include"../EngineStatics/JGLog.h"
#include"JGHLSLShaderDevice/JGBufferManager.h"
#include"JGHLSLShaderDevice/JGHLSLShaderDevice.h"
#include"JGShaderConstructor.h"


// 임시 인클루드
#include"JGHLSLShaderDevice/JGBuffer.h"
#include"JGTexture.h"
#include"ShaderCode/SampleShader.h"
#include"JGMaterial/JGMesh.h"

using namespace std;

RenderSystem::RenderSystem()
{
	m_Device       = make_unique<JGDeviceD>();
	m_SwapChain    = make_unique<JGSwapChainD>();
	m_RenderTarget = make_unique<JGRenderTargetD>();
	m_RenderState  = make_unique<JGRenderSystemStateD>();
	m_Viewport     = make_unique<JGViewportD>();
	m_JGBufferManager = make_unique<JGBufferManager>();
	m_ShaderDevice = make_unique<JGHLSLShaderDevice>();
	m_ShaderConstructor = make_unique<JGShaderConstructor>();
}
RenderSystem::~RenderSystem() { delete m_Mesh;
	delete texture;
}
bool RenderSystem::InitRenderSystem(HWND hWnd, const bool bFullScreen,const int ScreenWidth, const int ScreenHeight,
	const float FOV, const float FarZ, const float NearZ)
{
	// 변수 초기화
	m_hWnd = hWnd;
	m_bFullScreen  = bFullScreen;
	m_ScreenWidth  = ScreenWidth;
	m_ScreenHeight = ScreenHeight;
	bool result = true;
	// 로그 출력( 시작 )
	JGLog::Write(ELogLevel::Progress, TT("RenderSystem Init Start...."));

	// 장치 생성
	JGLog::Write(ELogLevel::Default, TT("JGDeviceD Init Start...."));
	result = m_Device->CreateDevice();
	if (!result)
	{
		JGLog::Write(ELogLevel::Error, TT("Create JGDeviceD Failed."));
		return false;
	}
	JGLog::Write(ELogLevel::Default, TT("JGDeviceD Init Complete...."));

	// 스왑체인 생성
	JGLog::Write(ELogLevel::Default, TT("SwapChain Init Start...."));
	result = m_SwapChain->CreateSwapChain(m_Device->GetDevice(), m_hWnd, m_bFullScreen, m_ScreenWidth, m_ScreenHeight);
	if (!result)
	{
		JGLog::Write(ELogLevel::Error, TT("Create JGSwapChainD Failed."));
		return false;
	}
	JGLog::Write(ELogLevel::Default, TT("SwapChain Init Complete...."));
	// 로그 출력 생성 완료

	// 렌더 타겟 생성
	JGLog::Write(ELogLevel::Default, TT("MainJGRenderTargetD Init Start...."));
	result = m_RenderTarget->CreateRenderTarget(m_Device->GetDevice(), m_SwapChain->Get(), m_ScreenWidth, m_ScreenHeight);
	if (!result)
	{
		JGLog::Write(ELogLevel::Error, TT("Create MainJGRenderTargetD Failed."));
		return false;
	}
	JGLog::Write(ELogLevel::Default, TT("MainJGRenderTargetD Init Complete...."));

	// 렌더시스템 상태 생성
	JGLog::Write(ELogLevel::Default, TT("JGRenderSystemStateD Init Start...."));
	result = m_RenderState->InitRenderSystemSetting(m_Device->GetDevice());
	if (!result)
	{
		JGLog::Write(ELogLevel::Error, TT("Create JGRenderSystemStateD Failed."));
		return false;
	}
	JGLog::Write(ELogLevel::Default, TT("JGRenderSystemStateD Init Complete...."));


	// 뷰포트 생성
	JGLog::Write(ELogLevel::Default, TT("JGMainViewport Init Start...."));
	result = m_Viewport->InitViewport(m_ScreenWidth, m_ScreenHeight, FOV, FarZ, NearZ);
	if (!result)
	{
		JGLog::Write(ELogLevel::Error, TT("Create JGMainViewport Failed."));
		return false;
	}
	JGLog::Write(ELogLevel::Default, TT("JGMainViewport Init Complete...."));


	JGLog::Write(ELogLevel::Default, TT("RenderSystemAssist Connecting..."));
	 //버퍼 매니저 초기화
	m_JGBufferManager->InitBufferManager(m_Device.get());
	// 셰이더 시스템 초기화
	m_ShaderDevice->CreateDevice(m_Device.get(), m_JGBufferManager.get());
	 //셰이더 생성 보조자와 셰이더 시스템과 연결
	m_ShaderConstructor->LinkHLSLDevice(m_ShaderDevice.get());
	JGLog::Write(ELogLevel::Default, TT("RenderSystemAssist ConnectComplete..."));


	// 임시
	// 임시 적용
	ApplicationInDeviceContext();

	//임시 함수
	TestFunc(hWnd);
	//
	JGLog::Write(ELogLevel::Progress, TT("RenderSystem Init Complete...."));
	return true;
}

void RenderSystem::BeginRendering()
{
	float Color[4] = { 0.0f,0.0f,0.0f,0.0f};
	m_Device->GetContext()->ClearRenderTargetView(m_RenderTarget->Get(), Color);
	m_Device->GetContext()->ClearDepthStencilView(m_RenderTarget->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void RenderSystem::EndRendering()
{
	m_SwapChain->Get()->Present(0, 0);
}
void RenderSystem::Render()
{
	D3DXMATRIX worldMatrix;
	D3DXMATRIX projectionMatrix;
	D3DXMATRIX viewMatrix = ViewMatrix;
	m_Viewport->GetProjectionMatrix(&projectionMatrix);
	D3DXMatrixIdentity(&worldMatrix);

	
	m_Mesh->Render(m_Device.get(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	SMatrixBuffer desc;
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &ViewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);
	desc.worldMatrix      = worldMatrix;
	desc.viewMatrix       = viewMatrix;
	desc.projectionMatirx = projectionMatrix;

	JGShaderData data;
	data.InsertData(TT("MatrixBuffer"), (void*)&desc);
	//셰이더 렌더링
	m_ShaderDevice->Render(TT("SampleShader"), &data,
		texture, (UINT)6);

}

void RenderSystem::TestFunc(HWND hWnd)
{

	texture = new JGTexture();
	texture->Add(m_Device.get(), TT("../ManagementFiles/Resource/Sample.jpg"));
	VertexType* vertices;
	UINT* indices;

	// Set the number of vertices in the vertex array.
	UINT m_vertexCount = 6;

	// Set the number of indices in the index array.
	UINT m_indexCount = 6;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	// Create the index array.
	indices = new UINT[m_indexCount];

	// Load the vertex array with data.
	vertices[0].position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);  // Bottom left.
	vertices[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	vertices[1].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  // Top middle.
	vertices[1].tex = D3DXVECTOR2(1.0f, 1.0f);
	vertices[2].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	vertices[3].position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);  // Bottom left.
	vertices[3].tex = D3DXVECTOR2(0.0f, 0.0f);
	vertices[4].position = D3DXVECTOR3(1.0f, 1.0f, 0.0f);  // Top middle.
	vertices[4].tex = D3DXVECTOR2(1.0f, 0.0f);
	vertices[5].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[5].tex = D3DXVECTOR2(1.0f, 1.0f);
	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.
	indices[3] = 3;  // Bottom left.
	indices[4] = 4;  // Top middle.
	indices[5] = 5;  // Bottom right.

	JGMesh* mesh = new JGMesh;
	mesh->Create_Vertex_Index_Buffer(
		m_JGBufferManager.get(), TT("Sample"), EJGUsageType::Static,
		EJGCPUType::None, EJGUsageType::Static, EJGCPUType::None,
		vertices, 6, sizeof(VertexType));
	m_Mesh = mesh;

	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	//
	D3DXVECTOR3 up, position, lookAt; 
	float yaw, pitch, roll; 
	D3DXMATRIX rotationMatrix; 
	// Setup the vector that points upwards.
	up.x = 0.0f; up.y = 1.0f; up.z = 0.0f; 
	// Setup the position of the camera in the world.
	position.x = 0;
	position.y = 0; 
	position.z = -10.0f; 
	// Setup where the camera is looking by default.
	lookAt.x = 0.0f; lookAt.y = 0.0f; lookAt.z = 1.0f;
	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians. 
	pitch = 0 * 0.0174532925f; 
	yaw = 0 * 0.0174532925f; 
	roll = 0 * 0.0174532925f; 
	// Create the rotation matrix from the yaw, pitch, and roll values. 
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);
	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin. 
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix); 
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix); 
	// Translate the rotated camera position to the location of the viewer. lookAt = position + lookAt; // Finally create the view matrix from the three updated vectors. 
	D3DXMatrixLookAtLH(&ViewMatrix, &position, &lookAt, &up);
}
void RenderSystem::ApplicationInDeviceContext()
{
	// 임시 적용
	m_Device->GetContext()->OMSetRenderTargets(1, m_RenderTarget->GetAddress(),
		m_RenderTarget->GetDepthStencilView());
	m_Device->GetContext()->OMSetDepthStencilState(m_RenderState->GetDepthState(EDepthStateType::ZBufferOn), 1);
	m_Device->GetContext()->RSSetState(m_RenderState->GetRasterizerState(EFillModeType::Solid));
	m_Device->GetContext()->RSSetViewports(1, m_Viewport->Get());

}
