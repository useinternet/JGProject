#include "JGTextureShader.h"
#include "HLSLConstantBufferDesc.h"
#include"../JGHLSLShaderDevice/JGShaderCompiler.h"
#include"../JGHLSLShaderDevice/JGBuffer.h"
#include"../JGDeviceD.h"
#include"../JGTexture.h"
JGTextureShader::JGTextureShader()
{
}

JGTextureShader::~JGTextureShader()
{
}

bool JGTextureShader::CreateShader()
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
	result = Compiler.Compile(GetJGDevice(), TT("../JGEngineCore/RenderSystem/ShaderHLSL/TextShader_vs.hlsl"),
		TT("../JGEngineCore/RenderSystem/ShaderHLSL/TextShader_ps.hlsl"));
	if (!result)
	{
		return false;
	}
	// 상수 버퍼 추가
	AddConstantBuffer(TT("MatrixBuffer"), sizeof(SMatrixBuffer_VS));
	AddConstantBuffer(TT("TextBuffer"), sizeof(STextBuffer_PS));


	return true;
}

void JGTextureShader::JGShaderDataSetting(JGShaderData* Data)
{
	//
	Write_RegisterBuffer(TT("MatrixBuffer"), Data, EJGMapType::Write_Discard,
		ERegisterType::Vertex);
	Write_RegisterBuffer(TT("TextBuffer"), Data, EJGMapType::Write_Discard,
		ERegisterType::Pixel);
}

void JGTextureShader::JGTextureSetting(JGTexture* Texture)
{
	GetJGDevice()->GetContext()->PSSetShaderResources(0, 1, Texture->GetEntry());
}
