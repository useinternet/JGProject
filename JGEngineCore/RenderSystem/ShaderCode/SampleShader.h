#pragma once
#include"../JGHLSLShaderDevice/JGShader.h"
#include"SampleShaderDesc.h"


class SampleShader : public JGShader
{
public:
	SampleShader();
	virtual ~SampleShader();
protected:
	virtual bool CreateShader() override;
	virtual void JGShaderDataSetting(JGShaderData* Data) override;
	virtual void JGTextureSetting(JGTexture* Texture) override;
};