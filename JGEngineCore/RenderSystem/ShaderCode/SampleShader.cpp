#include"SampleShader.h"
#include"../JGDeviceD.h"
#include"../JGHLSLShaderDevice/JGHLSLShaderDevice.h"
#include"../JGHLSLShaderDevice/JGShaderCompiler.h"
#include"../JGHLSLShaderDevice/JGBufferManager.h"
#include"../JGHLSLShaderDevice/JGBuffer.h"
#include"../JGTexture.h"
SampleShader::SampleShader()
{

}
SampleShader::~SampleShader()
{

}
bool SampleShader::CreateShader()
{
	bool result = true;
	AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	AddInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);

	JGShaderCompiler Compiler(
		GetVertexShaderAddress(),
		GetPixelShaderAddress(),
		GetInputElements(),
		GetElementCount(),
		GetInputLayoutAddress(),
		EntryPoint,
		Version);
	result = Compiler.Compile(GetJGDevice(),TT("../JGEngineCore/RenderSystem/ShaderHLSL/SampleShader_vs.hlsl"),
		TT("../JGEngineCore/RenderSystem/ShaderHLSL/SampleShader_ps.hlsl"));
	if (!result)
	{
		return false;
	}
	// 상수 버퍼 추가
	AddConstantBuffer(TT("MatrixBuffer"), sizeof(SMatrixBuffer));
	// 샘플러 추가
	AddSamplerState(GetJGDevice(), TT("WrapSampler"), ESamplerType::Wrap);

	return true;
}
void SampleShader::JGShaderDataSetting(JGShaderData* Data)
{
	Write_RegisterBuffer(TT("MatrixBuffer"), Data, EJGMapType::Write_Discard, ERegisterType::Vertex);
}
void SampleShader::JGTextureSetting(JGTexture* Texture)
{
	GetJGDevice()->GetContext()->PSSetShaderResources(0, 1, Texture->GetEntry());
}
