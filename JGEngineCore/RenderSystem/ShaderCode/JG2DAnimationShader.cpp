#include"JG2DAnimationShader.h"
#include"../JGHLSLShaderDevice/JGShaderCompiler.h"
#include"../JGHLSLShaderDevice/JGBuffer.h"
#include"../JGTexture.h"
#include"../JGDeviceD.h"
#include"HLSLConstantBufferDesc.h"
JG2DAnimationShader::JG2DAnimationShader()
{
}

JG2DAnimationShader::~JG2DAnimationShader()
{
}

bool JG2DAnimationShader::CreateShader()
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

	// 컴파일
	result =Compiler.Compile(
		GetJGDevice(), TT("../JGEngineCore/RenderSystem/ShaderHLSL/2DAnimationShader_vs.hlsl"),
		TT("../JGEngineCore/RenderSystem/ShaderHLSL/2DAnimationShader_ps.hlsl"));
	if (!result)
	{
		return false;
	}
	// 상수 버퍼 추가하고
	AddConstantBuffer(TT("MatrixBuffer"), sizeof(SMatrixBuffer_VS));
	AddConstantBuffer(TT("AnimationIncrementBuffer"), sizeof(SAnimationIncrementBuffer_VS));
	AddConstantBuffer(TT("2DSpriteBuffer"), sizeof(S2DSpriteBuffer_PS));
	// 샘플러 추가하고..
	AddSamplerState(GetJGDevice(), TT("WrapSampler"), ESamplerType::Wrap);
	return true;
}

void JG2DAnimationShader::JGShaderDataSetting(JGShaderData * Data)
{
	// 데이터 셋팅
	Write_RegisterBuffer(TT("MatrixBuffer"), Data, EJGMapType::Write_Discard,
		ERegisterType::Vertex);
	Write_RegisterBuffer(TT("AnimationIncrementBuffer"), Data, EJGMapType::Write_Discard,
		ERegisterType::Vertex);
	Write_RegisterBuffer(TT("2DSpriteBuffer"), Data, EJGMapType::Write_Discard,
		ERegisterType::Pixel);
}

void JG2DAnimationShader::JGTextureSetting(JGTexture* Texture)
{
	// 텍스쳐 놓고
	GetJGDevice()->GetContext()->PSSetShaderResources(0, 1, Texture->GetEntry());
}
