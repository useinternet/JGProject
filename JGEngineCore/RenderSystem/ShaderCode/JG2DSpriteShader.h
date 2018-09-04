#pragma once

#include"../JGHLSLShaderDevice/JGShader.h"

class JG2DSpriteShader : public JGShader
{
public:
	JG2DSpriteShader();
	virtual ~JG2DSpriteShader();

protected:
	virtual bool CreateShader() override;
	virtual void JGShaderDataSetting(JGShaderData* Data) override;
	virtual void JGTextureSetting(JGTexture* Texture) override;

};