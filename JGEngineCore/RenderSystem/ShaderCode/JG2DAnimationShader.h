#pragma once
#include"../JGHLSLShaderDevice/JGShader.h"



class JG2DAnimationShader : public JGShader
{
public:
	JG2DAnimationShader();
	virtual ~JG2DAnimationShader();


protected:
	virtual bool CreateShader() override;
	virtual void JGShaderDataSetting(JGShaderData* Data) override;
	virtual void JGTextureSetting(JGTexture* Texture) override;

};