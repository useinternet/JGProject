#include"JGColorShader.h"
#include"HLSLConstantBufferDesc.h"
#include"../JGHLSLShaderDevice/JGShaderCompiler.h"
#include"../JGHLSLShaderDevice/JGBuffer.h"
#include"../JGTexture.h"
#include"../JGDeviceD.h"
JGColorShader::JGColorShader()
{
}

JGColorShader::~JGColorShader()
{
}

bool JGColorShader::CreateShader()
{
	bool result = true;
	AddInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	AddInputElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	JGShaderCompiler Compiler(
		GetVertexShaderAddress(),
		GetPixelShaderAddress(),
		GetInputElements(),
		GetElementCount(),
		GetInputLayoutAddress(),
		EntryPoint,
		Version);
	result = Compiler.Compile(GetJGDevice(), TT("../JGEngineCore/RenderSystem/ShaderHLSL/ColorShader_vs.hlsl"),
		TT("../JGEngineCore/RenderSystem/ShaderHLSL/ColorShader_ps.hlsl"));
	if (!result)
	{
		return false;
	}
	// 상수 버퍼 추가
	AddConstantBuffer(TT("MatrixBuffer"), sizeof(SMatrixBuffer_VS));
	return true;
}

void JGColorShader::JGShaderDataSetting(JGShaderData * Data)
{
	Write_RegisterBuffer(TT("MatrixBuffer"), Data, EJGMapType::Write_Discard,
		ERegisterType::Vertex);
}


