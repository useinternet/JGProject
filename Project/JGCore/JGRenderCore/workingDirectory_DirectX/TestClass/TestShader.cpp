#include"TestShader.h"
#include"MaterialSystem/Shader/ShaderTool/InputLayout.h"
#include"MaterialSystem/Shader/ShaderTool/SamplerState.h"
#include"MaterialSystem/Shader/ShaderTool/ShaderCompiler.h"
#include"MaterialSystem/Shader/ShaderTool/ShaderObject.h"
#include"MaterialSystem/Shader/ShaderTool/ShaderObjectManager.h"
#include"BufferSystem/JGBuffer.h"
#include"BufferSystem/JGBufferManager.h"
using namespace std;
using namespace JGRC;
TestShader::TestShader()
{
	real Light[12] = { 0.15f, 0.15f, 0.15f, 1.0f ,1.0f, 1.0f, 1.0f, 1.0f ,0.0f, 0.0f, 1.0f };
	for (int i = 0; i < 12; ++i)
	{
		LightArray.push_back(Light[i]);
	}
}
TestShader::~TestShader()
{

}
bool TestShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;


	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"./HLSL/CommonShader_vs.hlsl", L"./HLSL/CommonShader_ps.hlsl");
	if (!result)
	{
		return false;
	}

	return true;
}
bool TestShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, jgMatrix4x4 worldMatrix, jgMatrix4x4 viewMatrix,
	jgMatrix4x4 projectionMatrix, ID3D11ShaderResourceView* texture)
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}
bool TestShader::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
	VertexShader = ShaderObjectManager::GetInstance()->CreateShaderObject(EShaderType::Vertex);
	PixelShader = ShaderObjectManager::GetInstance()->CreateShaderObject(EShaderType::Pixel);
	InputLayout::AddInputLayout("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA,
		0);
	InputLayout::AddInputLayout("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA,
		0);
	InputLayout::AddInputLayout("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA,
		0);


	ShaderCompiler init;
	init.hWnd = hwnd;
	ShaderCompiler VertexCompile(TT("./HLSL/CommonShader_vs.hlsl"), VertexShader);
	ShaderCompiler PixelCompile(TT("./HLSL/CommonShader_ps.hlsl"), PixelShader);


	MatrixBuffer = JGBufferManager::GetInstance()->CreateBuffer(EBufferType::ConstantBuffer,
		EUsageType::Dynamic, ECPUType::Access_Write, nullptr, sizeof(MatrixBufferType), 1);
	LightBuffer = JGBufferManager::GetInstance()->CreateBuffer(EBufferType::ConstantBuffer,
		EUsageType::Dynamic, ECPUType::Access_Write, nullptr, sizeof(real), 12);

	SamplerState::GetInstance()->AddSampler(TT("Wrap"), ESamplerType::Wrap, D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	return true;
}
bool TestShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, jgMatrix4x4 worldMatrix, jgMatrix4x4 viewMatrix,
	jgMatrix4x4 projectionMatrix, ID3D11ShaderResourceView* texture)
{
	MatrixBufferType dataPtr;
	unsigned int bufferNumber = 0;

	worldMatrix.transpose();
	viewMatrix.transpose();
	projectionMatrix.transpose();

	dataPtr.world = worldMatrix;
	dataPtr.view = viewMatrix;
	dataPtr.projection = projectionMatrix;


	MatrixBuffer->Write(EMapType::Write_Discard, &dataPtr);

	LightBuffer->Write(EMapType::Write_Discard, &LightArray[0]);

	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, MatrixBuffer->GetAddress());

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, LightBuffer->GetAddress());
	return true;
}
void TestShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(VertexShader->Layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader((ID3D11VertexShader*)(VertexShader->ShaderBuffer), NULL, 0);
	deviceContext->PSSetShader((ID3D11PixelShader*)(PixelShader->ShaderBuffer), NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, SamplerState::GetInstance()->GetAddress(TT("Wrap")));

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}