#include"TestShader.h"
#include"MaterialSystem/Shader/ShaderAnalyze/ShaderAnalyzer.h"
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
	//VSAyzer = make_unique<ShaderAnalyzer>();
	//PSAyzer = make_unique<ShaderAnalyzer>();
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
	/*VSAyzer->Analyze("./HLSL/CommonShader_vs.hlsl",EShaderType::Vertex);
	PSAyzer->Analyze("./HLSL/CommonShader_ps.hlsl", EShaderType::Pixel);

	VertexShader = ShaderObjectManager::GetInstance()->CreateShaderObject(EShaderType::Vertex);
	PixelShader = ShaderObjectManager::GetInstance()->CreateShaderObject(EShaderType::Pixel);

	InputLayout ly;
	VSAyzer->MakeInputLayoutArray(&ly);

	ShaderCompiler init;
	init.hWnd = hwnd;
	ShaderCompiler VertexCompile(TT("./HLSL/CommonShader_vs.hlsl"), VertexShader,&ly);
	ShaderCompiler PixelCompile(TT("./HLSL/CommonShader_ps.hlsl"), PixelShader,nullptr);


	VSAyzer->CreateConstantBuffers();
	PSAyzer->CreateConstantBuffers();


	MatrixBuffer = JGBufferManager::GetInstance()->CreateBuffer(EBufferType::ConstantBuffer,
		EUsageType::Dynamic, ECPUType::Access_Write, nullptr, sizeof(real), 48);
	LightBuffer = JGBufferManager::GetInstance()->CreateBuffer(EBufferType::ConstantBuffer,
		EUsageType::Dynamic, ECPUType::Access_Write, nullptr, sizeof(real), 12);

	SamplerState::GetInstance()->AddSampler(TT("Wrap"), ESamplerType::Wrap, D3D11_FILTER_MIN_MAG_MIP_LINEAR);*/
	return true;
}
bool TestShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, jgMatrix4x4 worldMatrix, jgMatrix4x4 viewMatrix,
	jgMatrix4x4 projectionMatrix, ID3D11ShaderResourceView* texture)
{

	worldMatrix.transpose();
	viewMatrix.transpose();
	projectionMatrix.transpose();

	//VSAyzer->SetParam("worldMatrix", &worldMatrix);
	//VSAyzer->SetParam("viewMatrix", &viewMatrix);
	//VSAyzer->SetParam("projectionMatrix", &projectionMatrix);
	//VSAyzer->WriteConstantBuffers();

	//jgVec4 Color(0.15f, 0.15f, 0.15f, 1.0f);
	//PSAyzer->SetParam("ambientColor", &Color);
	//Color.set(1.0f, 1.0f, 1.0f, 1.0f);
	//PSAyzer->SetParam("diffuseColor", &Color);
	//jgVec3 d(1.0f, 0.0f, 0.0f);
	//PSAyzer->SetParam("lightDirection", &d);
	//real pad = 0.0f;
	//PSAyzer->SetParam("padding", &pad);
	//PSAyzer->WriteConstantBuffers();

	return true;
}
void TestShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//// Set the vertex input layout.
	//deviceContext->IASetInputLayout(VertexShader->Layout);

	//// Set the vertex and pixel shaders that will be used to render this triangle.
	//deviceContext->VSSetShader((ID3D11VertexShader*)(VertexShader->ShaderBuffer), NULL, 0);
	//deviceContext->PSSetShader((ID3D11PixelShader*)(PixelShader->ShaderBuffer), NULL, 0);

	//// Set the sampler state in the pixel shader.
	//deviceContext->PSSetSamplers(0, 1, SamplerState::GetInstance()->GetAddress(TT("Wrap")));

	//// Render the triangle.
	//deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}