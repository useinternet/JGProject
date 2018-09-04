#include "JG2DSpriteShader.h"
#include"../JGHLSLShaderDevice/JGShaderCompiler.h"
#include"../JGHLSLShaderDevice/JGBuffer.h"
#include"../JGTexture.h"
#include"../JGDeviceD.h"
#include"HLSLConstantBufferDesc.h"
JG2DSpriteShader::JG2DSpriteShader()
{
}

JG2DSpriteShader::~JG2DSpriteShader()
{
}

bool JG2DSpriteShader::CreateShader()
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
	result = Compiler.Compile(GetJGDevice(), TT("../JGEngineCore/RenderSystem/ShaderHLSL/2DSpriteShader_vs.hlsl"),
		TT("../JGEngineCore/RenderSystem/ShaderHLSL/2DSpriteShader_ps.hlsl"));
	if (!result)
	{
		return false;
	}

	// 상수 버퍼 추가
	AddConstantBuffer(TT("MatrixBuffer"), sizeof(SMatrixBuffer_VS));
	// 상수 버퍼 추가
	AddConstantBuffer(TT("2DSpriteBuffer"), sizeof(S2DSpriteBuffer_PS));

	// 샘플러 추가
	AddSamplerState(GetJGDevice(), TT("WrapSampler"), ESamplerType::Wrap);

	return true;
}

void JG2DSpriteShader::JGShaderDataSetting(JGShaderData * Data)
{
	Write_RegisterBuffer(TT("MatrixBuffer"), Data, EJGMapType::Write_Discard,
		ERegisterType::Vertex);
	Write_RegisterBuffer(TT("2DSpriteBuffer"), Data, EJGMapType::Write_Discard,
		ERegisterType::Pixel);
}

void JG2DSpriteShader::JGTextureSetting(JGTexture * Texture)
{
	GetJGDevice()->GetContext()->PSSetShaderResources(0, 1, Texture->GetEntry());
}
